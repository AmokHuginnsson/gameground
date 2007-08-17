/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	server.cxx - this file is integral part of `galaxy' project.

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

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )
#include "server.h"

#include "setup.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

HServer::HServer( int a_iConnections )
	: HProcess( a_iConnections ), f_iMaxConnections( a_iConnections ),
	f_oSocket( HSocket::D_DEFAULTS, a_iConnections ), f_oLogics()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HServer::init_server( int a_iPort )
	{
	M_PROLOG
	f_oSocket.listen ( "0.0.0.0", a_iPort );
	register_file_descriptor_handler ( f_oSocket.get_file_descriptor ( ), &HServer::handler_connection );
	HProcess::init ( 3600 );
	for ( logics_t::HIterator it = f_oLogics.begin(); it != f_oLogics.end(); ++ it )
		it->second->set_socket( &f_oSocket );
	return ( 0 );
	M_EPILOG
	}

int HServer::handler_connection( int )
	{
	M_PROLOG
	HSocket::ptr_t l_oClient = f_oSocket.accept();
	M_ASSERT( !! l_oClient );
	register_file_descriptor_handler( l_oClient->get_file_descriptor(), &HServer::handler_message );
	if ( f_oSocket.get_client_count() >= f_iMaxConnections )
		{
		unregister_file_descriptor_handler( f_oSocket.get_file_descriptor() );
		f_oSocket.close();
		}
	fprintf( stdout, "%s\n", static_cast<char const* const>( l_oClient->get_host_name() ) );
	return ( 0 );
	M_EPILOG
	}

int HServer::handler_message( int a_iFileDescriptor )
	{
	M_PROLOG
	int l_iMsgLength = 0;
	HString l_oMessage;
	HString l_oArgument;
	HString l_oCommand;
	HSocket::ptr_t l_oClient = f_oSocket.get_client( a_iFileDescriptor );
	if ( !! l_oClient )
		{
		if ( ( l_iMsgLength = l_oClient->read_until( l_oMessage ) ) > 0 )
			{
			fprintf( stdout, "<-%s\n", static_cast<char const* const>( l_oMessage ) );
			l_oCommand = l_oMessage.split( ":", 0 );
			l_oArgument = l_oMessage.mid( l_oCommand.get_length() + 1 );
			l_iMsgLength = l_oArgument.get_length();
			if ( ( l_iMsgLength > 1 ) && ( l_oCommand == "GLX" ) )
				f_roGalaxy.process_command( a_iFileDescriptor, l_oArgument );
			else if ( l_oCommand == "QUIT" )
				f_bLoop = false;
			}
		else if ( l_iMsgLength < 0 )
			{
			unregister_file_descriptor_handler( a_iFileDescriptor );
			f_oSocket.shutdown_client( a_iFileDescriptor );
			if ( ! f_oSocket.get_client_count() )
				f_bLoop = false;
			}
		}
	else
		{
		unregister_file_descriptor_handler( a_iFileDescriptor );
		if ( ! f_oSocket.get_client_count() )
			f_bLoop = false;
		}
	return ( 0 );
	M_EPILOG
	}

