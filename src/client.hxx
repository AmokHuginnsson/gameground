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
	HClient( yaal::hcore::HStreamInterface::ptr_t const& );
	~HClient( void );
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
	void read( yaal::hcore::HString& );
};

}

#endif /* not CLIENTINFO_HXX_INCLUDED */

