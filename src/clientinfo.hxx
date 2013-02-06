/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	clientinfo.hxx - this file is integral part of `gameground' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef CLIENTINFO_HXX_INCLUDED
#define CLIENTINFO_HXX_INCLUDED

#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hsocket.hxx>

#include "logic.hxx"

namespace gameground {

struct OClientInfo {
	bool _valid;
	bool _anonymous;
	typedef yaal::hcore::HSet<HLogic::id_t> logics_t;
	yaal::hcore::HString _login;
	yaal::hcore::HSocket::ptr_t _socket;
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

