/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	boggled.cxx - this file is integral part of `gameground' project.

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
#include "boggled.h"

#include "setup.h"
#include "clientinfo.h"
#include "logicfactory.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace boggle_data
{

static int const D_MINIMUM_WORD_LENGTH = 3;
static int const D_MAXIMUM_WORD_LENGTH = 16;

static char const n_ppcDices[ 16 ][ 6 ] =
	{
		{ 'a', 'a', 'd', 'e', '³', 'y' },
		{ 'a', 'a', 'i', 'k', 'm', 'y' },
		{ 'a', 'c', 'i', 'n', 'n', 'p' },
		{ 'a', 'e', 'i', 'o', 'u', 'y' },
		{ 'a', 'e', 'o', 'y', 'z', 'z' },
		{ 'a', 'g', 'l', 'o', 'r', 's' },
		{ '±', 'c', 'g', 'm', 'ñ', 't' },
		{ 'b', 'e', 'e', 'p', 'z', '¼' },
		{ 'b', 'i', 'i', 'n', 'r', 'w' },
		{ 'c', 'm', 'o', 's', 't', '¿' },
		{ 'æ', 'j', 'm', 't', 'v', 'z' },
		{ 'd', 'e', 'k', 'n', 'o', 'r' },
		{ 'd', 'h', 'ó', 'n', 'w', 'z' },
		{ 'e', 'i', 'l', 's', '¶', 'w' },
		{ 'ê', 'f', 'h', 'j', 'l', 's' },
		{ 'i', 'k', '³', 'o', 'p', 'r' }
	};

struct BOGGLE
	{
	enum
		{
		UNINITIALIZED_SLOT = -1,
		SIDES = 6,
		DICE_COUNT = 16
		};
	};

}

namespace boggle
{

HBoggle::HBoggle( HString const& a_oName, int a_iPlayers, int a_iRoundTime, int a_iMaxRounds, int a_iInterRoundDelay )
	: HLogic( "bgl", a_oName ), f_eState( STATE::D_LOCKED ), f_iPlayers( a_iPlayers ),
	f_iRoundTime( a_iRoundTime ), f_iMaxRounds( a_iMaxRounds ),
	f_iInterRoundDelay( a_iInterRoundDelay ), f_iRound( 0 ), f_oPlayers()
	{
	M_PROLOG
	HRandomizer l_oRandom;
	l_oRandom.set( time ( NULL ) );
	f_oHandlers[ "play" ] = static_cast<handler_t>( &HBoggle::handler_play );
	f_oHandlers[ "say" ] = static_cast<handler_t>( &HBoggle::handler_message );
	return;
	M_EPILOG
	}

HBoggle::~HBoggle ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HBoggle::generate_game( void )
	{
	M_PROLOG
	HRandomizer rnd;
	randomizer_helper::init_randomizer_from_time( rnd );
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		f_ppiGame[ i ][ 0 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		{
		int k = 0, l_iSlot = rnd.rnd( boggle_data::BOGGLE::DICE_COUNT - i );
		for ( int j = 0; j < l_iSlot; ++ j, ++ k )
			while ( f_ppiGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
				++ k;
		while ( f_ppiGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
			++ k;
		f_ppiGame[ k ][ 0 ] = i;
		f_ppiGame[ k ][ 1 ] = rnd.rnd( boggle_data::BOGGLE::SIDES );
		}
	return;
	M_EPILOG
	}

HString HBoggle::make_deck( void )
	{
	M_PROLOG
	HString l_oDeck;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		l_oDeck += boggle_data::n_ppcDices[ f_ppiGame[ i ][ 0 ] ][ f_ppiGame[ i ][ 1 ] ];
	return ( l_oDeck );
	M_EPILOG
	}

void HBoggle::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "bgl:msg:";
	l_oMessage += a_poClientInfo->f_oName;
	l_oMessage += ": ";
	l_oMessage += a_roMessage;
	l_oMessage += '\n';
	broadcast( l_oMessage );
	return;
	M_EPILOG
	}

void HBoggle::handler_play ( OClientInfo* a_poClientInfo, HString const& a_oWord )
	{
	M_PROLOG
	int l_iLength = a_oWord.get_length();
	if ( ( f_eState == STATE::D_ACCEPTING )
			&& ( l_iLength >= boggle_data::D_MINIMUM_WORD_LENGTH )
			&& ( l_iLength <= boggle_data::D_MAXIMUM_WORD_LENGTH ) )
		{
		OPlayerInfo* info = get_player_info( a_poClientInfo );
		info->f_oWords->insert( a_oWord );
		}
	return;
	M_EPILOG
	}

HBoggle::OPlayerInfo* HBoggle::get_player_info( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	players_t::HIterator it = f_oPlayers.find( a_poClientInfo );
	M_ASSERT( it != f_oPlayers.end() );
	return ( &it->second );
	M_EPILOG
	}

bool HBoggle::do_accept( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	bool rejected = false;
	out << "new candidate " << a_poClientInfo->f_oName << endl;
	HString l_oMessage;
	out << "conditions: f_oPlayers.size() = " << f_oPlayers.size() << ", f_iPlayers = " << f_iPlayers << endl;
	if ( f_oPlayers.size() < f_iPlayers )
		{
		OPlayerInfo info;
		f_oPlayers[ a_poClientInfo ] = info;
		l_oMessage = "player:";
		l_oMessage += a_poClientInfo->f_oName + ",0,0\n";
		broadcast( l_oMessage );
		for ( players_t::HIterator it = f_oPlayers.begin(); it != f_oPlayers.end(); ++ it )
			{
			l_oMessage = "player:";
			l_oMessage += it->first->f_oName + "," + it->second.f_iScore + "," + it->second.f_iLast + "\n";
			a_poClientInfo->f_oSocket->write_until_eos( l_oMessage );
			l_oMessage = "bgl:msg:";
			l_oMessage += it->first->f_oName + " joined the mind contest.\n";
			a_poClientInfo->f_oSocket->write_until_eos( l_oMessage );
			}
		l_oMessage = "bgl:msg:";
		l_oMessage += a_poClientInfo->f_oName + " joined the mind contest.\n";
		broadcast( l_oMessage );
		out << "player [" << a_poClientInfo->f_oName << "] accepted" << endl;
		if ( f_oPlayers.size() >= f_iPlayers )
			{
			l_oMessage = "bgl:deck:";
			generate_game();
			l_oMessage += make_deck();
			l_oMessage += "\n";
			f_eState = STATE::D_ACCEPTING;
			broadcast( l_oMessage );
			a_poClientInfo->f_oSocket->write_until_eos( l_oMessage );
			}
		rejected = false;
		}
	else
		{
		out << "player [" << a_poClientInfo->f_oName << "] rejected" << endl;
		rejected = true;
		}
	return ( rejected );
	M_EPILOG
	}

void HBoggle::do_kick( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	f_oPlayers.remove( a_poClientInfo );
	-- f_iPlayers;
	return;
	M_EPILOG
	}

yaal::hcore::HString HBoggle::get_info() const
	{
	return ( HString( "bgl," ) + f_oName + "," + f_oPlayers.size() + "," + f_iPlayers + "," + f_iRoundTime + "," + f_iMaxRounds );
	}

}

namespace logic_factory
{

HLogic::ptr_t create_logic_boggle( HString const& a_oArgv )
	{
	M_PROLOG
	out << "creating logic: " << a_oArgv << endl;
	HString l_oName = a_oArgv.split( ",", 0 );
	HString l_oPlayers = a_oArgv.split( ",", 1 );
	HString l_oRoundTime = a_oArgv.split( ",", 2 );
	HString l_oMaxRounds = a_oArgv.split( ",", 3 );
	HString l_oInterRoundDelay = a_oArgv.split( ",", 4 );
	int l_iPlayers = strtol( l_oPlayers, NULL, 10 );
	int l_iRoundTime = strtol( l_oRoundTime, NULL, 10 );
	int l_iMaxRounds = strtol( l_oMaxRounds, NULL, 10 );
	int l_iInterRoundDelay = strtol( l_oInterRoundDelay, NULL, 10 );
	return ( HLogic::ptr_t( new boggle::HBoggle( l_oName,
					l_iPlayers,
					l_iRoundTime,
					l_iMaxRounds,
					l_iInterRoundDelay ) ) );
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
	l_oSetup.format( "%s:%d,%d,%d,%d", "bgl", setup.f_iPlayers, setup.f_iRoundTime, setup.f_iMaxRounds, setup.f_iInterRoundDelay );
	factory.register_logic_creator( l_oSetup, create_logic_boggle );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

