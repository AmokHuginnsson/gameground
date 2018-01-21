/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef CLIENTINFO_HXX_INCLUDED
#define CLIENTINFO_HXX_INCLUDED

#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hstreaminterface.hxx>

#include "logic.hxx"

namespace gameground {

struct OClientInfo {
	bool _valid;
	bool _anonymous;
	typedef yaal::hcore::HSet<HLogic::id_t> logics_t;
	yaal::hcore::HString _login;
	yaal::hcore::HStreamInterface::ptr_t _socket;
	logics_t _logics;
	OClientInfo( void ) : _valid( true ), _anonymous( false ), _login(), _socket(), _logics() {}
	~OClientInfo( void ) {
		M_PROLOG
		M_ASSERT( _logics.is_empty() );
		return;
		M_EPILOG
	};
};

}

#endif /* not CLIENTINFO_HXX_INCLUDED */

