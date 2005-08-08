/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	galaxyd.c - this file is integral part of `galaxy' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <stdhapi.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "galaxyd.h"

#include "setup.h"

using namespace std;
using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::tools;
using namespace stdhapi::tools::util;

int main_server ( void )
	{
	char buf [ 2 ] = "\0";
	HSocket l_oSocket ( HSocket::D_DEFAULTS, setup.f_iMaximumNumberOfClients );
	l_oSocket.listen ( "0.0.0.0", setup.f_iPort );
	HSocket * l_poClient = l_oSocket.accept ( );
	while ( 1 )
		{
		l_poClient->read ( buf, 1 );
		if ( fprintf ( stdout, buf ) <= 0 )
			break;
		}
	return ( 0 );
	}

