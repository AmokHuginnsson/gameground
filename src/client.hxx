/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef CLIENTINFO_HXX_INCLUDED
#define CLIENTINFO_HXX_INCLUDED

#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hstreaminterface.hxx>

#include "logic.hxx"

namespace gameground {

struct WebSockFragmentHeader {
	enum class OPCODE {
		FRAGMENT = 0x0,
		TEXT     = 0x1,
		BINARY   = 0x2,
		CLOSE    = 0x8,
		PING     = 0x9,
		PONG     = 0xa
	};
	bool _final;
	OPCODE _opcode;
	bool _masked;
	int _len;
	bool _valid;
	WebSockFragmentHeader( yaal::u16_t );
	WebSockFragmentHeader( yaal::hcore::HUTF8String const&, OPCODE );
	yaal::u16_t raw( void ) const;
	bool is_valid( void ) const {
		return ( _valid );
	}
	char const* opcode( void ) const;
};

class HClient {
public:
	typedef yaal::hcore::HSet<HLogic::id_t> logics_t;
private:
	bool _webSocket;
	bool _valid;
	bool _authenticated;
	yaal::hcore::HString _login;
	yaal::hcore::HStreamInterface::ptr_t _socket;
	yaal::hcore::HChunk _buffer;
	yaal::hcore::HUTF8String _utf8;
	logics_t _logics;
public:
	HClient( yaal::hcore::HStreamInterface::ptr_t const& );
	HClient( HClient&& ) = default;
	HClient& operator = ( HClient&& ) = default;
	~HClient( void );
	void upgrade( void );
	yaal::hcore::HString const& login( void ) const {
		return ( _login );
	}
	void set_login( yaal::hcore::HString const& );
	bool is_valid( void ) const {
		return ( _valid );
	}
	void invalidate( void );
	bool is_authenticated( void ) const {
		return ( _authenticated );
	}
	void authenticate( void );
	yaal::hcore::HStreamInterface::ptr_t const& sock( void ) const {
		return ( _socket );
	}
	logics_t const& logics( void ) const {
		return ( _logics );
	}
	void enter( HLogic::id_t id_ );
	void leave( HLogic::id_t id_ );
	void send( yaal::hcore::HString const& );
	void pong( yaal::hcore::HString const& );
	int long read( yaal::hcore::HString& );
private:
	int long read_web_socket( yaal::hcore::HString& );
	void send_web_socket( yaal::hcore::HString const&, WebSockFragmentHeader::OPCODE = WebSockFragmentHeader::OPCODE::TEXT );
	HClient( HClient const& ) = delete;
	HClient& operator = ( HClient const& ) = delete;
};

}

#endif /* not CLIENTINFO_HXX_INCLUDED */

