/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "logic.hxx"
#include "server.hxx"
#include "setup.hxx"
#include "client.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground {

char const* const HLogic::PROTOCOL::SEP = ":";
char const* const HLogic::PROTOCOL::SEPP = ",";
char const* const HLogic::PROTOCOL::SAY = "say";
char const* const HLogic::PROTOCOL::MSG = "msg";
char const* const HLogic::PROTOCOL::PLAYER = "player";
char const* const HLogic::PROTOCOL::PARTY = "party";
char const* const HLogic::PROTOCOL::PLAYER_QUIT = "player_quit";

HLogic::HLogic( HServer* server_, id_t const& id_, HString const& comment_ )
	: _server( server_ )
	, _id( id_ )
	, _name( get_token( comment_, ",", 0 ) )
	, _handlers( setup._maxConnections )
	, _clients()
	, _comment( comment_ )
	, _out()
	, _mutex()
	, _bcastPrefix()
	, _cache() {
	M_PROLOG
	_handlers[ PROTOCOL::SAY ] = static_cast<handler_t>( &HLogic::handler_message );
	_out << PROTOCOL::PARTY << PROTOCOL::SEP << _id << PROTOCOL::SEPP;
	_bcastPrefix = _out.consume();
	return;
	M_EPILOG
}

HLogic::~HLogic( void ) {
	OUT << "Destroying logic: " << _id << endl;
}

void HLogic::kick_client( HClient* client_, yaal::hcore::HString const& reason_ ) {
	M_PROLOG
	OUT << "kicking player `" << client_->login() << "' from " << _name << "," << _id << endl;
	client_->leave( _id );
	_clients.erase( client_ );
	if ( ! reason_.is_empty() ) {
		client_->send( _out << "err:" << reason_ << endl << _out );
	}
	do_kick( client_ );
	broadcast( _out << PROTOCOL::PLAYER_QUIT << PROTOCOL::SEP << client_->login() << endl << _out );
	return;
	M_EPILOG
}

bool HLogic::do_accept( HClient* ) {
	return ( true );
}

void HLogic::do_kick( HClient* ) {
	return;
}

bool HLogic::accept_client( HClient* client_ ) {
	M_PROLOG
	bool rejected( false );
	if ( ! do_accept( client_ ) ) {
		_clients.insert( client_ );
		client_->enter( _id );
	} else {
		rejected = true;
	}
	return rejected;
	M_EPILOG
}

void HLogic::post_accept_client( HClient* client_ ) {
	M_PROLOG
	do_post_accept( client_ );
	return;
	M_EPILOG
}

int HLogic::active_clients( void ) const {
	M_PROLOG
	return ( static_cast<int>( _clients.size() ) );
	M_EPILOG
}

HString HLogic::get_info( void ) const {
	return ( do_get_info() );
}

HString const& HLogic::get_comment( void ) const {
	return ( _comment );
}

HString const& HLogic::get_name( void ) const {
	return ( _name );
}

bool HLogic::process_command( HClient* client_, HString const& command_ ) {
	M_PROLOG
	HString mnemonic( get_token( command_, ":", 0 ) );
	HString argument( command_.mid( mnemonic.get_length() + 1 ) );
	bool failure( false );
	handlers_t::iterator it( _handlers.find( mnemonic ) );
	if ( it != _handlers.end() ) {
		( this->*(it->second) )( client_, argument );
	} else {
		failure = true;
		OUT << "mnemo: " << mnemonic << ", arg: " << argument << ", cmd: " << command_ << endl;
	}
	return failure;
	M_EPILOG
}

void HLogic::party( HStreamInterface& stream_ ) const {
	stream_ << _bcastPrefix;
}

yaal::hcore::HStreamInterface& operator << ( HStreamInterface& stream_, HLogic const& party_ ) {
	party_.party( stream_ );
	return stream_;
}

void HLogic::broadcast( HString const& message_ ) {
	M_PROLOG
	_cache.assign( _bcastPrefix ).append( message_ );
	for ( clients_t::HIterator it( _clients.begin() ), end( _clients.end() ); it != end; ++ it ) {
		try {
			if ( (*it)->is_valid() ) {
				(*it)->send( _cache );
			}
		} catch ( HOpenSSLException const& ) {
			drop_client( *it );
		}
	}
	return;
	M_EPILOG
}

void HLogic::handler_message( HClient* client_, HString const& message_ ) {
	M_PROLOG
	HLock l( _mutex );
	broadcast( _out << PROTOCOL::SAY << PROTOCOL::SEP << client_->login() << ": " << message_ << endl << _out );
	return;
	M_EPILOG
}

HLogic::id_t HLogic::id( void ) const {
	return ( _id );
}

void HLogicCreatorInterface::initialize_globals( void ) {
	M_PROLOG
	do_initialize_globals();
	return;
	M_EPILOG
}

void HLogicCreatorInterface::cleanup_globals( void ) {
	M_PROLOG
	do_cleanup_globals();
	return;
	M_EPILOG
}

HLogic::ptr_t HLogicCreatorInterface::new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	return ( do_new_instance( server_, id_, argv_ ) );
	M_EPILOG
}

HString HLogicCreatorInterface::get_info( void ) const {
	M_PROLOG
	return ( do_get_info() );
	M_EPILOG
}

void HLogic::drop_client( HClient* client_ ) {
	M_PROLOG
	_server->drop_client( client_ );
	return;
	M_EPILOG
}

bool HLogic::is_private( void ) const {
	M_PROLOG
	return ( do_is_private() );
	M_EPILOG
}

bool HLogic::do_is_private( void ) const {
	return ( false );
}

}

