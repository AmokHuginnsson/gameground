/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	clientinfo.hxx - this file is integral part of `gameground' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef CLIENTINFO_HXX_INCLUDED
#define CLIENTINFO_HXX_INCLUDED

#include <yaal/hcore/hset.hxx>

#include "logic.hxx"

namespace gameground
{

struct OClientInfo
	{
	bool _valid;
	bool _anonymous;
	typedef yaal::hcore::HSet<HLogic::id_t> logics_t;
	yaal::hcore::HString _login;
	yaal::hcore::HSocket::ptr_t _socket;
	logics_t _logics;
	OClientInfo( void ) : _valid( true ), _anonymous( false ), _login(), _socket(), _logics() {}
	~OClientInfo( void )
		{
		M_PROLOG
		M_ASSERT( _logics.is_empty() );
		return;
		M_EPILOG
		};
	};

}

#endif /* not CLIENTINFO_HXX_INCLUDED */

