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

HString const& mark( int color_ )
	{
	static HString buf;
	buf = "$";
	buf += color_;
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
char const* const HServer::PROTOCOL::ACCOUNT = "account";
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
char const* const HServer::PROTOCOL::VERSION = "version";
char const* const HServer::PROTOCOL::VERSION_ID = "0";

HServer::HServer( int connections_ )
	: _maxConnections( connections_ ),
	_socket( HSocket::socket_type_t( HSocket::TYPE::DEFAULT ) | HSocket::TYPE::NONBLOCKING | HSocket::TYPE::SSL_SERVER, connections_ ),
	_clients(), _logics(), _handlers(), _out(),
	_db( HDataBase::get_connector() ),
	_dispatcher( connections_, 3600 ), _idPool()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HServer::~HServer( void )
	{
	out << brightred << "<<<GameGround>>>" << lightgray << " server finished." << endl;
	}

int HServer::init_server( int port_ )
	{
	M_PROLOG
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.initialize_globals();
	_db->connect( setup._databasePath, setup._databaseLogin, setup._databasePassword );
	_socket.listen( "0.0.0.0", port_ );
	_dispatcher.register_file_descriptor_handler( _socket.get_file_descriptor(), bound_call( &HServer::handler_connection, this, _1 ) );
	_handlers[ PROTOCOL::SHUTDOWN ] = &HServer::handler_shutdown;
	_handlers[ PROTOCOL::QUIT ] = &HServer::handler_quit;
	_handlers[ PROTOCOL::MSG ] = &HServer::handler_chat;
	_handlers[ PROTOCOL::LOGIN ] = &HServer::handle_login;
	_handlers[ PROTOCOL::ACCOUNT ] = &HServer::handle_account;
	_handlers[ PROTOCOL::GET_LOGISTICS ] = &HServer::get_logics_info;
	_handlers[ PROTOCOL::GET_PLAYERS ] = &HServer::get_players_info;
	_handlers[ PROTOCOL::GET_GAMES ] = &HServer::get_games_info;
	_handlers[ PROTOCOL::GET_GAME_INFO ] = &HServer::get_game_info;
	_handlers[ PROTOCOL::CREATE ] = &HServer::create_game;
	_handlers[ PROTOCOL::JOIN ] = &HServer::join_game;
	_handlers[ PROTOCOL::ABANDON ] = &HServer::handler_abandon;
	_handlers[ PROTOCOL::CMD ] = &HServer::pass_command;
	out << brightblue << "<<<GameGround>>>" << lightgray << " server started." << endl;
	return ( 0 );
	M_EPILOG
	}

void HServer::broadcast( HString const& message_ )
	{
	M_PROLOG
	for ( clients_t::iterator it = _clients.begin(); it != _clients.end(); ++ it )
		*it->second._socket << message_ << endl;
	return;
	M_EPILOG
	}

void HServer::broadcast_to_interested( HString const& message_ )
	{
	M_PROLOG
	for ( clients_t::iterator it = _clients.begin(); it != _clients.end(); ++ it )
		if ( ! it->second._logic )
			*it->second._socket << message_ << endl;
	return;
	M_EPILOG
	}

void HServer::handler_chat( OClientInfo& client_, HString const& message_ )
	{
	M_PROLOG
	broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP << client_._login << ": " << message_ << _out );
	return;
	M_EPILOG
	}

void HServer::handle_login( OClientInfo& client_, HString const& loginInfo_ )
	{
	M_PROLOG
	clients_t::iterator it;
	int const MINIMUM_NAME_LENGTH( 4 );
	HString login( get_token( loginInfo_, ":", 0 ) );
	HString password( get_token( loginInfo_, ":", 1 ) );
	for ( it = _clients.begin(); it != _clients.end(); ++ it )
		if ( ( ! strcasecmp( it->second._login, login ) ) && ( it->second._socket != client_._socket ) )
			break;
	if ( login.find_other_than( LEGEAL_CHARACTER_SET[ CONSTR_CHAR_SET_LOGIN_NAME ] ) >= 0 )
		*client_._socket << "err:Name may only take form of `[a-zA-Z0-9]{4,}'." << endl;
	else if ( login.get_length() < MINIMUM_NAME_LENGTH )
		*client_._socket << "err:Your name is too short, it needs to be at least " << MINIMUM_NAME_LENGTH << " character long." << endl;
	else if ( it != _clients.end() )
		*client_._socket << "err:" << login << " already logged in." << endl;
	else
		{
		HRecordSet::ptr_t rs( _db->query( ( HFormat( "SELECT ( SELECT COUNT(*) FROM v_user_session WHERE login = LOWER('%s') AND password = LOWER('%s') )"
						" + ( SELECT COUNT(*) FROM v_user_session WHERE login = LOWER('%s') );" ) % login % password % login ).string() ) );
		M_ENSURE( !! rs );
		HRecordSet::iterator row = rs->begin();
		if ( row == rs->end() )
			{
			out << _db->get_error() << endl;
			M_ENSURE( ! "database query error" );
			}
		int result( lexical_cast<int>( row[0] ) );
		if ( ( result == 2 ) || ( result == 0 ) )
			{
			client_._login = login;
			if ( result ) /* user exists and supplied password was correct */
				update_last_activity( client_ );
			else if ( password != hash::sha1( "" ) )
				{
				rs = _db->query( ( HFormat( "INSERT INTO v_user_session ( login, password ) VALUES ( LOWER('%s'), LOWER('%s') );" ) % login % password ).string() );
				M_ENSURE( !! rs );
				}
			else
				{
				client_._anonymous = true;
				*client_._socket << PROTOCOL::MSG << PROTOCOL::SEP << mark( COLORS::FG_RED ) << " Your game stats will not be preserved nor your login protected." << endl;
				}
			broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << login << _out );
			broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLORS::FG_BLUE ) << " " << login << " entered the GameGround." << _out );
			}
		else
			{
			M_ENSURE( result == 1 );
			/* user exists but supplied password was incorrect */
			*client_._socket << "err:Login failed." << endl;
			}
		}
	return;
	M_EPILOG
	}

void HServer::handle_account( OClientInfo& client_, HString const& accountInfo_ )
	{
	M_PROLOG
	if ( client_._login.is_empty() )
		kick_client( client_._socket, _( "Set your name first (Just login with standard client, will ya?)." ) );
	else
		{
		HString action( get_token( accountInfo_, ":", 0 ) );
		if ( action == "register" )
			{
			}
		else if ( action == "update" )
			{
			}
		else
			kick_client( client_._socket, _( "Unknown account related action." ) );
		}
	return;
	M_EPILOG
	}

void HServer::pass_command( OClientInfo& client_, HString const& command_ )
	{
	M_PROLOG
	if ( ! client_._logic )
		client_._socket->write_until_eos( "err:Connect to some game first.\n" );
	else
		{
		HString msg;
		try
			{
			if ( client_._logic->process_command( &client_, command_ ) )
				msg = "Game logic could not comprehend your message: ";
			}
		catch ( HLogicException& e )
			{
			msg = e.what();
			}
		if ( ! msg.is_empty() )
			remove_client_from_logic( client_, msg.raw() );
		}
	return;
	M_EPILOG
	}

void HServer::create_game( OClientInfo& client_, HString const& arg_ )
	{
	M_PROLOG
	if ( client_._login.is_empty() )
		kick_client( client_._socket, _( "Set your name first (Just login with standard client, will ya?)." ) );
	else
		{
		HString type = get_token( arg_, ":", 0 );
		HString configuration = get_token( arg_, ":", 1 );
		HString name = get_token( configuration, ",", 0 );
		HLogicFactory& factory = HLogicFactoryInstance::get_instance();
		if ( ! factory.is_type_valid( type ) )
			kick_client( client_._socket, _( "No such game type." ) );
		else
			{
			HLogic::ptr_t logic;
			try
				{
				logic = factory.create_logic( type, create_id(), configuration );
				if ( ! logic->accept_client( &client_ ) )
					{
					id_t id;
					_logics[ logic->get_id() ] = logic;
					client_._logic = logic;
					out << name << "," << type << endl;
					broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
							<< client_._login << PROTOCOL::SEPP << logic->get_info() << _out );
					}
				else
					client_._socket->write_until_eos( "err:Specified configuration is inconsistent.\n" );
				}
			catch ( HLogicException& e )
				{
				kick_client( client_._socket, e.what() );
				}
			}
		}
	return;
	M_EPILOG
	}

void HServer::join_game( OClientInfo& client_, HString const& id_ )
	{
	M_PROLOG
	if ( client_._login.is_empty() )
		kick_client( client_._socket, _( "Set your name first (Just login with standard client, will ya?)." ) );
	else
		{
		logics_t::iterator it = _logics.find( id_ );
		if ( it == _logics.end() )
			client_._socket->write_until_eos( "err:Game does not exists.\n" );
		else if ( !! client_._logic )
			kick_client( client_._socket, _( "You were already in some game." ) );
		else if ( ! it->second->accept_client( &client_ ) )
			{
			client_._logic = it->second;
			broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
					<< client_._login << PROTOCOL::SEPP << client_._logic->get_info() << _out );
			}
		else
			client_._socket->write_until_eos( "err:Game is full.\n" );
		}
	return;
	M_EPILOG
	}

void HServer::handler_connection( int )
	{
	M_PROLOG
	HSocket::ptr_t client = _socket.accept();
	M_ASSERT( !! client );
	if ( _socket.get_client_count() >= _maxConnections )
		client->close();
	else
		{
		_dispatcher.register_file_descriptor_handler( client->get_file_descriptor(), bound_call( &HServer::handler_message, this, _1 ) );
		_clients[ client->get_file_descriptor() ]._socket = client;
		}
	out << client->get_host_name() << endl;
	return;
	M_EPILOG
	}

void HServer::handler_message( int fileDescriptor_ )
	{
	M_PROLOG
	HString message;
	HString argument;
	HString command;
	clients_t::iterator clientIt;
	HSocket::ptr_t client = _socket.get_client( fileDescriptor_ );
	try
		{
		int long nRead( 0 );
		if ( ( clientIt = _clients.find( fileDescriptor_ ) ) == _clients.end() )
			kick_client( client );
		else if ( ( nRead = client->read_until( message ) ) > 0 )
			{
			if ( clientIt->second._login.is_empty() )
				out << "`unnamed'";
			else
				out << clientIt->second._login; 
			cout << "->" << message << endl;
			command = get_token( message, ":", 0 );
			argument = message.mid( command.get_length() + 1 );
			int msgLength = static_cast<int>( command.get_length() );
			if ( msgLength < 1 )
				kick_client( client, _( "Malformed data." ) );
			else
				{
				handlers_t::iterator it = _handlers.find( command );
				if ( it != _handlers.end() )
					{
					HLogic::ptr_t logic = clientIt->second._logic;
					( this->*it->second )( clientIt->second, argument );
					if ( ( !! logic ) && ( ! logic->active_clients() ) )
						_logics.erase( logic->get_name() );
					}
				else
					kick_client( client, _( "Unknown command." ) );
				}
			}
		else if ( ! nRead )
			kick_client( client, "" );
		/* else nRead < 0 => REPEAT */
		}
	catch ( HOpenSSLException& )
		{
		kick_client( client );
		}
	return;
	M_EPILOG
	}

void HServer::kick_client( yaal::hcore::HSocket::ptr_t& client_, char const* const reason_ )
	{
	M_PROLOG
	M_ASSERT( !! client_ );
	int fileDescriptor = client_->get_file_descriptor();
	if ( reason_ && reason_[0] )
		*client_ << PROTOCOL::KCK << PROTOCOL::SEP << reason_ << endl;
	_socket.shutdown_client( fileDescriptor );
	_dispatcher.unregister_file_descriptor_handler( fileDescriptor );
	clients_t::iterator clientIt = _clients.find( fileDescriptor );
	M_ASSERT( clientIt != _clients.end() );
	out << "client ";
	if ( clientIt->second._login.is_empty() )
		cout << "`unnamed'";
	else
		cout << clientIt->second._login; 
	if ( ! reason_ || reason_[ 0 ] )
		{
		HString reason = " was kicked because of: ";
		reason += ( reason_ ? reason_ : "connection error" );
		if ( ! clientIt->second._login.is_empty() )
			broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLORS::FG_BRIGHTRED ) << " " << clientIt->second._login << reason << _out );
		cout << reason;
		}
	else
		cout << " disconnected from server.";
	cout << endl;
	remove_client_from_logic( clientIt->second );
	HString login;
	if ( ! clientIt->second._login.is_empty() )
		login = clientIt->second._login;
	_clients.erase( fileDescriptor );
	if ( ! login.is_empty() )
		broadcast_to_interested( _out << PROTOCOL::PLAYER_QUIT << PROTOCOL::SEP << login << _out );
	return;
	M_EPILOG
	}

void HServer::send_logics_info( OClientInfo& client_ )
	{
	M_PROLOG
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	for( HLogicFactory::creators_t::iterator it = factory.begin();
			it != factory.end(); ++ it )
		*client_._socket << PROTOCOL::LOGIC << PROTOCOL::SEP << it->second._info << endl;
	return;
	M_EPILOG
	}

void HServer::get_logics_info( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	send_logics_info( client_ );
	return;
	M_EPILOG
	}

void HServer::handler_shutdown( OClientInfo&, HString const& )
	{
	_dispatcher.stop();
	return;
	}

void HServer::handler_quit( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	if ( ! client_._anonymous )
		update_last_activity( client_ );
	HString login( client_._login );
	kick_client( client_._socket, "" );
	if ( ! login.is_empty() )
		broadcast_to_interested( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< mark( COLORS::FG_BROWN ) << " " << login << " has left the GameGround." << _out );
	return;
	M_EPILOG
	}

void HServer::handler_abandon( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	remove_client_from_logic( client_ );
	broadcast_to_interested( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << client_._login << _out );
	return;
	M_EPILOG
	}

void HServer::remove_client_from_logic( OClientInfo& client_, char const* const reason_ )
	{
	M_PROLOG
	if ( !! client_._logic )
		{
		HLogic::ptr_t logic = client_._logic;
		out << "separating logic info from client info for: " << client_._login << endl;
		logic->kick_client( &client_, reason_ );
		client_._logic = HLogic::ptr_t();
		if ( ! logic->active_clients() )
			_logics.erase( logic->get_name() );
		}
	return;
	M_EPILOG
	}

void HServer::get_players_info( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	send_players_info( client_ );
	return;
	M_EPILOG
	}

void HServer::get_games_info( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	send_games_info( client_ );
	return;
	M_EPILOG
	}

void HServer::get_game_info( OClientInfo& client_, HString const& name_ )
	{
	M_PROLOG
	send_game_info( client_, name_ );
	return;
	M_EPILOG
	}

void HServer::send_players_info( OClientInfo& client_ )
	{
	M_PROLOG
	for( clients_t::iterator it = _clients.begin();
			it != _clients.end(); ++ it )
		{
		if ( ! it->second._login.is_empty() )
			{
			*client_._socket << PROTOCOL::PLAYER << PROTOCOL::SEP << it->second._login;
			if ( !! it->second._logic )
				*client_._socket << PROTOCOL::SEPP << it->second._logic->get_info();
			*client_._socket << endl;
			}
		}
	return;
	M_EPILOG
	}

void HServer::send_games_info( OClientInfo& client_ )
	{
	M_PROLOG
	for( logics_t::iterator it = _logics.begin();
			it != _logics.end(); ++ it )
		*client_._socket << PROTOCOL::GAME << PROTOCOL::SEP << it->second->get_info() << endl;
	return;
	M_EPILOG
	}

void HServer::update_last_activity( OClientInfo const& info_ )
	{
	M_PROLOG
	HRecordSet::ptr_t rs( _db->query(
		( HFormat(
				"UPDATE v_user_session SET last_activity = datetime('now', 'localtime') WHERE login = LOWER('%s');"
		) % info_._login ).string() ) );
	M_ENSURE( !! rs );
	return;
	M_EPILOG
	}

void HServer::send_game_info( OClientInfo& /*client_*/, HString const& )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HLogic::id_t HServer::create_id( void )
	{
	M_PROLOG
	HLogic::id_t id = _idPool;
	++ _idPool;
	return ( id );
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

