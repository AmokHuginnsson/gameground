/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hformat.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/hash.hxx>
#include <yaal/tools/util.hxx>
#include <yaal/tools/streamtools.hxx>
#include <yaal/tools/ansi.hxx>
#include <yaal/hconsole/console.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "server.hxx"

#include "setup.hxx"
#include "logicfactory.hxx"
#include "security.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace yaal::dbwrapper;
using namespace yaal::ansi;
using namespace yaal::hconsole;

namespace gameground {

namespace {

HString const& mark( int color_ ) {
	static HString buf;
	buf = "$";
	buf += color_;
	buf += ";###$7;";
	return ( buf );
}

}

static int const MAX_GAME_NAME_LENGTH = 20;
#define LEGEAL_CHARACTER_SET_BASE "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-"
static int const CONSTR_CHAR_SET_LOGIN_NAME = 0;
static int const CONSTR_CHAR_SET_GAME_NAME = 1;
char const* const LEGEAL_CHARACTER_SET[] = { LEGEAL_CHARACTER_SET_BASE, " " LEGEAL_CHARACTER_SET_BASE };
char const* const HServer::PROTOCOL::ABANDON = "abandon";
char const* const HServer::PROTOCOL::ACCOUNT = "account";
char const* const HServer::PROTOCOL::CLIENT_SETUP = "client_setup";
char const* const HServer::PROTOCOL::CMD = "cmd";
char const* const HServer::PROTOCOL::CREATE = "create";
char const* const HServer::PROTOCOL::ERR = "err";
char const* const HServer::PROTOCOL::PARTY = "party";
char const* const HServer::PROTOCOL::PARTY_INFO = "party_info";
char const* const HServer::PROTOCOL::PARTY_CLOSE = "party_close";
char const* const HServer::PROTOCOL::GET_PARTYS = "get_partys";
char const* const HServer::PROTOCOL::GET_LOGICS = "get_logics";
char const* const HServer::PROTOCOL::GET_PLAYERS = "get_players";
char const* const HServer::PROTOCOL::GET_ACCOUNT = "get_account";
char const* const HServer::PROTOCOL::JOIN = "join";
char const* const HServer::PROTOCOL::KCK = "kck";
char const* const HServer::PROTOCOL::LOGIC = "logic";
char const* const HServer::PROTOCOL::MSG = "msg";
char const* const HServer::PROTOCOL::SAY = "say";
char const* const HServer::PROTOCOL::LOGIN = "login";
char const* const HServer::PROTOCOL::PLAYER = "player";
char const* const HServer::PROTOCOL::PLAYER_QUIT = "player_quit";
char const* const HServer::PROTOCOL::QUIT = "quit";
char const* const HServer::PROTOCOL::SEP = ":";
char const* const HServer::PROTOCOL::SEPP = ",";
char const* const HServer::PROTOCOL::SHUTDOWN = "shutdown";
char const* const HServer::PROTOCOL::VERSION = "version";
char const* const HServer::PROTOCOL::VERSION_ID = "4";
char const* const HServer::PROTOCOL::WARN = "warn";

static const HString NULL_PASS = tools::hash::sha1( "" );

char const _msgYourClientIsTainted_[] = "Your client is tainted, go away!";

HServer::HServer( int connections_ )
	: _maxConnections( connections_ ),
	_socket(
		make_pointer<HSocket>(
			HSocket::TYPE::DEFAULT | HSocket::TYPE::NONBLOCKING | HSocket::TYPE::SSL | HSocket::TYPE::SERVER,
			connections_
		)
	),
	_clients(), _logins(), _logics(), _handlers(), _out(),
	_db( HDataBase::get_connector() ), _mutex(),
	_dispatcher( connections_, 3600 * 1000 ), _idPool( 1 ),
	_dropouts() {
	M_PROLOG
	return;
	M_EPILOG
}

HServer::~HServer( void ) {
	M_ASSERT( _logics.is_empty() );
	M_ASSERT( _logins.is_empty() );
	M_ASSERT( _clients.is_empty() );
	OUT << brightred << "<<<GameGround>>>" << lightgray << " server finished." << endl;
}

int HServer::init_server( int port_ ) {
	M_PROLOG
	HLogicFactory& factory( HLogicFactoryInstance::get_instance() );
	factory.initialize_globals();
	_db->connect( setup._databasePath, setup._databaseLogin, setup._databasePassword );
	_socket->listen( "0.0.0.0", port_ );
	_dispatcher.register_file_descriptor_handler( _socket, call( &HServer::handler_connection, this, _1 ) );
	_handlers[ PROTOCOL::SHUTDOWN ] = &HServer::handler_shutdown;
	_handlers[ PROTOCOL::QUIT ] = &HServer::handler_quit;
	_handlers[ PROTOCOL::MSG ] = &HServer::handler_chat;
	_handlers[ PROTOCOL::LOGIN ] = &HServer::handle_login;
	_handlers[ PROTOCOL::ACCOUNT ] = &HServer::handle_account;
	_handlers[ PROTOCOL::GET_LOGICS ] = &HServer::handle_get_logics;
	_handlers[ PROTOCOL::GET_PLAYERS ] = &HServer::handle_get_players;
	_handlers[ PROTOCOL::GET_PARTYS ] = &HServer::handle_get_partys;
	_handlers[ PROTOCOL::GET_ACCOUNT ] = &HServer::handle_get_account;
	_handlers[ PROTOCOL::CREATE ] = &HServer::create_party;
	_handlers[ PROTOCOL::JOIN ] = &HServer::join_party;
	_handlers[ PROTOCOL::ABANDON ] = &HServer::handler_abandon;
	_handlers[ PROTOCOL::CMD ] = &HServer::pass_command;
	OUT << brightblue << "<<<GameGround>>>" << lightgray << " server started." << endl;
	return ( 0 );
	M_EPILOG
}

void HServer::handler_connection( yaal::tools::HIODispatcher::stream_t& ) {
	M_PROLOG
	HSocket::ptr_t client = _socket->accept();
	M_ASSERT( !! client );
	if ( static_cast<int>( _clients.get_size() ) >= _maxConnections ) {
		client->close();
	} else {
		_dispatcher.register_file_descriptor_handler( client, call( &HServer::handler_message, this, _1 ) );
		_clients[ client.raw() ]._socket = client;
	}
	OUT << client->get_host_name() << endl;
	return;
	M_EPILOG
}

void HServer::handler_message( yaal::tools::HIODispatcher::stream_t& stream_ ) {
	M_PROLOG
	HString message;
	HString argument;
	HString command;
	clients_t::iterator clientIt;
	if ( ( clientIt = _clients.find( stream_.raw() ) ) == _clients.end() ) {
		kick_client( stream_ );
	} else {
		int long nRead( -1 );
		try {
			nRead = stream_->read_until( message );
		} catch ( HOpenSSLException& ) {
			drop_client( &clientIt->second );
		}
		if ( nRead > 0 ) {
			if ( clientIt->second._login.is_empty() )
				OUT << "`unnamed'";
			else
				OUT << clientIt->second._login;
			clog << "->" << message << endl;
			command = get_token( message, ":", 0 );
			argument = message.mid( command.get_length() + 1 );
			int msgLength = static_cast<int>( command.get_length() );
			if ( msgLength < 1 ) {
				kick_client( stream_, "Malformed data." );
			} else {
				handlers_t::iterator handler = _handlers.find( command );
				if ( handler != _handlers.end() ) {
					try {
						( this->*handler->second )( clientIt->second, argument );
					} catch ( HOpenSSLException const& ) {
						drop_client( &clientIt->second );
					}
					flush_logics();
				} else {
					kick_client( stream_, "Unknown command." );
				}
			}
		} else if ( ! nRead ) {
			kick_client( stream_, "" );
		}
		/* else nRead < 0 => REPEAT */
	}
	if ( ! _dropouts.is_empty() ) {
		flush_droupouts();
	}
	return;
	M_EPILOG
}

void HServer::kick_client( yaal::tools::HIODispatcher::stream_t& client_, char const* const reason_ ) {
	M_PROLOG
	M_ASSERT( !! client_ );
	clients_t::iterator clientIt( _clients.find( client_.raw() ) );
	M_ASSERT( clientIt != _clients.end() );
	if ( clientIt->second._valid && reason_ && reason_[0] ) {
		try {
			*client_ << PROTOCOL::KCK << PROTOCOL::SEP << reason_ << endl;
		} catch ( HOpenSSLException const& ) {
			/* Kicked client is no longer valid but we cannot do anything about it. */
		}
	}
	_dispatcher.unregister_file_descriptor_handler( client_ );
	clientIt->second._valid = false;
	remove_client_from_all_logics( clientIt->second );
	OUT << "client ";
	HString login;
	if ( ! clientIt->second._login.is_empty() ) {
		login = clientIt->second._login;
		clog << login;
	} else
		clog << "`unnamed'";
	if ( ! reason_ || reason_[ 0 ] ) {
		HString reason = " was kicked because of: ";
		reason += ( reason_ ? reason_ : "connection error" );
		if ( ! clientIt->second._login.is_empty() )
			broadcast_all_parties( &clientIt->second, _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLOR::FG_BRIGHTRED ) << " " << clientIt->second._login << reason << endl << _out );
		clog << reason;
	} else {
		char const msgDisconnected[] = " disconnected from server.";
		if ( ! clientIt->second._login.is_empty() )
			broadcast_all_parties( &clientIt->second, _out << PROTOCOL::MSG << PROTOCOL::SEP
					<< mark( COLOR::FG_YELLOW ) << " " << clientIt->second._login << msgDisconnected << endl << _out );
		clog << msgDisconnected;
	}
	if ( ! login.is_empty() ) {
		_logins.erase( login );
	}
	_clients.erase( clientIt );
	clog << endl;
	if ( ! login.is_empty() )
		broadcast( _out << PROTOCOL::PLAYER_QUIT << PROTOCOL::SEP << login << endl << _out );
	return;
	M_EPILOG
}

void HServer::handler_shutdown( OClientInfo&, HString const& ) {
	_dispatcher.stop();
	return;
}

void HServer::handler_quit( OClientInfo& client_, HString const& ) {
	M_PROLOG
	if ( ! client_._anonymous )
		update_last_activity( client_ );
	HString login( client_._login );
	kick_client( client_._socket, "" );
	if ( ! login.is_empty() )
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< mark( COLOR::FG_BROWN ) << " " << login << " has left the GameGround." << endl << _out );
	return;
	M_EPILOG
}

void HServer::broadcast( HString const& message_ ) {
	M_PROLOG
	for ( clients_t::iterator it( _clients.begin() ), end( _clients.end() ); it != end; ++ it ) {
		try {
			if ( it->second._valid ) {
				*it->second._socket << message_;
			}
		} catch ( HOpenSSLException const& ) {
			drop_client( &it->second );
		}
	}
	return;
	M_EPILOG
}

void HServer::broadcast_party( HString const& id_, HString const& message_ ) {
	M_PROLOG
	logics_t::iterator logic( _logics.find( id_ ) );
	if ( logic != _logics.end() )
		logic->second->broadcast( message_ );
	return;
	M_EPILOG
}

void HServer::broadcast_all_parties( OClientInfo* info_, HString const& message_ ) {
	M_PROLOG
	broadcast( message_ );
	for ( OClientInfo::logics_t::iterator it( info_->_logics.begin() ), end( info_->_logics.end() ); it != end; ++ it )
		broadcast_party( *it, message_ );
	return;
	M_EPILOG
}

void HServer::broadcast_private( HLogic& party_, HString const& message_ ) {
	M_PROLOG
	for ( HLogic::clients_t::HIterator it( party_._clients.begin() ), end( party_._clients.end() ); it != end; ++ it ) {
		try {
			if ( (*it)->_valid ) {
				*(*it)->_socket << message_;
			}
		} catch ( HOpenSSLException const& ) {
			drop_client( *it );
		}
	}
	return;
	M_EPILOG
}

void HServer::handler_chat( OClientInfo& client_, HString const& message_ ) {
	M_PROLOG
	broadcast( _out << PROTOCOL::SAY << PROTOCOL::SEP << client_._login << ": " << message_ << endl << _out );
	return;
	M_EPILOG
}

void HServer::handle_login( OClientInfo& client_, HString const& loginInfo_ ) {
	M_PROLOG
	int const MINIMUM_NAME_LENGTH( 4 );
	HString version( get_token( loginInfo_, ":", 0 ) );
	HString login( get_token( loginInfo_, ":", 1 ) );
	HString password( get_token( loginInfo_, ":", 2 ) );
	do {
		if ( version != PROTOCOL::VERSION_ID ) {
			*client_._socket << "err:Your client version is not supported." << endl;
			kick_client( client_._socket );
		} else if ( login.find_other_than( LEGEAL_CHARACTER_SET[ CONSTR_CHAR_SET_LOGIN_NAME ] ) >= 0 )
			*client_._socket << "err:Name may only take form of `[a-zA-Z0-9]{4,}'." << endl;
		else if ( login.get_length() < MINIMUM_NAME_LENGTH )
			*client_._socket << "err:Your name is too short, it needs to be at least " << MINIMUM_NAME_LENGTH << " character long." << endl;
		else if ( ! is_sha1( password ) )
			kick_client( client_._socket, _msgYourClientIsTainted_ );
		else if ( _logins.count( login ) > 0 )
			*client_._socket << "err:" << login << " already logged in." << endl;
		else {
			HQuery::ptr_t query( _db->prepare_query( "SELECT ( SELECT COUNT(*) FROM v_user_session WHERE login = LOWER(?) AND password = LOWER(?) )"
						" + ( SELECT COUNT(*) FROM v_user_session WHERE login = LOWER(?) ), ( SELECT setup FROM v_user_session WHERE login = LOWER(?) );" ) );
			query->bind( 1, login );
			query->bind( 2, password );
			query->bind( 3, login );
			query->bind( 4, login );
			HRecordSet::ptr_t rs( query->execute() );
			M_ENSURE( !! rs );
			HRecordSet::iterator row = rs->begin();
			if ( row == rs->end() ) {
				OUT << _db->get_error() << endl;
				M_ENSURE( ! "database query execution error" );
			}
			int result( lexical_cast<int>( *row[0] ) );
			if ( ( result == 2 ) || ( result == 0 ) ) {
				client_._login = login;
				_logins.insert( make_pair( login, &client_ ) );
				if ( result ) { /* user exists and supplied password was correct */
					update_last_activity( client_ );
					HRecordSet::value_t clientSetup( row[1] );
					if ( clientSetup && ! clientSetup->is_empty() ) {
						*client_._socket << PROTOCOL::CLIENT_SETUP << PROTOCOL::SEP << *clientSetup << endl;
					}
				} else if ( password != NULL_PASS ) {
					HQuery::ptr_t queryLogin( _db->prepare_query( "INSERT INTO v_user_session ( login, password ) VALUES ( LOWER(?), LOWER(?) );" ) );
					queryLogin->bind( 1, login );
					queryLogin->bind( 2, password );
					rs = queryLogin->execute();
					M_ENSURE( !! rs );
				} else {
					client_._anonymous = true;
					*client_._socket << PROTOCOL::MSG << PROTOCOL::SEP << mark( COLOR::FG_RED ) << " Your game stats will not be preserved nor your login protected." << endl;
				}
				broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << login << endl << _out );
				broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
						<< mark( COLOR::FG_BLUE ) << " " << login << " entered the GameGround." << endl << _out );
			} else {
				M_ENSURE( result == 1 );
				/* user exists but supplied password was incorrect */
				*client_._socket << "err:Login failed." << endl;
			}
		}
	} while ( false );
	return;
	M_EPILOG
}

void HServer::handle_account( OClientInfo& client_, HString const& accountInfo_ ) {
	M_PROLOG
	if ( client_._login.is_empty() )
		kick_client( client_._socket, "Set your name first (Just login with standard client, will ya?)." );
	else if ( client_._anonymous )
		kick_client( client_._socket, "Only registered users are allowed to do that." );
	else {
		HTokenizer t( accountInfo_, "," );
		int item( 0 );
		HString name;
		HString email;
		HString description;
		HString oldPassword;
		HString newPassword;
		HString newPasswordRepeat;
		HString clientSetup;
		for ( HTokenizer::HIterator it( t.begin() ), end( t.end() ); it != end; ++ it, ++ item ) {
			switch ( item ) {
				case ( 0 ): name = *it; break;
				case ( 1 ): email = *it; break;
				case ( 2 ): description = *it; break;
				case ( 3 ): clientSetup = *it; break;
				case ( 4 ): oldPassword = *it; break;
				case ( 5 ): newPassword = *it; break;
				case ( 6 ): newPasswordRepeat = *it; break;
				default: break;
			}
		}
		bool oldPasswordNull( oldPassword == NULL_PASS );
		bool newPasswordNull( newPassword == NULL_PASS );
		bool newPasswordRepeatNull( newPasswordRepeat == NULL_PASS );
		if ( oldPasswordNull && newPasswordNull && newPasswordRepeatNull ) {
			HQuery::ptr_t query( _db->prepare_query( "UPDATE tbl_user SET name = ?, email = ?, description = ?, setup = ? WHERE login = LOWER(?);" ) );
			query->bind( 1, escape_copy( name, _escapeTable_ ) );
			query->bind( 2, escape_copy( email, _escapeTable_ ) );
			query->bind( 3, escape_copy( description, _escapeTable_ ) );
			query->bind( 4, escape_copy( clientSetup, _escapeTable_ ) );
			query->bind( 5, client_._login );
			HRecordSet::ptr_t rs( query->execute() );
			M_ENSURE( !! rs );
		} else {
			if ( ! ( oldPasswordNull || newPasswordNull || newPasswordRepeatNull ) ) {
				if ( newPassword == newPasswordRepeat ) {
					if ( ! ( is_sha1( newPassword ) && is_sha1( oldPassword ) ) )
						kick_client( client_._socket, _msgYourClientIsTainted_ );
					else {
						HQuery::ptr_t query( _db->prepare_query( "UPDATE tbl_user SET name = ?, email = ?, description = ?, setup = ?, password = ?"
									" WHERE login = LOWER(?) AND password = LOWER(?);" ) );
						query->bind( 1, escape_copy( name, _escapeTable_ ) );
						query->bind( 2, escape_copy( email, _escapeTable_ ) );
						query->bind( 3, escape_copy( description, _escapeTable_ ) );
						query->bind( 4, escape_copy( clientSetup, _escapeTable_ ) );
						query->bind( 5, newPassword );
						query->bind( 6, client_._login );
						query->bind( 7, oldPassword );
						HRecordSet::ptr_t rs( query->execute() );
						M_ENSURE( !! rs );
						if ( rs->get_size() != 1 ) {
							*client_._socket << "warn:Password not changed - old password do not match.\n";
						}
					}
				} else {
					*client_._socket << "warn:Cannot change your password - passwords do not match.\n";
				}
			} else {
				*client_._socket << "warn:You have to enter all of - old, new and repeated passwords to change the password.\n";
			}
		}
	}
	return;
	M_EPILOG
}

void HServer::pass_command( OClientInfo& client_, HString const& command_ ) {
	M_PROLOG
	if ( client_._logics.is_empty() ) {
		*client_._socket << "err:Connect to some game first.\n";
	} else {
		HString id( get_token( command_, ":", 0 ) );
		logics_t::iterator logic( _logics.find( id ) );
		if ( logic == _logics.end() ) {
			*client_._socket << "err:No such party exists.\n";
		} else {
			if ( client_._logics.count( id ) == 0 ) {
				*client_._socket << "err:You are not part of this party.\n";
			} else {
				HString msg;
				try {
					msg = command_.mid( id.get_length() + 1 );
					if ( logic->second->process_command( &client_, msg ) ) {
						static int const MAX_MSG_LEN( 100 );
						char const err[] = "Game logic could not comprehend your message: ";
						if ( ( msg.get_length() + ( static_cast<int>( sizeof ( err ) ) - 1 ) ) > MAX_MSG_LEN ) {
							msg.erase( MAX_MSG_LEN - ( sizeof ( err ) - 1 ) );
						}
						msg.insert( 0, err, sizeof ( err ) - 1 );
					} else {
						msg.clear();
					}
				} catch ( HLogicException& e ) {
					msg = e.what();
				}
				if ( ! msg.is_empty() ) {
					remove_client_from_logic( client_, logic->second, msg );
				}
			}
		}
	}
	return;
	M_EPILOG
}

void HServer::create_party( OClientInfo& client_, HString const& arg_ ) {
	M_PROLOG
	if ( client_._login.is_empty() )
		kick_client( client_._socket, "Set your name first (Just login with standard client, will ya?)." );
	else {
		HString type = get_token( arg_, ":", 0 );
		HString configuration = get_token( arg_, ":", 1 );
		HLogicFactory& factory = HLogicFactoryInstance::get_instance();
		if ( ! factory.is_type_valid( type ) )
			kick_client( client_._socket, "No such game type." );
		else {
			HLogic::ptr_t logic;
			HLogic::id_t id( create_id() );
			try {
				logic = factory.create_logic( type, this, id, configuration );
				if ( ! logic->accept_client( &client_ ) ) {
					if ( id == logic->id() ) {
						_logics[ id ] = logic;
						OUT << "creating new party: " << logic->get_name() << "," << id << " (" << type << ')' << endl;
					} else {
						free_id( id );
						OUT << "reusing old party: " << logic->get_name() << "," << logic->id() << " (" << type << ')' << endl;
					}
					_out << PROTOCOL::PARTY_INFO << PROTOCOL::SEP << logic->id() << PROTOCOL::SEPP << logic->get_info() << endl;
					if ( ! logic->is_private() ) {
						broadcast( _out.consume() );
						broadcast_player_info( client_ );
					} else {
						*client_._socket << _out.consume();
						broadcast_player_info( client_, *logic );
					}
					logic->post_accept_client( &client_ );
				} else {
					free_id( id );
					*client_._socket << "err:Specified configuration is inconsistent.\n";
				}
			} catch ( HLogicException& e ) {
				kick_client( client_._socket, e.what() );
			}
		}
	}
	return;
	M_EPILOG
}

void HServer::join_party( OClientInfo& client_, HString const& id_ ) {
	M_PROLOG
	if ( client_._login.is_empty() )
		kick_client( client_._socket, "Set your name first (Just login with standard client, will ya?)." );
	else {
		logics_t::iterator it = _logics.find( id_ );
		if ( it == _logics.end() ) {
			*client_._socket << "err:Party does not exists.\n";
		} else if ( client_._logics.count( id_ ) != 0 ) {
			kick_client( client_._socket, "You were already in this party." );
		} else if ( ! it->second->accept_client( &client_ ) ) {
			if ( ! it->second->is_private() ) {
				broadcast_player_info( client_ );
			} else {
				*client_._socket << PROTOCOL::PARTY_INFO << PROTOCOL::SEP << id_ << PROTOCOL::SEPP << it->second->get_info() << endl;
				broadcast_player_info( client_, *it->second );
			}
			it->second->post_accept_client( &client_ );
		} else {
			*client_._socket << "err:You are not allowed in this party.\n";
		}
	}
	return;
	M_EPILOG
}

void HServer::send_logics_info( OClientInfo& client_ ) {
	M_PROLOG
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	for ( HLogicFactory::creators_t::iterator it = factory.begin();
			it != factory.end(); ++ it )
		SENDF( *client_._socket ) << PROTOCOL::LOGIC << PROTOCOL::SEP << it->second.get_info() << endl;
	return;
	M_EPILOG
}

void HServer::handle_get_logics( OClientInfo& client_, HString const& ) {
	M_PROLOG
	send_logics_info( client_ );
	return;
	M_EPILOG
}

void HServer::handler_abandon( OClientInfo& client_, HString const& id_ ) {
	M_PROLOG
	if ( client_._logics.count( id_ ) == 0 )
		kick_client( client_._socket, "You were not part of this party." );
	else {
		logics_t::iterator logic( _logics.find( id_ ) );
		if ( logic != _logics.end() ) {
			OUT << "client " << client_._login << " abandoned party `" << id_ << "'" << endl;
			remove_client_from_logic( client_, logic->second );
		}
	}
	return;
	M_EPILOG
}

void HServer::remove_client_from_all_logics( OClientInfo& client_ ) {
	M_PROLOG
	OUT << "removing client from all logics: " << client_._login << endl;
	for ( OClientInfo::logics_t::iterator it( client_._logics.begin() ), end( client_._logics.end() ); it != end; ) {
		HLogic::id_t id( *it );
		++ it;
		logics_t::iterator logic( _logics.find( id ) );
		M_ASSERT( logic != _logics.end() );
		logic->second->kick_client( &client_ );
	}
	client_._logics.clear();
	broadcast_player_info( client_ );
	flush_logics();
	return;
	M_EPILOG
}

void HServer::flush_logics( void ) {
	M_PROLOG
	for ( logics_t::iterator it( _logics.begin() ), end( _logics.end() ); it != end; ) {
		if ( ! it->second->active_clients() ) {
			logics_t::iterator del( it );
			++ it;
			broadcast( _out << PROTOCOL::PARTY_CLOSE << PROTOCOL::SEP << del->first << endl << _out );
			_logics.erase( del );
		} else
			++ it;
	}
	/* does not work becuse remove_if does not work on maps */
	// _logics.erase( remove_if( _logics.begin(), _logics.end(), not1( compose1( call( &HLogic::active_clients, _1 ), select2nd<logics_t::value_type>() ) ) ), _logics.end() );
	return;
	M_EPILOG
}

void HServer::remove_client_from_logic( OClientInfo& client_, HLogic::ptr_t logic_, HString const& reason_ ) {
	M_PROLOG
	if ( !! logic_ ) {
		OUT << "separating logic info from client info for: " << client_._login << " and party: " << logic_->get_info() << endl;
		logic_->kick_client( &client_, reason_ );
		HString const& id( logic_->id() );
		client_._logics.erase( id );
		if ( ! logic_->is_private() ) {
			broadcast_player_info( client_ );
		} else {
			broadcast_player_info( client_, *logic_ );
		}
		if ( ! logic_->active_clients() ) {
			if ( ! logic_->is_private() ) {
				broadcast( _out << PROTOCOL::PARTY_CLOSE << PROTOCOL::SEP << id << endl << _out );
			}
			_logics.erase( id );
		}
	}
	return;
	M_EPILOG
}

void HServer::handle_get_players( OClientInfo& client_, HString const& ) {
	M_PROLOG
	send_players_info( client_ );
	return;
	M_EPILOG
}

void HServer::handle_get_partys( OClientInfo& client_, HString const& ) {
	M_PROLOG
	send_partys_info( client_ );
	return;
	M_EPILOG
}

void HServer::handle_get_account( OClientInfo& client_, HString const& login_ ) {
	M_PROLOG
	if ( client_._login.is_empty() ) {
		kick_client( client_._socket, "Set your name first (Just login with standard client, will ya?)." );
	} else {
		bool accountSelf( login_.is_empty() || ( login_ == client_._login ) );
		HString const& login( accountSelf ? client_._login : login_ );
		char const accountQuerySelf[] = "SELECT name, description, email, setup FROM tbl_user WHERE login = LOWER(?);";
		char const accountQueryOther[] = "SELECT name, description FROM tbl_user WHERE login = LOWER(?);";
		HQuery::ptr_t query( _db->prepare_query( ( accountSelf ? accountQuerySelf : accountQueryOther ) ) );
		query->bind( 1, login );
		HRecordSet::ptr_t rs( query->execute() );
		M_ENSURE( !! rs );
		HRecordSet::iterator row( rs->begin() );
		if ( row != rs->end() ) {
			HRecordSet::value_t name( row[0] );
			HRecordSet::value_t description( row[1] );
			HRecordSet::value_t email;
			HRecordSet::value_t clientSetup;
			if ( accountSelf ) {
				email = row[2];
				clientSetup = row[3];
			}
			/* Account information is escaped twice,
			 * once for network transfer and once for database storage. */
			SENDF( *client_._socket ) << PROTOCOL::ACCOUNT << PROTOCOL::SEP
				<< login << PROTOCOL::SEPP
				<< ( name ? unescape_copy( *name, _escapeTable_ ) : "" ) << PROTOCOL::SEPP
				<< ( description ? unescape_copy( *description, _escapeTable_ ) : "" ) << PROTOCOL::SEPP
				<< ( email ? unescape_copy( *email, _escapeTable_ ) : "" ) << PROTOCOL::SEPP
				<< ( clientSetup ? unescape_copy( *clientSetup, _escapeTable_ ) : "" ) << endl;
		} else {
			SENDF( *client_._socket ) << PROTOCOL::ACCOUNT << PROTOCOL::SEP << login << endl;
		}
	}
	return;
	M_EPILOG
}

void HServer::broadcast_player_info( OClientInfo& client_ ) {
	M_PROLOG
	_out << PROTOCOL::PLAYER << PROTOCOL::SEP << client_._login;
	for ( OClientInfo::logics_t::iterator it( client_._logics.begin() ), end( client_._logics.end() ); it != end; ++ it ) {
		logics_t::iterator logic( _logics.find( *it ) );
		if ( logic != _logics.end() )
		 _out << PROTOCOL::SEPP << logic->first;
	}
	broadcast( _out << endl << _out );
	return;
	M_EPILOG
}

void HServer::broadcast_player_info( OClientInfo& client_, HLogic& logic_ ) {
	M_PROLOG
	_out << PROTOCOL::PLAYER << PROTOCOL::SEP << client_._login;
	for ( OClientInfo::logics_t::iterator it( client_._logics.begin() ), end( client_._logics.end() ); it != end; ++ it ) {
		logics_t::iterator logic( _logics.find( *it ) );
		if ( logic != _logics.end() )
		 _out << PROTOCOL::SEPP << logic->first;
	}
	broadcast_private( logic_, _out << endl << _out );
	return;
	M_EPILOG
}

void HServer::send_player_info( OClientInfo& about_, OClientInfo& to_ ) {
	M_PROLOG
	try {
		if ( ! about_._login.is_empty() ) {
			_out << PROTOCOL::PLAYER << PROTOCOL::SEP << about_._login;
			for ( OClientInfo::logics_t::iterator it( about_._logics.begin() ), end( about_._logics.end() ); it != end; ++ it ) {
				logics_t::iterator logic( _logics.find( *it ) );
				if ( logic != _logics.end() )
					_out << PROTOCOL::SEPP << logic->first;
			}
			_out << endl;
			SENDF( *to_._socket ) << _out.consume();
		}
	} catch ( HOpenSSLException const& ) {
		drop_client( &to_ );
	}
	return;
	M_EPILOG
}

void HServer::send_players_info( OClientInfo& client_ ) {
	M_PROLOG
	for( clients_t::iterator client = _clients.begin();
			client != _clients.end(); ++ client ) {
		if ( client->second._valid )
			send_player_info( client->second, client_ );
	}
	return;
	M_EPILOG
}

void HServer::send_partys_info( OClientInfo& client_ ) {
	M_PROLOG
	for( logics_t::iterator it( _logics.begin() ), end( _logics.end() ); it != end; ++ it )
		SENDF( *client_._socket ) << PROTOCOL::PARTY_INFO << PROTOCOL::SEP << it->first << PROTOCOL::SEPP << it->second->get_info() << endl;
	return;
	M_EPILOG
}

void HServer::update_last_activity( OClientInfo const& info_ ) {
	M_PROLOG
	HQuery::ptr_t query(
			_db->prepare_query(
				"UPDATE v_user_session SET last_activity = datetime('now', 'localtime') WHERE login = LOWER(?);" ) );
	query->bind( 1, info_._login );
	HRecordSet::ptr_t rs( query->execute() );
	M_ENSURE( !! rs );
	return;
	M_EPILOG
}

HLogic::id_t HServer::create_id( void ) {
	M_PROLOG
	HLogic::id_t id = _idPool.to_string();
	++ _idPool;
	return ( id );
	M_EPILOG
}

void HServer::free_id( HLogic::id_t const& id_ ) {
	M_PROLOG
	HNumber id( _idPool );
	-- id;
	if ( id.to_string() == id_ )
		_idPool.swap( id );
	return;
	M_EPILOG
}

void HServer::drop_client( OClientInfo* clientInfo_ ) {
	M_PROLOG
	_dropouts.push_back( clientInfo_ );
	clientInfo_->_valid = false;
	return;
	M_EPILOG
}

void HServer::flush_droupouts( void ) {
	M_PROLOG
	while ( ! _dropouts.is_empty() ) {
		OClientInfo* dropout( _dropouts.back() );
		_dropouts.pop_back();
		M_ASSERT( !! dropout->_socket );
		kick_client( dropout->_socket, NULL );
	}
	return;
	M_EPILOG
}

OClientInfo* HServer::get_client( HString const& login_ ) {
	M_PROLOG
	logins_t::iterator it( _logins.find( login_ ) );
	return ( it != _logins.end() ? it->second : NULL );
	M_EPILOG
}

HServer::db_accessor_t HServer::db( void ) {
	M_PROLOG
	db_accessor_t eaDB( HLock( _mutex ), _db );
	return ( eaDB );
	M_EPILOG
}

void HServer::cleanup( void ) {
	M_PROLOG
	HLogicFactory& factory( HLogicFactoryInstance::get_instance() );
	factory.cleanup_globals();
	_logins.clear();
	for ( clients_t::iterator it( _clients.begin() ), end( _clients.end() ); it != end; ) {
		clients_t::iterator del( it ++ );
		kick_client( del->second._socket, "Server is being shutdown." );
	}
	flush_logics();
	_clients.clear();
	return;
	M_EPILOG
}

void HServer::run( void ) {
	M_PROLOG
	try {
		_dispatcher.run();
	} catch ( ... ) {
		cleanup();
		throw;
	}
	cleanup();
	return;
	M_EPILOG
}

}

