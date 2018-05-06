/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "chatd.hxx"

#include "setup.hxx"
#include "server.hxx"
#include "client.hxx"
#include "logicfactory.hxx"
#include "spellchecker.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground {

namespace chat {

HChat::chats_t HChat::_chats_;

HChat::HChat( HServer* server_, HLogic::id_t const& id_, chatter_names_ptr_t chatterNames_, HString const& comment_ )
	: HLogic( server_, id_, comment_ ), _key( comment_ ), _chatterNames( chatterNames_ ), _info( HString( "chat," ) + _key + "," + get_comment() ) {
	M_PROLOG
	_handlers[ "say" ] = static_cast<handler_t>( &HChat::handler_message );
	return;
	M_EPILOG
}

HChat::~HChat ( void ) {
	M_PROLOG
	_chats_.erase( _key );
	return;
	M_EPILOG
}

bool HChat::do_accept( HClient* client_ ) {
	OUT << "new candidate " << client_->login() << endl;
	return ( _chatterNames->count( client_->login() ) != 1 );
}

void HChat::do_post_accept( HClient* ) {
	M_PROLOG
	HLock l( _mutex );
	return;
	M_EPILOG
}

void HChat::do_kick( HClient* client_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( client_->is_valid() ) {
		try {
			client_->send( _out << HServer::PROTOCOL::PARTY_CLOSE << PROTOCOL::SEP << _id << endl << _out );
		} catch ( HOpenSSLException const& ) {
			drop_client( client_ );
		}
	}
	_clients.erase( client_ );
	if ( _clients.is_empty() )
		_chats_.erase( _key );
	return;
	M_EPILOG
}

void HChat::handler_message( HClient* client_, HString const& message_ ) {
	M_PROLOG
	int chattersCount( static_cast<int>( _clients.get_size() ) );
	if ( chattersCount > 0 ) {
		if ( chattersCount != _chatterNames->get_size() ) {
			for ( chatter_names_t::iterator it( _chatterNames->begin() ), end( _chatterNames->end() ); it != end; ++ it ) {
				if ( *it != client_->login() ) {
					HClient* client( _server->get_client( *it ) );
					if ( client && _clients.count( client ) == 0 ) {
						_server->handle_get_account( *client, client_->login() );
						_server->join_party( *client, _id );
					}
				}
			}
		}
		HLogic::handler_message( client_, message_ );
	}
	return;
	M_EPILOG
}

yaal::hcore::HString HChat::do_get_info() const {
	HLock l( _mutex );
	return ( _info );
}

bool HChat::do_is_private( void ) const {
	return ( true );
}

HLogic::ptr_t HChat::get_chat( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	HString chatter1( get_token( argv_, ",", 0 ) );
	HString chatter2( get_token( argv_, ",", 1 ) );
	HTokenizer t( argv_, "," );
	HChat::chatter_names_ptr_t chatterNames( make_pointer<chatter_names_t>() );
	int keyBufferSize( 0 );
	for ( HTokenizer::iterator it( t.begin() ), end( t.end() ); it != end; ++ it ) {
		if ( it->is_empty() )
			throw HLogicException( "invalid chatters (empty)" );
		if ( ! chatterNames->insert( *it ).second )
			throw HLogicException( "invalid chatters (duplicate)" );
		keyBufferSize += static_cast<int>( it->get_length() );
	}
	if ( chatterNames->get_size() < 2 )
		throw HLogicException( "invalid chatters (bad count)" );
	keyBufferSize += static_cast<int>( chatterNames->get_size() );
	++ keyBufferSize;
	HString key( keyBufferSize, 0_ycp );
	for ( chatter_names_t::iterator it( chatterNames->begin() ), end( chatterNames->end() ); it != end; ++ it ) {
		if ( ! key.is_empty() )
			key += ':';
		key += *it;
	}
	chats_t::iterator it( _chats_.find( key ) );
	HLogic::ptr_t logic;
	if ( it != _chats_.end() ) {
		logic = it->second;
	} else {
		OUT << "creating logic: " << argv_ << endl;
		logic = make_pointer<chat::HChat>( server_, id_, chatterNames, key );
		_chats_.insert( make_pair( key, logic ) );
	}
	return ( logic );
	M_EPILOG
}

void HChat::cleanup( void ) {
	M_PROLOG
	_chats_.clear();
	return;
	M_EPILOG
}

}

namespace logic_factory {

class HChatCreator : public HLogicCreatorInterface {
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual HString do_get_info( void ) const;
	virtual void do_cleanup_globals( void );
} chatCreator;

HLogic::ptr_t HChatCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	return ( chat::HChat::get_chat( server_, id_, argv_ ) );
	M_EPILOG
}

void HChatCreator::do_cleanup_globals( void ) {
	M_PROLOG
	chat::HChat::cleanup();
	return;
	M_EPILOG
}

HString HChatCreator::do_get_info( void ) const {
	M_PROLOG
	HString setup;
	setup.assign( "chat" );
	OUT << setup << endl;
	return ( setup );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed( false );
	HLogicFactory& factory( HLogicFactoryInstance::get_instance() );
	factory.register_logic_creator( HTokenizer( chatCreator.get_info(), ":"  )[0], &chatCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

