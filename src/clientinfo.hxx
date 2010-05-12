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

#include "logic.hxx"

namespace gameground
{

struct OClientInfo
	{
	bool _anonymous;
	yaal::hcore::HString f_oLogin;
	yaal::hcore::HSocket::ptr_t f_oSocket;
	HLogic::ptr_t f_oLogic;
	OClientInfo( void ) : _anonymous( true ), f_oLogin(), f_oSocket(), f_oLogic() {}
	};

}

#endif /* not CLIENTINFO_HXX_INCLUDED */

