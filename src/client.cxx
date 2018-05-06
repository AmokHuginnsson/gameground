/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hexception.hxx>
#include <yaal/hcore/safe_int.hxx>
#include <yaal/tools/streamtools.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "client.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground {

enum class OPCODE {
	FRAGMENT = 0x0,
	TEXT     = 0x1,
	BINARY   = 0x2,
	CLOSE    = 0x8,
	PING     = 0x9,
	PONG     = 0xa
};

struct WebSockFragmentHeader {
	bool _final;
	OPCODE _opcode;
	bool _masked;
	int _len;
	bool _valid;
	WebSockFragmentHeader( u16_t raw_ )
		: _final( false )
		, _opcode( OPCODE::FRAGMENT )
		, _masked( false )
		, _len( 0 )
		, _valid( false ) {
		raw_ = hton( raw_ );
		_final = ( ( raw_ >> 15 ) & 1 ) != 0;
		int reserved = ( raw_ >> 12 ) & 7;
		_opcode = static_cast<OPCODE>( ( raw_ >> 8 ) & 15 );
		_masked = ( ( raw_ >> 7 ) & 1 ) != 0;
		_len = raw_ & 127;
		_valid = ( reserved == 0 )
			&& _masked
			&& (
				( _opcode == OPCODE::FRAGMENT )
				|| ( _opcode == OPCODE::TEXT )
				|| ( _opcode == OPCODE::BINARY )
				|| ( _opcode == OPCODE::CLOSE )
				|| ( _opcode == OPCODE::PING )
				|| ( _opcode == OPCODE::PONG )
			);
	}
	WebSockFragmentHeader( yaal::hcore::HUTF8String const& str_ )
		: _final( false )
		, _opcode( OPCODE::FRAGMENT )
		, _masked( false )
		, _len( 0 )
		, _valid( false ) {
		_final = true;
		_len = static_cast<int>( str_.byte_count() );
		if ( _len > 65535 ) {
			_len = 127;
		} else if ( _len > 126 ) {
			_len = 126;
		}
		_opcode = OPCODE::TEXT;
		_valid = true;
	}
	u16_t raw( void ) const {
		u16_t r( 0 );
		r = static_cast<u16_t>( _len ) & 127;
		r |= static_cast<u16_t>( _masked ? ( 1 << 7 ) : 0 );
		r |= static_cast<u16_t>( static_cast<u16_t>( _opcode ) << 8 );
		r |= static_cast<u16_t>( _final ? ( 1 << 15 ) : 0 );
		return ( hton( r ) );
	}
	bool is_valid( void ) const {
		return ( _valid );
	}
	char const* opcode( void ) const {
		char const* oc( "reserved" );
		switch ( _opcode ) {
			case ( OPCODE::FRAGMENT ): oc = "fragment"; break;
			case ( OPCODE::TEXT ):     oc = "text";     break;
			case ( OPCODE::BINARY ):   oc = "binary";   break;
			case ( OPCODE::CLOSE ):    oc = "close";    break;
			case ( OPCODE::PING ):     oc = "ping";     break;
			case ( OPCODE::PONG ):     oc = "pong";     break;
		}
		return ( oc );
	}
};

inline void apply_mask( void* data_, int size_, u32_t mask_ ) {
	u32_t* d( static_cast<u32_t*>( data_ ) );
	int const maskSize( static_cast<int>( sizeof ( mask_ ) ) );
	for ( int i( 0 ); i < ( size_ / maskSize ); ++ i ) {
		d[i] ^= mask_;
	}
	int rest( size_ % maskSize );
	u8_t* o( static_cast<u8_t*>( data_ ) + size_ - rest );
	u8_t* m( reinterpret_cast<u8_t*>( &mask_ ) );
	for ( int i( 0 ); i < rest; ++ i ) {
		o[i] ^= m[i];
	}
	return;
}

inline HStreamInterface& operator << ( HStreamInterface& os, WebSockFragmentHeader const& wsfh ) {
	os << "final = " << wsfh._final << "\n"
		<< "opcode = " << wsfh.opcode() << "\n"
		<< "masked = " << wsfh._masked << "\n"
		<< "len = " << wsfh._len << endl;
	return ( os );
}

HClient::HClient( yaal::hcore::HStreamInterface::ptr_t const& sock_ )
	: _webSocket( false )
	, _valid( true )
	, _authenticated( false )
	, _login()
	, _socket( sock_ )
	, _buffer()
	, _utf8()
	, _logics() {
}

HClient::~HClient( void ) {
	M_PROLOG
	M_ASSERT( _logics.is_empty() );
	return;
	M_EPILOG
}

void HClient::upgrade( void ) {
	_webSocket = true;
}

void HClient::send( yaal::hcore::HString const& message_ ) {
	M_PROLOG
	if ( ! _webSocket ) {
		*_socket << message_;
	} else {
		do {
			_utf8.assign( message_ );
			WebSockFragmentHeader wsfh( _utf8 );
			u16_t header( wsfh.raw() );
			int toWrite( sizeof ( header ) );
			if ( _socket->write( &header, toWrite ) != toWrite ) {
				OUT << "failed to write header" << endl;
				break;
			}
			if ( wsfh._len == 127 ) {
				u64_t len( static_cast<u64_t>( _utf8.byte_count() ) );
				toWrite = sizeof ( len );
				if ( _socket->write( &header, toWrite ) != toWrite ) {
					OUT << "failed to write 64-bit len" << endl;
					break;
				}
			} else if ( wsfh._len == 126 ) {
				u16_t len( static_cast<u16_t>( _utf8.byte_count() ) );
				toWrite = sizeof ( len );
				if ( _socket->write( &header, toWrite ) != toWrite ) {
					OUT << "failed to write 16-bit len" << endl;
					break;
				}
			}
			_socket->write( _utf8.c_str(), _utf8.byte_count() );
		} while ( false );
	}
	return;
	M_EPILOG
}

int long HClient::read( yaal::hcore::HString& line_ ) {
	M_PROLOG
	int long nRead( -1 );
	if ( ! _webSocket ) {
		nRead = _socket->read_until( line_, "\n" );
	} else {
		nRead = 0;
		do {
			u16_t header( 0 );
			int toRead( sizeof ( header ) );
			if ( _socket->read( &header, toRead ) != toRead ) {
				break;
			}
			WebSockFragmentHeader wsfh( header );
			if ( ! wsfh.is_valid() ) {
				OUT << "invalid header" << endl;
				break;
			}
			OUT << "wsfh:\n" << wsfh << endl;
			int payloadLen( wsfh._len );
			if ( payloadLen == 127 ) {
				u64_t len( 0 );
				toRead = sizeof ( len );
				if ( _socket->read( &len, toRead ) != toRead ) {
					OUT << "failed to read 64-bit len" << endl;
					break;
				}
				payloadLen = safe_int::cast<int>( len );
			} else if ( payloadLen == 126 ) {
				u16_t len( 0 );
				toRead = sizeof ( len );
				if ( _socket->read( &len, toRead ) != toRead ) {
					OUT << "failed to read 16-bit len" << endl;
					break;
				}
				payloadLen = len;
			}
			u32_t mask( 0 );
			toRead = sizeof ( mask );
			if ( _socket->read( &mask, toRead ) != toRead ) {
				OUT << "failed to read mask" << endl;
				break;
			}
			_buffer.realloc( payloadLen );
			if ( _socket->read( _buffer.raw(), payloadLen ) != payloadLen ) {
				OUT << "failed to read payload" << endl;
				break;
			}
			apply_mask( _buffer.raw(), payloadLen, mask );
			line_.assign( _buffer.get<char>(), payloadLen );
			nRead = payloadLen;
		} while ( false );
	}
	if ( nRead > 1 ) {
		int long len( line_.get_size() );
		line_.trim_right( "\r\n" );
		nRead -= ( len - line_.get_length() );
	}
	return ( nRead );
	M_EPILOG
}

void HClient::set_login( yaal::hcore::HString const& login_ ) {
	M_PROLOG
	M_ASSERT( _login.is_empty() );
	_login = login_;
	return;
	M_EPILOG
}

void HClient::enter( HLogic::id_t id_ ) {
	M_PROLOG
	_logics.insert( id_ );
	return;
	M_EPILOG
}

void HClient::leave( HLogic::id_t id_ ) {
	M_PROLOG
	_logics.erase( id_ );
	return;
	M_EPILOG
}

void HClient::invalidate( void ) {
	_valid = false;
}

void HClient::authenticate( void ) {
	_authenticated = true;
}

}

