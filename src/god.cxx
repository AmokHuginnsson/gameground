/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	god.cxx - this file is integral part of `gameground' project.

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
#include "god.h"

#include "setup.h"
#include "clientinfo.h"
#include "logicfactory.h"
#include "spellchecker.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace go
{

HGo::HGo( HString const& a_oName )
	: HLogic( "go", a_oName ), f_eMove( MOVE::D_BLACK ), f_iGobanSize( setup.f_iGobanSize ),
	f_iKomi( setup.f_iKomi ), f_iHandicaps( setup.f_iHandicaps ), f_iMainTime( setup.f_iMainTime ),
	f_iByoYomiPeriods( setup.f_iByoYomiPeriods ), f_iByoYomiTime( setup.f_iByoYomiTime ),
	f_iMove( 0 ), f_ppcGame( NULL ), f_oPlayers(), f_oVarTmpBuffer(), f_oMutex()
	{
	M_PROLOG
	HRandomizer l_oRandom;
	l_oRandom.set( time ( NULL ) );
	f_oHandlers[ "setup" ] = static_cast<handler_t>( &HGo::handler_setup );
	f_oHandlers[ "play" ] = static_cast<handler_t>( &HGo::handler_play );
	f_oHandlers[ "say" ] = static_cast<handler_t>( &HGo::handler_message );
	return;
	M_EPILOG
	}

HGo::~HGo ( void )
	{
	M_PROLOG
	HAsyncCallerService::get_instance().flush( this );
	HScheduledAsyncCallerService::get_instance().flush( this );
	return;
	M_EPILOG
	}

void HGo::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	HString l_oMessage;
	l_oMessage = "go:msg:";
	l_oMessage += a_poClientInfo->f_oName;
	l_oMessage += ": ";
	l_oMessage += a_roMessage;
	l_oMessage += '\n';
	broadcast( l_oMessage );
	return;
	M_EPILOG
	}

void HGo::handler_setup( OClientInfo*, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	broadcast( HString( "go:setup:" ) + a_roMessage + "\n" );
	return;
	M_EPILOG
	}

void HGo::handler_play ( OClientInfo*, HString const& )
	{
	M_PROLOG
	HLock l( f_oMutex );
	return;
	M_EPILOG
	}

HGo::players_t::iterator HGo::find_player( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	players_t::iterator it;
	for ( it = f_oPlayers.begin(); it != f_oPlayers.end(); ++ it )
		if ( it->first == a_poClientInfo )
			break;
	return ( it );
	M_EPILOG
	}

HGo::OPlayerInfo* HGo::get_player_info( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	players_t::iterator it = find_player( a_poClientInfo );
	M_ASSERT( it != f_oPlayers.end() );
	return ( &it->second );
	M_EPILOG
	}

bool HGo::do_accept( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	bool rejected = false;
	out << "new candidate " << a_poClientInfo->f_oName << endl;
	if ( f_oPlayers.size() == 0 )
		*a_poClientInfo->f_oSocket << "go:setup:admin" << endl;
	*a_poClientInfo->f_oSocket
		<< "go:setup:goban," << f_iGobanSize << endl
		<< "go:setup:komi," << f_iKomi << endl
		<< "go:setup:handicap," << f_iHandicaps << endl
		<< "go:setup:maintime," << f_iMainTime << endl
		<< "go:setup:byoyomipediods," << f_iByoYomiPeriods << endl
		<< "go:setup:byoyomitime," << f_iByoYomiTime << endl;
	player_t info;
	info.first = a_poClientInfo;
	f_oPlayers.push_back( info );
	return ( rejected );
	M_EPILOG
	}

void HGo::do_kick( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	bool newadmin = false;
	players_t::iterator it = find_player( a_poClientInfo );
	if ( it == f_oPlayers.begin() )
		newadmin = true;
	f_oPlayers.erase( it );
	if ( newadmin )
		{
		it = f_oPlayers.begin();
		if ( it != f_oPlayers.end() )
			*it->first->f_oSocket << "go:setup:admin" << endl;
		}
	broadcast( HString( "go:msg:Player " ) + a_poClientInfo->f_oName + " left this match.\n" );
	return;
	M_EPILOG
	}

yaal::hcore::HString HGo::get_info() const
	{
	HLock l( f_oMutex );
	return ( HString( "go," ) + f_oName + "," + f_oPlayers.size() + "," + f_iGobanSize + "," + f_iKomi + "," + f_iHandicaps + "," + f_iMainTime + "," + f_iByoYomiPeriods + "," + f_iByoYomiTime );
	}

void HGo::schedule( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	schedule_timeout();
	return;
	M_EPILOG
	}

void HGo::schedule_timeout( void )
	{
	M_PROLOG
	++ f_iMove;
/*
	HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + f_iRoundTime,
			HCallInterface::ptr_t( new HCall<HGo&, typeof( &HGo::on_timeout )>( *this, &HGo::on_timeout ) ) );
*/
	return;
	M_EPILOG
	}

void HGo::on_timeout( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	return;
	M_EPILOG
	}

}

namespace logic_factory
{

HLogic::ptr_t create_logic_go( HString const& a_oArgv )
	{
	M_PROLOG
	out << "creating logic: " << a_oArgv << endl;
	HString l_oName = a_oArgv.split( ",", 0 );
	return ( HLogic::ptr_t( new go::HGo( l_oName ) ) );
	M_EPILOG
	}

namespace
{

bool registrar( void )
	{
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	HString l_oSetup;
	l_oSetup.format( "go:%d,%d,%d,%d,%d,%d", setup.f_iGobanSize, setup.f_iKomi, setup.f_iHandicaps,
			setup.f_iMainTime, setup.f_iByoYomiPeriods, setup.f_iByoYomiTime );
	factory.register_logic_creator( l_oSetup, create_logic_go );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

