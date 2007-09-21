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
#include "logicfactory.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

HServer::HServer( int a_iConnections )
	: HProcess( a_iConnections ), f_iMaxConnections( a_iConnections ),
	f_oSocket( HSocket::TYPE::D_DEFAULT, a_iConnections ), f_oClients(), f_oLogics(), f_oHandlers()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HServer::init_server( int a_iPort )
	{
	M_PROLOG
	f_oSocket.listen ( "0.0.0.0", a_iPort );
	register_file_descriptor_handler ( f_oSocket.get_file_descriptor(), &HServer::handler_connection );
	f_oHandlers[ "msg" ] = &HServer::broadcast;
	f_oHandlers[ "name" ] = &HServer::set_client_name;
	f_oHandlers[ "cmd" ] = &HServer::pass_command;
	f_oHandlers[ "create" ] = &HServer::create_game;
	f_oHandlers[ "join" ] = &HServer::join_game;
	HProcess::init ( 3600 );
	return ( 0 );
	M_EPILOG
	}

void HServer::broadcast( OClientInfo&, HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::HIterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		it->second.f_oSocket->write_until_eos ( a_roMessage );
	return;
	M_EPILOG
	}

void HServer::set_client_name( OClientInfo& a_roInfo, HString const& a_oName )
	{
	M_PROLOG
	clients_t::HIterator it;
	for ( it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		{
		if ( ( it->second.f_oName == a_oName ) && ( it->second.f_oSocket != a_roInfo.f_oSocket ) )
			{
			a_roInfo.f_oSocket->write_until_eos ( "err:Name taken." );
			break;
			}
		}
	if ( it == f_oClients.end() )
		a_roInfo.f_oName = a_oName;
	return;
	M_EPILOG
	}

void HServer::pass_command( OClientInfo& a_roInfo, HString const& a_oCommand )
	{
	M_PROLOG
	if ( ! a_roInfo.f_oLogic )
		a_roInfo.f_oSocket->write_until_eos( "err:Connect to some game first." );
	else
		a_roInfo.f_oLogic->process_command( &a_roInfo, a_oCommand );
	return;
	M_EPILOG
	}

void HServer::create_game( OClientInfo& a_roInfo, HString const& a_oArg )
	{
	M_PROLOG
	if ( a_roInfo.f_oName.is_empty() )
		a_roInfo.f_oSocket->write_until_eos( "err:Set your name first." );
	else
		{
		HString l_oType = a_oArg.split( ":", 0 );
		HString l_oName = a_oArg.split( ":", 1 );
		HLogicFactory& factory = HLogicFactoryInstance::get_instance();
		logics_t::HIterator it = f_oLogics.find( l_oName );
		if ( it != f_oLogics.end() )
			a_roInfo.f_oSocket->write_until_eos( "err:Game already exists." );
		else if ( ! factory.is_type_valid( l_oType ) )
			kick_client( a_roInfo.f_oSocket, _( "No such game type." ) );
		else if ( l_oName.is_empty() )
			kick_client( a_roInfo.f_oSocket, _( "No game name given." ) );
		else
			{
			HLogic::ptr_t l_oLogic;
			try
				{
				f_oLogics[ l_oName ] = l_oLogic = factory.create_logic( l_oType, l_oName );
				if ( ! l_oLogic->accept_client( &a_roInfo ) )
					a_roInfo.f_oLogic = l_oLogic;
				}
			catch ( HLogicException& e )
				{
				kick_client( a_roInfo.f_oSocket, e.what() );
				}
			}
		}
	return;
	M_EPILOG
	}

void HServer::join_game( OClientInfo& a_roInfo, HString const& a_oName )
	{
	M_PROLOG
	if ( a_roInfo.f_oName.is_empty() )
		a_roInfo.f_oSocket->write_until_eos( "err:Set your name first." );
	else
		{
		logics_t::HIterator it = f_oLogics.find( a_oName );
		if ( it == f_oLogics.end() )
			a_roInfo.f_oSocket->write_until_eos( "err:Game does not exists." );
		else if ( ! it->second->accept_client( &a_roInfo ) )
			a_roInfo.f_oLogic = it->second;
		}
	return;
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
	else
		f_oClients[ l_oClient->get_file_descriptor() ].f_oSocket = l_oClient;
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
	clients_t::HIterator clientIt;
	HSocket::ptr_t l_oClient = f_oSocket.get_client( a_iFileDescriptor );
	if ( ! l_oClient )
		kick_client( l_oClient );
	else if ( ( clientIt = f_oClients.find( a_iFileDescriptor ) ) == f_oClients.end() )
		kick_client( l_oClient );
	else if ( ( l_iMsgLength = l_oClient->read_until( l_oMessage ) ) < 0 )
		kick_client( l_oClient, _( "Read failure." ) );
	else if ( l_iMsgLength > 0 )
		{
		fprintf( stdout, "<-%s\n", static_cast<char const* const>( l_oMessage ) );
		l_oCommand = l_oMessage.split( ":", 0 );
		l_oArgument = l_oMessage.mid( l_oCommand.get_length() + 1 );
		l_iMsgLength = l_oArgument.get_length();
		if ( l_iMsgLength <= 1 )
			{
			if ( l_oCommand == "QUIT" )
				f_bLoop = false;
			else
				kick_client( l_oClient, _( "Malformed data." ) );
			}
		else
			{
			handlers_t::HIterator it = f_oHandlers.find( l_oCommand );
			if ( it != f_oHandlers.end() )
				{
				( this->*it->second )( clientIt->second, l_oArgument );
				if ( ( !! clientIt->second.f_oLogic ) && ( ! clientIt->second.f_oLogic->active_clients() ) )
					f_oLogics.remove( clientIt->second.f_oLogic->get_name() );
				}
			else
				kick_client( l_oClient, _( "Unknown command." ) );
			}
		}
	return ( 0 );
	M_EPILOG
	}

void HServer::kick_client( yaal::hcore::HSocket::ptr_t& a_oClient, char const* const a_pcReason )
	{
	M_PROLOG
	M_ASSERT( !! a_oClient );
	int l_iFileDescriptor = a_oClient->get_file_descriptor();
	a_oClient->write_until_eos( a_pcReason );
	f_oSocket.shutdown_client( l_iFileDescriptor );
	unregister_file_descriptor_handler( l_iFileDescriptor );
	clients_t::HIterator clientIt = f_oClients.find( l_iFileDescriptor );
	M_ASSERT( clientIt != f_oClients.end() );
	if ( ( !! clientIt->second.f_oLogic ) && ( ! clientIt->second.f_oLogic->active_clients() ) )
		f_oLogics.remove( clientIt->second.f_oLogic->get_name() );
	f_oClients.remove( l_iFileDescriptor );
	if ( ! f_oSocket.get_client_count() )
		f_bLoop = false;
	return;
	M_EPILOG
	}

