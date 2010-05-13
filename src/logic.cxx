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

HLogic::HLogic( HString const& a_oSymbol, HString const& a_oName )
	: f_oSymbol( a_oSymbol ), _handlers( setup._maxConnections ),
	_clients(), f_oName( a_oName ), _out()
	{
	}

HLogic::~HLogic( void )
	{
	}

void HLogic::kick_client( OClientInfo* a_poClientInfo, char const* const a_pcReason )
	{
	M_PROLOG
	a_poClientInfo->f_oLogic = HLogic::ptr_t();
	_clients.erase( a_poClientInfo );
	if ( a_pcReason )
		*a_poClientInfo->_socket << "err:" << a_pcReason << endl;
	do_kick( a_poClientInfo );
	broadcast( _out << PROTOCOL::PLAYER_QUIT << PROTOCOL::SEP << a_poClientInfo->_login << endl << _out );
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

bool HLogic::accept_client( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	bool rejected = false;
	if ( ! do_accept( a_poClientInfo ) )
		{
		_clients.insert( a_poClientInfo );
		do_post_accept( a_poClientInfo );
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

HString const& HLogic::get_name( void ) const
	{
	return ( f_oName );
	}

bool HLogic::process_command( OClientInfo* a_poClientInfo, HString const& a_roCommand )
	{
	M_PROLOG
	HString l_oMnemonic;
	HString l_oArgument;
	l_oArgument = a_roCommand;
	while ( ( l_oMnemonic = get_token( l_oArgument, ":", 0 ) ) == f_oSymbol )
		l_oArgument = l_oArgument.mid( l_oMnemonic.get_length() + 1 );
	l_oMnemonic = get_token( l_oArgument, ":", 0 );
	l_oArgument = l_oArgument.mid( l_oMnemonic.get_length() + 1 );
	bool failure = false;
	handlers_t::iterator it( _handlers.find( l_oMnemonic ) );
	if ( it != _handlers.end() )
		( this->*(it->second) )( a_poClientInfo, l_oArgument );
	else
		{
		failure = true, kick_client( a_poClientInfo );
		out << "mnemo: " << l_oMnemonic << ", arg: " << l_oArgument << ", cmd: " << a_roCommand << endl;
		}
	return ( failure );
	M_EPILOG
	}

void HLogic::broadcast( HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::HIterator it = _clients.begin(); it != _clients.end(); ++ it )
		(*it)->_socket->write_until_eos ( a_roMessage );
	return;
	M_EPILOG
	}

void HLogicCreatorInterface::initialize_globals( void )
	{
	do_initialize_globals();
	}

HLogic::ptr_t HLogicCreatorInterface::new_instance( HString const& a_oArgv )
	{
	return ( do_new_instance( a_oArgv ) );
	}

}

