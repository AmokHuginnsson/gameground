/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logic.cxx - this file is integral part of `gameground' project.

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
#include "logic.hxx"
#include "server.hxx"
#include "setup.hxx"
#include "clientinfo.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground
{

char const* const HLogic::PROTOCOL::SEP = ":";
char const* const HLogic::PROTOCOL::SEPP = ",";
char const* const HLogic::PROTOCOL::SAY = "say";
char const* const HLogic::PROTOCOL::MSG = "msg";
char const* const HLogic::PROTOCOL::PLAYER = "player";
char const* const HLogic::PROTOCOL::PLAYER_QUIT = "player_quit";

HLogic::HLogic( HServer* server_, id_t const& id_, HString const& symbol_, HString const& comment_ )
	: _server( server_ ), _id( id_ ), _symbol( symbol_ ), _handlers( setup._maxConnections ),
	_clients(), _comment( comment_ ), _out()
	{
	}

HLogic::~HLogic( void )
	{
	}

void HLogic::kick_client( OClientInfo* clientInfo_, char const* const reason_ )
	{
	M_PROLOG
	clientInfo_->_logics.erase( _id );
	_clients.erase( clientInfo_ );
	if ( reason_ )
		*clientInfo_->_socket << "err:" << reason_ << endl;
	do_kick( clientInfo_ );
	broadcast( _out << PROTOCOL::PLAYER_QUIT << PROTOCOL::SEP << clientInfo_->_login << endl << _out );
	return;
	M_EPILOG
	}

bool HLogic::do_accept( OClientInfo* )
	{
	return ( true );
	}

void HLogic::do_kick( OClientInfo* )
	{
	return;
	}

bool HLogic::accept_client( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	bool rejected = false;
	if ( ! do_accept( clientInfo_ ) )
		{
		_clients.insert( clientInfo_ );
		do_post_accept( clientInfo_ );
		}
	else
		rejected = true;
	return ( rejected );
	M_EPILOG
	}

int HLogic::active_clients( void ) const
	{
	M_PROLOG
	return ( static_cast<int>( _clients.size() ) );
	M_EPILOG
	}

HString HLogic::get_info( void ) const
	{
	return ( do_get_info() );
	}

HString const& HLogic::get_comment( void ) const
	{
	return ( _comment );
	}

bool HLogic::process_command( OClientInfo* clientInfo_, HString const& command_ )
	{
	M_PROLOG
	HString mnemonic;
	HString argument;
	argument = command_;
	while ( ( mnemonic = get_token( argument, ":", 0 ) ) == _symbol )
		argument = argument.mid( mnemonic.get_length() + 1 );
	mnemonic = get_token( argument, ":", 0 );
	argument = argument.mid( mnemonic.get_length() + 1 );
	bool failure = false;
	handlers_t::iterator it( _handlers.find( mnemonic ) );
	if ( it != _handlers.end() )
		( this->*(it->second) )( clientInfo_, argument );
	else
		{
		failure = true, kick_client( clientInfo_ );
		out << "mnemo: " << mnemonic << ", arg: " << argument << ", cmd: " << command_ << endl;
		}
	return ( failure );
	M_EPILOG
	}

void HLogic::broadcast( HString const& message_ )
	{
	M_PROLOG
	for ( clients_t::HIterator it = _clients.begin(); it != _clients.end(); ++ it )
		{
		try
			{
			(*it)->_socket->write_until_eos( message_ );
			}
		catch ( HOpenSSLException const& )
			{
			drop_client( *it );
			}
		}
	disect_dropouts();
	return;
	M_EPILOG
	}

HLogic::id_t HLogic::get_id( void ) const
	{
	return ( _id );
	}

void HLogicCreatorInterface::initialize_globals( void )
	{
	M_PROLOG
	do_initialize_globals();
	M_EPILOG
	}

HLogic::ptr_t HLogicCreatorInterface::new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ )
	{
	M_PROLOG
	return ( do_new_instance( server_, id_, argv_ ) );
	M_EPILOG
	}

HString HLogicCreatorInterface::get_info( void ) const
	{
	M_PROLOG
	return ( do_get_info() );
	M_EPILOG
	}

void HLogic::drop_client( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	_server->drop_client( clientInfo_ );
	M_EPILOG
	}

void HLogic::disect_dropouts( void )
	{
	M_PROLOG
	_server->disect_dropouts();
	M_EPILOG
	}

}

