/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	server.cxx - this file is integral part of `gameground' project.

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

#include <iostream>

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

namespace
{

HString const& mark( int a_iColor )
	{
	static HString buf;
	buf = "$";
	buf += a_iColor;
	buf += ";###;$7;";
	return ( buf );
	}

}

HServer::HServer( int a_iConnections )
	: HProcess( a_iConnections ), f_iMaxConnections( a_iConnections ),
	f_oSocket( HSocket::TYPE::D_DEFAULT, a_iConnections ), f_oClients(), f_oLogics(), f_oHandlers()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HServer::~HServer( void )
	{
	out << brightred << "<<<GameGround>>>" << lightgray << " server finished." << endl;
	}

int HServer::init_server( int a_iPort )
	{
	M_PROLOG
	f_oSocket.listen ( "0.0.0.0", a_iPort );
	register_file_descriptor_handler ( f_oSocket.get_file_descriptor(), &HServer::handler_connection );
	f_oHandlers[ "shutdown" ] = &HServer::handler_shutdown;
	f_oHandlers[ "quit" ] = &HServer::handler_quit;
	f_oHandlers[ "msg" ] = &HServer::handler_chat;
	f_oHandlers[ "name" ] = &HServer::set_client_name;
	f_oHandlers[ "get_logics" ] = &HServer::get_logics_info;
	f_oHandlers[ "get_players" ] = &HServer::get_players_info;
	f_oHandlers[ "get_games" ] = &HServer::get_games_info;
	f_oHandlers[ "get_game_info" ] = &HServer::get_game_info;
	f_oHandlers[ "create" ] = &HServer::create_game;
	f_oHandlers[ "join" ] = &HServer::join_game;
	f_oHandlers[ "abandon" ] = &HServer::handler_abandon;
	f_oHandlers[ "cmd" ] = &HServer::pass_command;
	HProcess::init ( 3600 );
	out << brightblue << "<<<GameGround>>>" << lightgray << " server started." << endl;
	return ( 0 );
	M_EPILOG
	}

void HServer::broadcast( HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::HIterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		*it->second.f_oSocket << a_roMessage << endl;
	return;
	M_EPILOG
	}

void HServer::broadcast_to_interested( HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::HIterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		if ( ! it->second.f_oLogic )
			*it->second.f_oSocket << a_roMessage << endl;
	return;
	M_EPILOG
	}

void HServer::handler_chat( OClientInfo& a_roInfo, HString const& a_roMessage )
	{
	M_PROLOG
	broadcast_to_interested( "msg:" + a_roInfo.f_oName + ": " + a_roMessage );
	return;
	M_EPILOG
	}

void HServer::set_client_name( OClientInfo& a_roInfo, HString const& a_oName )
	{
	M_PROLOG
	clients_t::HIterator it;
	int const D_MINIMUM_NAME_LENGTH = 4;
	for ( it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		if ( ( it->second.f_oName == a_oName ) && ( it->second.f_oSocket != a_roInfo.f_oSocket ) )
			break;
	if ( a_oName.find_other_than( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ) >= 0 )
		*a_roInfo.f_oSocket << "err:Name may only take form of `[a-zA-Z0-9]+'." << endl;
	else if ( a_oName.get_length() < D_MINIMUM_NAME_LENGTH )
		*a_roInfo.f_oSocket << "err:Your name is too short, it needs to be at least 4 character long." << endl;
	else if ( it != f_oClients.end() )
		*a_roInfo.f_oSocket << "err:Name taken." << endl;
	else
		{
		a_roInfo.f_oName = a_oName;
		broadcast_to_interested( "player:" + a_oName );
		broadcast_to_interested( "msg:" + mark( COLORS::D_FG_BLUE ) + " " + a_oName + " entered the GameGround." );
		}
	return;
	M_EPILOG
	}

void HServer::pass_command( OClientInfo& a_roInfo, HString const& a_oCommand )
	{
	M_PROLOG
	if ( ! a_roInfo.f_oLogic )
		a_roInfo.f_oSocket->write_until_eos( "err:Connect to some game first.\n" );
	else if ( a_roInfo.f_oLogic->process_command( &a_roInfo, a_oCommand ) )
		*a_roInfo.f_oSocket << "err:Server could not comprehend your message: " << a_oCommand << endl;
	return;
	M_EPILOG
	}

void HServer::create_game( OClientInfo& a_roInfo, HString const& a_oArg )
	{
	M_PROLOG
	if ( a_roInfo.f_oName.is_empty() )
		a_roInfo.f_oSocket->write_until_eos( "err:Set your name first.\n" );
	else
		{
		HString l_oType = a_oArg.split( ":", 0 );
		HString l_oConfiguration = a_oArg.split( ":", 1 );
		HString l_oName = l_oConfiguration.split( ",", 0 );
		HLogicFactory& factory = HLogicFactoryInstance::get_instance();
		logics_t::HIterator it = f_oLogics.find( l_oName );
		if ( it != f_oLogics.end() )
			a_roInfo.f_oSocket->write_until_eos( "err:Game already exists.\n" );
		else if ( ! factory.is_type_valid( l_oType ) )
			kick_client( a_roInfo.f_oSocket, _( "No such game type." ) );
		else if ( l_oName.is_empty() )
			kick_client( a_roInfo.f_oSocket, _( "No game name given." ) );
		else
			{
			HLogic::ptr_t l_oLogic;
			try
				{
				l_oLogic = factory.create_logic( l_oType, l_oConfiguration );
				if ( ! l_oLogic->accept_client( &a_roInfo ) )
					{
					f_oLogics[ l_oName ] = l_oLogic;
					a_roInfo.f_oLogic = l_oLogic;
					out << l_oName << "," << l_oType << endl;
					broadcast_to_interested( "player:" + a_roInfo.f_oName + "," + l_oLogic->get_info() );
					}
				else
					a_roInfo.f_oSocket->write_until_eos( "err:Specified configuration is inconsistent.\n" );
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
		a_roInfo.f_oSocket->write_until_eos( "err:Set your name first.\n" );
	else
		{
		logics_t::HIterator it = f_oLogics.find( a_oName );
		if ( it == f_oLogics.end() )
			a_roInfo.f_oSocket->write_until_eos( "err:Game does not exists.\n" );
		else if ( ! it->second->accept_client( &a_roInfo ) )
			{
			a_roInfo.f_oLogic = it->second;
			broadcast_to_interested( "player:" + a_roInfo.f_oName + "," + a_roInfo.f_oLogic->get_info() );
			}
		else
			a_roInfo.f_oSocket->write_until_eos( "err:Game is full.\n" );
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
	out << static_cast<char const* const>( l_oClient->get_host_name() ) << endl;
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
	try
		{
		if ( ! l_oClient )
			kick_client( l_oClient );
		else if ( ( clientIt = f_oClients.find( a_iFileDescriptor ) ) == f_oClients.end() )
			kick_client( l_oClient );
		else if ( ( l_iMsgLength = l_oClient->read_until( l_oMessage ) ) < 0 )
			kick_client( l_oClient, _( "Read failure." ) );
		else if ( l_iMsgLength > 0 )
			{
			out << "<-" << static_cast<char const* const>( l_oMessage ) << endl;
			l_oCommand = l_oMessage.split( ":", 0 );
			l_oArgument = l_oMessage.mid( l_oCommand.get_length() + 1 );
			l_iMsgLength = l_oCommand.get_length();
			if ( l_iMsgLength < 1 )
				kick_client( l_oClient, _( "Malformed data." ) );
			else
				{
				handlers_t::HIterator it = f_oHandlers.find( l_oCommand );
				if ( it != f_oHandlers.end() )
					{
					HLogic::ptr_t l_oLogic = clientIt->second.f_oLogic;
					( this->*it->second )( clientIt->second, l_oArgument );
					if ( ( !! l_oLogic ) && ( ! l_oLogic->active_clients() ) )
						f_oLogics.remove( l_oLogic->get_name() );
					}
				else
					kick_client( l_oClient, _( "Unknown command." ) );
				}
			}
		}
	catch ( HOpenSSLException& )
		{
		kick_client( l_oClient );
		}
	return ( 0 );
	M_EPILOG
	}

void HServer::kick_client( yaal::hcore::HSocket::ptr_t& a_oClient, char const* const a_pcReason )
	{
	M_PROLOG
	M_ASSERT( !! a_oClient );
	int l_iFileDescriptor = a_oClient->get_file_descriptor();
	if ( a_pcReason && a_pcReason[0] )
		*a_oClient << "kck:" << a_pcReason << endl;
	f_oSocket.shutdown_client( l_iFileDescriptor );
	unregister_file_descriptor_handler( l_iFileDescriptor );
	clients_t::HIterator clientIt = f_oClients.find( l_iFileDescriptor );
	M_ASSERT( clientIt != f_oClients.end() );
	out << "client ";
	if ( clientIt->second.f_oName.is_empty() )
		cout << "`unnamed'";
	else
		cout << clientIt->second.f_oName; 
	if ( ! a_pcReason || a_pcReason[ 0 ] )
		{
		HString reason = " was kicked because of: ";
		reason += ( a_pcReason ? a_pcReason : "connection error" );
		if ( ! clientIt->second.f_oName.is_empty() )
			broadcast_to_interested( "msg:" + mark( COLORS::D_FG_BRIGHTRED ) + " " + clientIt->second.f_oName + reason );
		cout << reason;
		}
	else
		cout << " disconnected from server.";
	cout << endl;
	remove_client_from_logic( clientIt->second );
	HString name;
	if ( ! clientIt->second.f_oName.is_empty() )
		name = clientIt->second.f_oName;
	f_oClients.remove( l_iFileDescriptor );
	if ( ! name.is_empty() )
		broadcast_to_interested( "player_quit:" + name );
	return;
	M_EPILOG
	}

void HServer::send_logics_info( OClientInfo& a_roInfo )
	{
	M_PROLOG
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	for( HLogicFactory::creators_t::HIterator it = factory.begin();
			it != factory.end(); ++ it )
		*a_roInfo.f_oSocket << "logic:" << it->second.f_oInfo << endl;
	return;
	M_EPILOG
	}

void HServer::get_logics_info( OClientInfo& a_roInfo, HString const& )
	{
	M_PROLOG
	send_logics_info( a_roInfo );
	return;
	M_EPILOG
	}

void HServer::handler_shutdown( OClientInfo&, HString const& )
	{
	f_bLoop = false;
	return;
	}

void HServer::handler_quit( OClientInfo& a_roInfo, HString const& )
	{
	M_PROLOG
	HString name = a_roInfo.f_oName;
	kick_client( a_roInfo.f_oSocket, "" );
	if ( ! name.is_empty() )
		broadcast_to_interested( "msg:" + mark( COLORS::D_FG_BROWN ) + " " + name + " has left the GameGround." );
	return;
	M_EPILOG
	}

void HServer::handler_abandon( OClientInfo& a_roInfo, HString const& )
	{
	M_PROLOG
	remove_client_from_logic( a_roInfo );
	broadcast_to_interested( "player:" + a_roInfo.f_oName );
	return;
	M_EPILOG
	}

void HServer::remove_client_from_logic( OClientInfo& a_roInfo )
	{
	M_PROLOG
	if ( !! a_roInfo.f_oLogic )
		{
		HLogic::ptr_t l_oLogic = a_roInfo.f_oLogic;
		l_oLogic->kick_client( &a_roInfo );
		a_roInfo.f_oLogic = HLogic::ptr_t();
		if ( ! l_oLogic->active_clients() )
			f_oLogics.remove( l_oLogic->get_name() );
		}
	return;
	M_EPILOG
	}

void HServer::get_players_info( OClientInfo& a_roInfo, HString const& )
	{
	M_PROLOG
	send_players_info( a_roInfo );
	return;
	M_EPILOG
	}

void HServer::get_games_info( OClientInfo& a_roInfo, HString const& )
	{
	M_PROLOG
	send_games_info( a_roInfo );
	return;
	M_EPILOG
	}

void HServer::get_game_info( OClientInfo& a_roInfo, HString const& a_oName )
	{
	M_PROLOG
	send_game_info( a_roInfo, a_oName );
	return;
	M_EPILOG
	}

void HServer::send_players_info( OClientInfo& a_roInfo )
	{
	M_PROLOG
	for( clients_t::HIterator it = f_oClients.begin();
			it != f_oClients.end(); ++ it )
		{
		if ( ! it->second.f_oName.is_empty() )
			{
			*a_roInfo.f_oSocket << "player:" << it->second.f_oName;
			if ( !! it->second.f_oLogic )
				*a_roInfo.f_oSocket << "," << it->second.f_oLogic->get_info();
			*a_roInfo.f_oSocket << endl;
			}
		}
	return;
	M_EPILOG
	}

void HServer::send_games_info( OClientInfo& a_roInfo )
	{
	M_PROLOG
	for( logics_t::HIterator it = f_oLogics.begin();
			it != f_oLogics.end(); ++ it )
		*a_roInfo.f_oSocket << "game:" << it->second->get_info() << endl;
	return;
	M_EPILOG
	}

void HServer::send_game_info( OClientInfo& /*a_roInfo*/, HString const& )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

