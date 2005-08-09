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

class HServer : public HProcess
	{
protected:
	/*{*/
	HSocket f_oSocket;
	/*}*/
public:
	/*{*/
	HServer ( int );
	int init_server ( int );
	using HProcess::run;
	/*}*/
protected:
	/*{*/
	int handler_connection ( int );
	int handler_message ( int );
	/*}*/
	};

HServer::HServer ( int a_iMaximumNumberOfClients )
	: HProcess ( a_iMaximumNumberOfClients ),
	f_oSocket ( HSocket::D_DEFAULTS, a_iMaximumNumberOfClients )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HServer::init_server ( int a_iPort )
	{
	M_PROLOG
	f_oSocket.listen ( "0.0.0.0", a_iPort );
	M_REGISTER_FILE_DESCRIPTOR_HANDLER ( f_oSocket.get_file_descriptor ( ), HServer::handler_connection );
	HProcess::init ( 3600 );
	return ( 0 );
	M_EPILOG
	}

int HServer::handler_connection ( int )
	{
	M_PROLOG
	HSocket * l_poClient = f_oSocket.accept ( );
	M_REGISTER_FILE_DESCRIPTOR_HANDLER ( l_poClient->get_file_descriptor ( ), HServer::handler_message );
	return ( 0 );
	M_EPILOG
	}

int HServer::handler_message ( int a_iFileDescriptor )
	{
	M_PROLOG
	int l_iFileDescriptor = - 1;
	HString l_oMessage;
	HSocket * l_poClient = f_oSocket.get_client ( a_iFileDescriptor );
	M_ASSERT ( l_poClient );
	if ( l_poClient->read_until ( l_oMessage ) > 0 )
		{
		f_oSocket.rewind_client_list ( );
		while ( f_oSocket.get_client_next ( l_iFileDescriptor, l_poClient ) )
			if ( l_iFileDescriptor != a_iFileDescriptor )
				l_poClient->write ( l_oMessage, l_oMessage.get_length ( ) );
		fprintf ( stdout, l_oMessage );
		}
	else
		{
		unregister_file_descriptor_handler ( a_iFileDescriptor );
		f_oSocket.shutdown_client ( a_iFileDescriptor );
		}
	return ( 0 );
	M_EPILOG
	}

int main_server ( void )
	{
	HServer l_oServer ( setup.f_iMaximumNumberOfClients );
	l_oServer.init_server ( setup.f_iPort );
	l_oServer.run ( );
	return ( 0 );
	}

