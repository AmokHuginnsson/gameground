/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef CLIENTINFO_HXX_INCLUDED
#define CLIENTINFO_HXX_INCLUDED

#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hstreaminterface.hxx>

#include "logic.hxx"

namespace gameground {

class HClient {
public:
	typedef yaal::hcore::HSet<HLogic::id_t> logics_t;
private:
	bool _valid;
	bool _authenticated;
	yaal::hcore::HString _login;
	yaal::hcore::HStreamInterface::ptr_t _socket;
	logics_t _logics;
public:
	HClient( yaal::hcore::HStreamInterface::ptr_t const& sock_ )
		: _valid( true )
		, _authenticated( false )
		, _login()
		, _socket( sock_ )
		, _logics() {
	}
	~HClient( void ) {
		M_PROLOG
		M_ASSERT( _logics.is_empty() );
		return;
		M_EPILOG
	};
	yaal::hcore::HString const& login( void ) const {
		return ( _login );
	}
	void set_login( yaal::hcore::HString const& login_ ) {
		M_PROLOG
		M_ASSERT( _login.is_empty() );
		_login = login_;
		return;
		M_EPILOG
	}
	bool is_valid( void ) const {
		return ( _valid );
	}
	void invalidate( void ) {
		_valid = false;
	}
	bool is_authenticated( void ) const {
		return ( _authenticated );
	}
	void authenticate( void ) {
		_authenticated = true;
	}
	yaal::hcore::HStreamInterface::ptr_t const& sock( void ) const {
		return ( _socket );
	}
	logics_t const& logics( void ) const {
		return ( _logics );
	}
	void enter( HLogic::id_t id_ ) {
		M_PROLOG
		_logics.insert( id_ );
		return;
		M_EPILOG
	}
	void leave( HLogic::id_t id_ ) {
		M_PROLOG
		_logics.erase( id_ );
		return;
		M_EPILOG
	}
	void send( yaal::hcore::HString const& message_ ) {
		*_socket << message_;
	}
	void read( yaal::hcore::HString& line_ ) {
		M_PROLOG
		_socket->read_until( line_ );
		return;
		M_EPILOG
	}
};

}

#endif /* not CLIENTINFO_HXX_INCLUDED */

