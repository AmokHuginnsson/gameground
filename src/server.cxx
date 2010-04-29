/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	server.cxx - this file is integral part of `gameground' project.

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

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "server.hxx"

#include "setup.hxx"
#include "logicfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace yaal::dbwrapper;

namespace gameground
{

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

static int const MAX_GAME_NAME_LENGTH = 20;
#define LEGEAL_CHARACTER_SET_BASE "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-"
static int const CONSTR_CHAR_SET_LOGIN_NAME = 0;
static int const CONSTR_CHAR_SET_GAME_NAME = 1;
char const* const LEGEAL_CHARACTER_SET[] = { LEGEAL_CHARACTER_SET_BASE, " "LEGEAL_CHARACTER_SET_BASE };
char const* const HServer::PROTOCOL::ABANDON = "abandon";
char const* const HServer::PROTOCOL::CMD = "cmd";
char const* const HServer::PROTOCOL::CREATE = "create";
char const* const HServer::PROTOCOL::ERR = "err";
char const* const HServer::PROTOCOL::GAME = "game";
char const* const HServer::PROTOCOL::GET_GAMES = "get_games";
char const* const HServer::PROTOCOL::GET_GAME_INFO = "get_game_info";
char const* const HServer::PROTOCOL::GET_LOGISTICS = "get_logics";
char const* const HServer::PROTOCOL::GET_PLAYERS = "get_players";
char const* const HServer::PROTOCOL::JOIN = "join";
char const* const HServer::PROTOCOL::KCK = "kck";
char const* const HServer::PROTOCOL::LOGIC = "logic";
char const* const HServer::PROTOCOL::MSG = "msg";
char const* const HServer::PROTOCOL::LOGIN = "login";
char const* const HServer::PROTOCOL::PLAYER = "player";
char const* const HServer::PROTOCOL::PLAYER_QUIT = "player_quit";
char const* const HServer::PROTOCOL::QUIT = "quit";
char const* const HServer::PROTOCOL::SEP = ":";
char const* const HServer::PROTOCOL::SEPP = ",";
char const* const HServer::PROTOCOL::SHUTDOWN = "shutdown";

HServer::HServer( int a_iConnections )
	: f_iMaxConnections( a_iConnections ),
	f_oSocket( HSocket::socket_type_t( HSocket::TYPE::DEFAULT ) | HSocket::TYPE::NONBLOCKING | HSocket::TYPE::SSL_SERVER, a_iConnections ),
	f_oClients(), f_oLogics(), f_oHandlers(), _out(), _db( HDataBase::get_connector() ), _dispatcher( a_iConnections, 3600 )
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
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.initialize_globals();
	_db->connect( setup.f_oDatabasePath, setup.f_oDatabaseLogin, setup.f_oDatabasePassword );
	f_oSocket.listen( "0.0.0.0", a_iPort );
	_dispatcher.register_file_descriptor_handler( f_oSocket.get_file_descriptor(), bound_call( &HServer::handler_connection, this, _1 ) );
	f_oHandlers[ PROTOCOL::SHUTDOWN ] = &HServer::handler_shutdown;
	f_oHandlers[ PROTOCOL::QUIT ] = &HServer::handler_quit;
	f_oHandlers[ PROTOCOL::MSG ] = &HServer::handler_chat;
	f_oHandlers[ PROTOCOL::LOGIN ] = &HServer::handle_login;
	f_oHandlers[ PROTOCOL::GET_LOGISTICS ] = &HServer::get_logics_info;
	f_oHandlers[ PROTOCOL::GET_PLAYERS ] = &HServer::get_players_info;
	f_oHandlers[ PROTOCOL::GET_GAMES ] = &HServer::get_games_info;
	f_oHandlers[ PROTOCOL::GET_GAME_INFO ] = &HServer::get_game_info;
	f_oHandlers[ PROTOCOL::CREATE ] = &HServer::create_game;
	f_oHandlers[ PROTOCOL::JOIN ] = &HServer::join_game;
	f_oHandlers[ PROTOCOL::ABANDON ] = &HServer::handler_abandon;
	f_oHandlers[ PROTOCOL::CMD ] = &HServer::pass_command;
	out << brightblue << "<<<GameGround>>>" << lightgray << " server started." << endl;
	return ( 0 );
	M_EPILOG
	}

void HServer::broadcast( HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::iterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		*it->second.f_oSocket << a_roMessage << endl;
	return;
	M_EPILOG
	}

void HServer::broadcast_to_interested( HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::iterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		if ( ! it->second.f_oLogic )
			*it->second.f_oSocket << a_roMessage << endl;
	return;
	M_EPILOG
	}

void HServer::handler_chat( OClientInfo& a_roInfo, HString const& a_roMessage )
	{
	M_PROLOG
	broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP << a_roInfo.f_oLogin << ": " << a_roMessage << _out );
	return;
	M_EPILOG
	}

void HServer::handle_login( OClientInfo& a_roInfo, HString const& a_oLoginInfo )
	{
	M_PROLOG
	clients_t::iterator it;
	int const MINIMUM_NAME_LENGTH = 4;
	HString login( get_token( a_oLoginInfo, ":", 0 ) );
	HString password( get_token( a_oLoginInfo, ":", 1 ) );
	for ( it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		if ( ( ! strcasecmp( it->second.f_oLogin, login ) ) && ( it->second.f_oSocket != a_roInfo.f_oSocket ) )
			break;
	if ( login.find_other_than( LEGEAL_CHARACTER_SET[ CONSTR_CHAR_SET_LOGIN_NAME ] ) >= 0 )
		*a_roInfo.f_oSocket << "err:Name may only take form of `[a-zA-Z0-9]+'." << endl;
	else if ( login.get_length() < MINIMUM_NAME_LENGTH )
		*a_roInfo.f_oSocket << "err:Your name is too short, it needs to be at least 4 character long." << endl;
	else if ( it != f_oClients.end() )
		*a_roInfo.f_oSocket << "err:" << login << " already logged in." << endl;
	else
		{
		HRecordSet::ptr_t rs = _db->query( lexical_cast<HString>( HFormat( "SELECT COUNT(*) FROM tbl_user WHERE login = LOWER('%s') AND password = LOWER('%s');" ) % login % password ) );
		M_ENSURE( !! rs );
		HRecordSet::iterator row = rs->begin();
		if ( row == rs->end() )
			{
			out << _db->get_error() << endl;
			M_ENSURE( ! "database query error" );
			}
		if ( lexical_cast<int>( row[0] ) == 1 )
			{
			a_roInfo.f_oLogin = login;
			broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << login << _out );
			broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLORS::FG_BLUE ) << " " << login << " entered the GameGround." << _out );
			}
		else
			*a_roInfo.f_oSocket << "err:Login failed." << endl;
		}
	return;
	M_EPILOG
	}

void HServer::pass_command( OClientInfo& a_roInfo, HString const& a_oCommand )
	{
	M_PROLOG
	if ( ! a_roInfo.f_oLogic )
		a_roInfo.f_oSocket->write_until_eos( "err:Connect to some game first.\n" );
	else
		{
		HString msg;
		try
			{
			if ( a_roInfo.f_oLogic->process_command( &a_roInfo, a_oCommand ) )
				msg = "Game logic could not comprehend your message: ";
			}
		catch ( HLogicException& e )
			{
			msg = e.what();
			}
		if ( ! msg.is_empty() )
			remove_client_from_logic( a_roInfo, msg.raw() );
		}
	return;
	M_EPILOG
	}

void HServer::create_game( OClientInfo& a_roInfo, HString const& a_oArg )
	{
	M_PROLOG
	if ( a_roInfo.f_oLogin.is_empty() )
		kick_client( a_roInfo.f_oSocket, _( "Set your name first." ) );
	else
		{
		HString l_oType = get_token( a_oArg, ":", 0 );
		HString l_oConfiguration = get_token( a_oArg, ":", 1 );
		HString l_oName = get_token( l_oConfiguration, ",", 0 );
		HLogicFactory& factory = HLogicFactoryInstance::get_instance();
		logics_t::iterator it = f_oLogics.find( l_oName );
		if ( it != f_oLogics.end() )
			a_roInfo.f_oSocket->write_until_eos( "err:Game already exists.\n" );
		else if ( l_oName.find_other_than( LEGEAL_CHARACTER_SET[ CONSTR_CHAR_SET_GAME_NAME ] ) >= 0 )
			*a_roInfo.f_oSocket << "err:Game name may only take form of `[a-zA-Z0-9_-]+'." << endl;
		else if ( l_oName.get_length() > MAX_GAME_NAME_LENGTH )
			*a_roInfo.f_oSocket << "err:Game name cannot be longer than " << MAX_GAME_NAME_LENGTH << " characters." << endl;
		else if ( l_oName[0] == ' ' )
			*a_roInfo.f_oSocket << "err:Game name cannot start with whitespace." << endl;
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
					broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
							<< a_roInfo.f_oLogin << PROTOCOL::SEPP << l_oLogic->get_info() << _out );
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
	if ( a_roInfo.f_oLogin.is_empty() )
		kick_client( a_roInfo.f_oSocket, "Set your name first." );
	else
		{
		logics_t::iterator it = f_oLogics.find( a_oName );
		if ( it == f_oLogics.end() )
			a_roInfo.f_oSocket->write_until_eos( "err:Game does not exists.\n" );
		else if ( !! a_roInfo.f_oLogic )
			kick_client( a_roInfo.f_oSocket, _( "You were already in some game." ) );
		else if ( ! it->second->accept_client( &a_roInfo ) )
			{
			a_roInfo.f_oLogic = it->second;
			broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
					<< a_roInfo.f_oLogin << PROTOCOL::SEPP << a_roInfo.f_oLogic->get_info() << _out );
			}
		else
			a_roInfo.f_oSocket->write_until_eos( "err:Game is full.\n" );
		}
	return;
	M_EPILOG
	}

void HServer::handler_connection( int )
	{
	M_PROLOG
	HSocket::ptr_t l_oClient = f_oSocket.accept();
	M_ASSERT( !! l_oClient );
	if ( f_oSocket.get_client_count() >= f_iMaxConnections )
		l_oClient->close();
	else
		{
		_dispatcher.register_file_descriptor_handler( l_oClient->get_file_descriptor(), bound_call( &HServer::handler_message, this, _1 ) );
		f_oClients[ l_oClient->get_file_descriptor() ].f_oSocket = l_oClient;
		}
	out << l_oClient->get_host_name() << endl;
	return;
	M_EPILOG
	}

void HServer::handler_message( int a_iFileDescriptor )
	{
	M_PROLOG
	HString l_oMessage;
	HString l_oArgument;
	HString l_oCommand;
	clients_t::iterator clientIt;
	HSocket::ptr_t l_oClient = f_oSocket.get_client( a_iFileDescriptor );
	try
		{
		int long nRead( 0 );
		if ( ( clientIt = f_oClients.find( a_iFileDescriptor ) ) == f_oClients.end() )
			kick_client( l_oClient );
		else if ( ( nRead = l_oClient->read_until( l_oMessage ) ) > 0 )
			{
			if ( clientIt->second.f_oLogin.is_empty() )
				out << "`unnamed'";
			else
				out << clientIt->second.f_oLogin; 
			cout << "->" << l_oMessage << endl;
			l_oCommand = get_token( l_oMessage, ":", 0 );
			l_oArgument = l_oMessage.mid( l_oCommand.get_length() + 1 );
			int l_iMsgLength = static_cast<int>( l_oCommand.get_length() );
			if ( l_iMsgLength < 1 )
				kick_client( l_oClient, _( "Malformed data." ) );
			else
				{
				handlers_t::iterator it = f_oHandlers.find( l_oCommand );
				if ( it != f_oHandlers.end() )
					{
					HLogic::ptr_t l_oLogic = clientIt->second.f_oLogic;
					( this->*it->second )( clientIt->second, l_oArgument );
					if ( ( !! l_oLogic ) && ( ! l_oLogic->active_clients() ) )
						f_oLogics.erase( l_oLogic->get_name() );
					}
				else
					kick_client( l_oClient, _( "Unknown command." ) );
				}
			}
		else if ( ! nRead )
			kick_client( l_oClient, "" );
		/* else nRead < 0 => REPEAT */
		}
	catch ( HOpenSSLException& )
		{
		kick_client( l_oClient );
		}
	return;
	M_EPILOG
	}

void HServer::kick_client( yaal::hcore::HSocket::ptr_t& a_oClient, char const* const a_pcReason )
	{
	M_PROLOG
	M_ASSERT( !! a_oClient );
	int l_iFileDescriptor = a_oClient->get_file_descriptor();
	if ( a_pcReason && a_pcReason[0] )
		*a_oClient << PROTOCOL::KCK << PROTOCOL::SEP << a_pcReason << endl;
	f_oSocket.shutdown_client( l_iFileDescriptor );
	_dispatcher.unregister_file_descriptor_handler( l_iFileDescriptor );
	clients_t::iterator clientIt = f_oClients.find( l_iFileDescriptor );
	M_ASSERT( clientIt != f_oClients.end() );
	out << "client ";
	if ( clientIt->second.f_oLogin.is_empty() )
		cout << "`unnamed'";
	else
		cout << clientIt->second.f_oLogin; 
	if ( ! a_pcReason || a_pcReason[ 0 ] )
		{
		HString reason = " was kicked because of: ";
		reason += ( a_pcReason ? a_pcReason : "connection error" );
		if ( ! clientIt->second.f_oLogin.is_empty() )
			broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLORS::FG_BRIGHTRED ) << " " << clientIt->second.f_oLogin << reason << _out );
		cout << reason;
		}
	else
		cout << " disconnected from server.";
	cout << endl;
	remove_client_from_logic( clientIt->second );
	HString login;
	if ( ! clientIt->second.f_oLogin.is_empty() )
		login = clientIt->second.f_oLogin;
	f_oClients.erase( l_iFileDescriptor );
	if ( ! login.is_empty() )
		broadcast_to_interested( _out << PROTOCOL::PLAYER_QUIT << PROTOCOL::SEP << login << _out );
	return;
	M_EPILOG
	}

void HServer::send_logics_info( OClientInfo& a_roInfo )
	{
	M_PROLOG
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	for( HLogicFactory::creators_t::iterator it = factory.begin();
			it != factory.end(); ++ it )
		*a_roInfo.f_oSocket << PROTOCOL::LOGIC << PROTOCOL::SEP << it->second.f_oInfo << endl;
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
	_dispatcher.stop();
	return;
	}

void HServer::handler_quit( OClientInfo& a_roInfo, HString const& )
	{
	M_PROLOG
	HString login = a_roInfo.f_oLogin;
	kick_client( a_roInfo.f_oSocket, "" );
	if ( ! login.is_empty() )
		broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< mark( COLORS::FG_BROWN ) << " " << login << " has left the GameGround." << _out );
	return;
	M_EPILOG
	}

void HServer::handler_abandon( OClientInfo& a_roInfo, HString const& )
	{
	M_PROLOG
	remove_client_from_logic( a_roInfo );
	broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << a_roInfo.f_oLogin << _out );
	return;
	M_EPILOG
	}

void HServer::remove_client_from_logic( OClientInfo& a_roInfo, char const* const a_pcReason )
	{
	M_PROLOG
	if ( !! a_roInfo.f_oLogic )
		{
		HLogic::ptr_t l_oLogic = a_roInfo.f_oLogic;
		out << "separating logic info from client info for: " << a_roInfo.f_oLogin << endl;
		l_oLogic->kick_client( &a_roInfo, a_pcReason );
		a_roInfo.f_oLogic = HLogic::ptr_t();
		if ( ! l_oLogic->active_clients() )
			f_oLogics.erase( l_oLogic->get_name() );
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
	for( clients_t::iterator it = f_oClients.begin();
			it != f_oClients.end(); ++ it )
		{
		if ( ! it->second.f_oLogin.is_empty() )
			{
			*a_roInfo.f_oSocket << PROTOCOL::PLAYER << PROTOCOL::SEP << it->second.f_oLogin;
			if ( !! it->second.f_oLogic )
				*a_roInfo.f_oSocket << PROTOCOL::SEPP << it->second.f_oLogic->get_info();
			*a_roInfo.f_oSocket << endl;
			}
		}
	return;
	M_EPILOG
	}

void HServer::send_games_info( OClientInfo& a_roInfo )
	{
	M_PROLOG
	for( logics_t::iterator it = f_oLogics.begin();
			it != f_oLogics.end(); ++ it )
		*a_roInfo.f_oSocket << PROTOCOL::GAME << PROTOCOL::SEP << it->second->get_info() << endl;
	return;
	M_EPILOG
	}

void HServer::send_game_info( OClientInfo& /*a_roInfo*/, HString const& )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HServer::run( void )
	{
	M_PROLOG
	_dispatcher.run();
	return;
	M_EPILOG
	}

}

