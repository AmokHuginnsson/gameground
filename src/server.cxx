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
char const* const HServer::PROTOCOL::PARTY = "party";
char const* const HServer::PROTOCOL::PARTY_INFO = "party_info";
char const* const HServer::PROTOCOL::PARTY_CLOSE = "party_close";
char const* const HServer::PROTOCOL::GET_PARTYS = "get_partys";
char const* const HServer::PROTOCOL::GET_LOGICS = "get_logics";
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
	_dispatcher( connections_, 3600 * 1000 ), _idPool( 1 ),
	_dropouts()
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
	_dispatcher.register_file_descriptor_handler( _socket.get_file_descriptor(), call( &HServer::handler_connection, this, _1 ) );
	_handlers[ PROTOCOL::SHUTDOWN ] = &HServer::handler_shutdown;
	_handlers[ PROTOCOL::QUIT ] = &HServer::handler_quit;
	_handlers[ PROTOCOL::MSG ] = &HServer::handler_chat;
	_handlers[ PROTOCOL::LOGIN ] = &HServer::handle_login;
	_handlers[ PROTOCOL::ACCOUNT ] = &HServer::handle_account;
	_handlers[ PROTOCOL::GET_LOGICS ] = &HServer::handle_get_logics;
	_handlers[ PROTOCOL::GET_PLAYERS ] = &HServer::handle_get_players;
	_handlers[ PROTOCOL::GET_PARTYS ] = &HServer::handle_get_partys;
	_handlers[ PROTOCOL::CREATE ] = &HServer::create_game;
	_handlers[ PROTOCOL::JOIN ] = &HServer::join_game;
	_handlers[ PROTOCOL::ABANDON ] = &HServer::handler_abandon;
	_handlers[ PROTOCOL::CMD ] = &HServer::pass_command;
	out << brightblue << "<<<GameGround>>>" << lightgray << " server started." << endl;
	return ( 0 );
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
		_dispatcher.register_file_descriptor_handler( client->get_file_descriptor(), call( &HServer::handler_message, this, _1 ) );
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
	bool kick( false );
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
				handlers_t::iterator handler = _handlers.find( command );
				if ( handler != _handlers.end() )
					{
					( this->*handler->second )( clientIt->second, argument );
					flush_logics();
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
		kick = true;
		}
	if ( kick && !! client
			&& ( _dropouts.is_empty()
				|| ! ( find( _dropouts.begin(), _dropouts.end(), &clientIt->second ) != _dropouts.end() ) ) )
		kick_client( client );
	if ( ! _dropouts.is_empty() )
		flush_droupouts();
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
	remove_client_from_all_logics( clientIt->second );
	out << "client ";
	HString login;
	if ( ! clientIt->second._login.is_empty() )
		{
		login = clientIt->second._login;
		cout << login;
		}
	else
		cout << "`unnamed'";
	_clients.erase( fileDescriptor );
	if ( ! reason_ || reason_[ 0 ] )
		{
		HString reason = " was kicked because of: ";
		reason += ( reason_ ? reason_ : "connection error" );
		if ( ! clientIt->second._login.is_empty() )
			broadcast_all_parties( &clientIt->second, _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLORS::FG_BRIGHTRED ) << " " << clientIt->second._login << reason << endl << _out );
		cout << reason;
		}
	else
		cout << " disconnected from server.";
	cout << endl;
	if ( ! login.is_empty() )
		broadcast( _out << PROTOCOL::PLAYER_QUIT << PROTOCOL::SEP << login << endl << _out );
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
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< mark( COLORS::FG_BROWN ) << " " << login << " has left the GameGround." << endl << _out );
	return;
	M_EPILOG
	}

void HServer::broadcast( HString const& message_ )
	{
	M_PROLOG
	for ( clients_t::iterator it( _clients.begin() ), end( _clients.end() ); it != end; ++ it )
		{
		try
			{
			if ( it->second._valid )
				it->second._socket->write_until_eos( message_ );
			}
		catch ( HOpenSSLException const& )
			{
			drop_client( &it->second );
			}
		}
	return;
	M_EPILOG
	}

void HServer::broadcast_party( HString const& id_, HString const& message_ )
	{
	M_PROLOG
	logics_t::iterator logic( _logics.find( id_ ) );
	if ( logic != _logics.end() )
		logic->second->broadcast( message_ );
	return;
	M_EPILOG
	}

void HServer::broadcast_all_parties( OClientInfo* info_, HString const& message_ )
	{
	M_PROLOG
	broadcast( message_ );
	for ( OClientInfo::logics_t::iterator it( info_->_logics.begin() ), end( info_->_logics.end() ); it != end; ++ it )
		broadcast_party( *it, message_ );
	return;
	M_EPILOG
	}

void HServer::handler_chat( OClientInfo& client_, HString const& message_ )
	{
	M_PROLOG
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP << client_._login << ": " << message_ << endl << _out );
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
		int result( lexical_cast<int>( *row[0] ) );
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
			broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << login << endl << _out );
			broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLORS::FG_BLUE ) << " " << login << " entered the GameGround." << endl << _out );
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
	if ( client_._logics.is_empty() )
		client_._socket->write_until_eos( "err:Connect to some game first.\n" );
	else
		{
		HString id( get_token( command_, ":", 0 ) );
		logics_t::iterator logic( _logics.find( id ) );
		if ( logic == _logics.end() )
			client_._socket->write_until_eos( "err:No such party exists.\n" );
		else
			{
			if ( client_._logics.count( id ) == 0 )
				client_._socket->write_until_eos( "err:You are not part of this party.\n" );
			else
				{
				HString msg;
				try
					{
					if ( logic->second->process_command( &client_, command_.mid( id.get_length() + 1 ) ) )
						msg = "Game logic could not comprehend your message: ";
					}
				catch ( HLogicException& e )
					{
					msg = e.what();
					}
				if ( ! msg.is_empty() )
					remove_client_from_logic( client_, logic->second, msg.raw() );
				}
			}
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
				logic = factory.create_logic( type, this, create_id(), configuration );
				if ( ! logic->accept_client( &client_ ) )
					{
					HLogic::id_t const& id( logic->id() );
					_logics[ id ] = logic;
					out << name << "," << type << endl;
					broadcast( _out << PROTOCOL::PARTY_INFO << PROTOCOL::SEP << id << PROTOCOL::SEPP << logic->get_info() << endl << _out );
					send_player_info( client_ );
					logic->post_accept_client( &client_ );
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
			client_._socket->write_until_eos( "err:Party does not exists.\n" );
		else if ( client_._logics.count( id_ ) != 0 )
			kick_client( client_._socket, _( "You were already in this party." ) );
		else if ( ! it->second->accept_client( &client_ ) )
			{
			send_player_info( client_ );
			it->second->post_accept_client( &client_ );
			}
		else
			client_._socket->write_until_eos( "err:Game is full.\n" );
		}
	return;
	M_EPILOG
	}

void HServer::send_logics_info( OClientInfo& client_ )
	{
	M_PROLOG
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	for ( HLogicFactory::creators_t::iterator it = factory.begin();
			it != factory.end(); ++ it )
		SENDF( *client_._socket ) << PROTOCOL::LOGIC << PROTOCOL::SEP << it->second.get_info() << endl;
	return;
	M_EPILOG
	}

void HServer::handle_get_logics( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	send_logics_info( client_ );
	return;
	M_EPILOG
	}

void HServer::handler_abandon( OClientInfo& client_, HString const& id_ )
	{
	M_PROLOG
	if ( client_._logics.count( id_ ) == 0 )
		kick_client( client_._socket, _( "You were not part of this party." ) );
	else
		{
		logics_t::iterator logic( _logics.find( id_ ) );
		if ( logic != _logics.end() )
			{
			out << "client " << client_._login << " abandoned party `" << id_ << "'" << endl;
			remove_client_from_logic( client_, logic->second );
			}
		}
	return;
	M_EPILOG
	}

void HServer::remove_client_from_all_logics( OClientInfo& client_ )
	{
	M_PROLOG
	out << "removing client from all logics: " << client_._login << endl;
	for ( OClientInfo::logics_t::iterator it( client_._logics.begin() ), end( client_._logics.end() ); it != end; )
		{
		HLogic::id_t id( *it );
		++ it;
		logics_t::iterator logic( _logics.find( id ) );
		M_ASSERT( logic != _logics.end() );
		logic->second->kick_client( &client_ );
		}
	client_._logics.clear();
	send_player_info( client_ );
	flush_logics();
	return;
	M_EPILOG
	}

void HServer::flush_logics( void )
	{
	M_PROLOG
	for ( logics_t::iterator it( _logics.begin() ), end( _logics.end() ); it != end; )
		{
		if ( ! it->second->active_clients() )
			{
			logics_t::iterator del( it );
			++ it;
			broadcast( _out << PROTOCOL::PARTY_CLOSE << PROTOCOL::SEP << del->first << endl << _out );
			_logics.erase( del );
			}
		else
			++ it;
		}
	/* does not work becuse remove_if does not work on maps */
	// _logics.erase( remove_if( _logics.begin(), _logics.end(), not1( compose1( call( &HLogic::active_clients, _1 ), select2nd<logics_t::value_type>() ) ) ), _logics.end() );
	return;
	M_EPILOG
	}

void HServer::remove_client_from_logic( OClientInfo& client_, HLogic::ptr_t logic_, char const* const reason_ )
	{
	M_PROLOG
	if ( !! logic_ )
		{
		out << "separating logic info from client info for: " << client_._login << " and party: " << logic_->get_info() << endl;
		logic_->kick_client( &client_, reason_ );
		HString const& id( logic_->id() );
		client_._logics.erase( id );
		send_player_info( client_ );
		if ( ! logic_->active_clients() )
			{
			broadcast( _out << PROTOCOL::PARTY_CLOSE << PROTOCOL::SEP << id << endl << _out );
			_logics.erase( id );
			}
		}
	return;
	M_EPILOG
	}

void HServer::handle_get_players( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	send_players_info( client_ );
	return;
	M_EPILOG
	}

void HServer::handle_get_partys( OClientInfo& client_, HString const& )
	{
	M_PROLOG
	send_games_info( client_ );
	return;
	M_EPILOG
	}

void HServer::send_player_info( OClientInfo& client_ )
	{
	M_PROLOG
	_out << PROTOCOL::PLAYER << PROTOCOL::SEP << client_._login;
	for ( OClientInfo::logics_t::iterator it( client_._logics.begin() ), end( client_._logics.end() ); it != end; ++ it )
		{
		logics_t::iterator logic( _logics.find( *it ) );
		if ( logic != _logics.end() )
		 _out << PROTOCOL::SEPP << logic->first;
		}
	broadcast( _out << endl << _out );
	return;
	M_EPILOG
	}

void HServer::send_players_info( OClientInfo& client_ )
	{
	M_PROLOG
	for( clients_t::iterator client = _clients.begin();
			client != _clients.end(); ++ client )
		{
		if ( client->second._valid )
			{
			try
				{
				if ( ! client->second._login.is_empty() )
					{
					_out << PROTOCOL::PLAYER << PROTOCOL::SEP << client->second._login;
					for ( OClientInfo::logics_t::iterator it( client->second._logics.begin() ), end( client->second._logics.end() ); it != end; ++ it )
						{
						logics_t::iterator logic( _logics.find( *it ) );
						if ( logic != _logics.end() )
							_out << PROTOCOL::SEPP << logic->first;
						}
					_out << endl;
					SENDF( *client_._socket ) << ( _out << _out );
					}
				}
			catch ( HOpenSSLException const& )
				{
				drop_client( &client_ );
				}
			}
		}
	return;
	M_EPILOG
	}

void HServer::send_games_info( OClientInfo& client_ )
	{
	M_PROLOG
	for( logics_t::iterator it( _logics.begin() ), end( _logics.end() ); it != end; ++ it )
		SENDF( *client_._socket ) << PROTOCOL::PARTY_INFO << PROTOCOL::SEP << it->first << PROTOCOL::SEPP << it->second->get_info() << endl;
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

HLogic::id_t HServer::create_id( void )
	{
	M_PROLOG
	HLogic::id_t id = _idPool.to_string();
	++ _idPool;
	return ( id );
	M_EPILOG
	}

void HServer::drop_client( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	_dropouts.push_back( clientInfo_ );
	clientInfo_->_valid = false;
	return;
	M_EPILOG
	}

void HServer::flush_droupouts( void )
	{
	M_PROLOG
	while ( ! _dropouts.is_empty() )
		{
		OClientInfo* dropout( _dropouts.back() );
		_dropouts.pop_back();
		M_ASSERT( !! dropout->_socket );
		kick_client( dropout->_socket, NULL );
		}
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

