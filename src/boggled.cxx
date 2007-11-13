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
#include "spellchecker.h"

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
	f_iInterRoundDelay( a_iInterRoundDelay ), f_iRound( 0 ), f_oPlayers(),
	f_oWords(), f_oVarTmpBuffer(), f_oMutex()
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
	HAsyncCallerService::get_instance().flush( this );
	HScheduledAsyncCallerService::get_instance().flush( this );
	return;
	M_EPILOG
	}

void HBoggle::generate_game( void )
	{
	M_PROLOG
	HRandomizer rnd;
	randomizer_helper::init_randomizer_from_time( rnd );
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		f_ppcGame[ i ][ 0 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		{
		int k = 0, l_iSlot = rnd.rnd( boggle_data::BOGGLE::DICE_COUNT - i );
		for ( int j = 0; j < l_iSlot; ++ j, ++ k )
			while ( f_ppcGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
				++ k;
		while ( f_ppcGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
			++ k;
		f_ppcGame[ k ][ 0 ] = boggle_data::n_ppcDices[ i ][ rnd.rnd( boggle_data::BOGGLE::SIDES ) ];
		}
	return;
	M_EPILOG
	}

HString HBoggle::make_deck( void )
	{
	M_PROLOG
	HString l_oDeck;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		l_oDeck += f_ppcGame[ i ][ 0 ];
	return ( l_oDeck );
	M_EPILOG
	}

void HBoggle::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
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
	HLock l( f_oMutex );
	int l_iLength = a_oWord.get_length();
	if ( ( f_eState == STATE::D_ACCEPTING )
			&& ( l_iLength >= boggle_data::D_MINIMUM_WORD_LENGTH )
			&& ( l_iLength <= boggle_data::D_MAXIMUM_WORD_LENGTH ) )
		{
		words_t::HIterator it = f_oWords.find( a_oWord );
		if ( it == f_oWords.end() )
			it = f_oWords.insert( a_oWord, client_set_ptr_t( new client_set_t() ) );
		it->second->insert( a_poClientInfo );
		out << "word: " << a_oWord << "< inserted, proof: " << f_oWords.size() << "," << it->second->size() << endl;
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
	HLock l( f_oMutex );
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
		l_oMessage += "bgl:msg:Welcome, this match settings are:\nbgl:msg:   round time - ";
		l_oMessage += f_iRoundTime;
		l_oMessage += " seconds\nbgl:msg:   number of rounds - ";
		l_oMessage += f_iMaxRounds;
		l_oMessage += "\nbgl:msg:   round interval - ";
		l_oMessage += f_iInterRoundDelay;
		l_oMessage += "\nbgl:msg:This match requires ";
		l_oMessage += f_iPlayers;
		l_oMessage += " players to start the game.\n";
		broadcast( l_oMessage );
		a_poClientInfo->f_oSocket->write_until_eos( l_oMessage );
		for ( players_t::HIterator it = f_oPlayers.begin(); it != f_oPlayers.end(); ++ it )
			{
			l_oMessage = "bgl:player:";
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
		if ( ! f_iRound && ( f_oPlayers.size() >= f_iPlayers ) )
			{
			schedule_end_round();
			a_poClientInfo->f_oSocket->write_until_eos( f_oVarTmpBuffer );
			l_oMessage = "bgl:msg:The match has begun, good luck!\n";
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
	HLock l( f_oMutex );
	f_oPlayers.remove( a_poClientInfo );
	for ( words_t::HIterator it = f_oWords.begin(); it != f_oWords.end(); ++ it )
		{
		if ( it->second->size() > 1 )
			continue;
		if ( *(it->second->begin()) == a_poClientInfo )
			it = f_oWords.erase( it );
		}
	return;
	M_EPILOG
	}

yaal::hcore::HString HBoggle::get_info() const
	{
	HLock l( f_oMutex );
	return ( HString( "bgl," ) + f_oName + "," + f_oPlayers.size() + "," + f_iPlayers + "," + f_iRoundTime + "," + f_iMaxRounds + "," + f_iInterRoundDelay );
	}

void HBoggle::schedule( EVENT::event_t a_eEvent )
	{
	M_PROLOG
	HLock l( f_oMutex );
	if ( a_eEvent == EVENT::D_BEGIN_ROUND )
		HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + f_iInterRoundDelay,
				HCallInterface::ptr_t( new HCall<HBoggle&, typeof( &HBoggle::on_begin_round )>( *this, &HBoggle::on_begin_round ) ) );
	else
		schedule_end_round();
	return;
	M_EPILOG
	}

void HBoggle::schedule_end_round( void )
	{
	M_PROLOG
	HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + f_iRoundTime,
			HCallInterface::ptr_t( new HCall<HBoggle&, typeof( &HBoggle::on_end_round )>( *this, &HBoggle::on_end_round ) ) );
	f_oVarTmpBuffer = "bgl:deck:";
	generate_game();
	f_oVarTmpBuffer += make_deck();
	f_oVarTmpBuffer += "\n";
	f_eState = STATE::D_ACCEPTING;
	broadcast( f_oVarTmpBuffer );
	return;
	M_EPILOG
	}

void HBoggle::on_begin_round( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	out << "<<begin>>" << endl;
	HAsyncCallerService::get_instance().register_call( 0,
			HCallInterface::ptr_t( new HCall<HBoggle&, typeof( &HBoggle::schedule ), EVENT::event_t>( *this, &HBoggle::schedule, EVENT::D_END_ROUND ) ) );
	++ f_iRound;
	HString l_oMessage = "bgl:msg:New round started, you have got ";
	l_oMessage += f_iRoundTime;
	l_oMessage += " seconds, ";
	l_oMessage += f_iMaxRounds - f_iRound;
	l_oMessage += " rounds left!\n";
	broadcast( l_oMessage );
	l_oMessage = "bgl:round:";
	l_oMessage += f_iRound;
	l_oMessage += "\n";
	broadcast( l_oMessage );
	return;
	M_EPILOG
	}

void HBoggle::on_end_round( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	out << "<<end>>" << endl;
	f_eState = STATE::D_LOCKED;
	HString l_oMsg;
	if ( f_iRound < f_iMaxRounds )
		{
		HAsyncCallerService::get_instance().register_call( 0,
				HCallInterface::ptr_t( new HCall<HBoggle&, typeof( &HBoggle::schedule ), EVENT::event_t>( *this, &HBoggle::schedule, EVENT::D_BEGIN_ROUND ) ) );
		l_oMsg = "bgl:msg:This round has ended, next round in ";
		l_oMsg += f_iInterRoundDelay;
		l_oMsg += " seconds!\n";
		}
	else
		l_oMsg = "bgl:msg:Game Over!\n";
	broadcast( l_oMsg );
	int scores[ 16 ] = { 0, 0, 1, 1, 2, 3, 5, 7, 11, 11, 11, 12, 13, 14, 15, 16  };
	typedef HList<words_t::HIterator> longest_t;
	longest_t longest;
	int l_iLongestLength = 0;
	for ( words_t::HIterator it = f_oWords.begin(); it != f_oWords.end(); ++ it )
		{
		if ( it->second->size() > 1 )
			continue;
		int l_iLength = it->first.get_length();
		if ( word_is_good( it->first, l_iLength ) )
			{
			if ( l_iLength > l_iLongestLength )
				{
				longest.clear();
				l_iLongestLength = l_iLength;
				}
			if ( l_iLength == l_iLongestLength )
				longest.push_back( it );
			OClientInfo* clInfo = *it->second->begin();
			OPlayerInfo& info = *get_player_info( clInfo );
			*(clInfo->f_oSocket) << "bgl:scored:" << it->first << "[" << scores[ l_iLength - 1 ] << "]" << endl;
			info.f_iLast += scores[ l_iLength - 1 ];
			}
		}
	for ( players_t::HIterator it = f_oPlayers.begin(); it != f_oPlayers.end(); ++ it )
		{
		l_oMsg = "bgl:player:";
		it->second.f_iScore += it->second.f_iLast;
		l_oMsg += it->first->f_oName + "," + it->second.f_iScore + "," + it->second.f_iLast + "\n";
		broadcast( l_oMsg );
		it->second.f_iLast = 0;
		}
	for ( longest_t::iterator it = longest.begin(); it != longest.end(); ++ it )
		{
		l_oMsg = "bgl:longest:";
		l_oMsg += (*it)->first;
		l_oMsg += " [";
		l_oMsg += (*(*it)->second->begin())->f_oName;
		l_oMsg += "]\n";
		broadcast( l_oMsg );
		}
	f_oWords.clear(); 
	return;
	M_EPILOG
	}

bool HBoggle::is_good( int f, char const* ptr, int length )
	{
	bool good = false;
	static int const D_DIRECTIONS = 8;
	int versor[ D_DIRECTIONS ][ 2 ] = 
		{
			{ -1, -1 }, /* top left */
			{ 0, -1 }, /* top */
			{ 1, -1 }, /* top right */
			{ -1, 0 }, /* left */
			{ 1, 0 }, /* rigth */
			{ -1, 1 }, /* bottom down */
			{ 0, 1 }, /* bottom */
			{ 1, 1 } /* bottom right */
		};
	if ( ! f_ppcGame[ f ][ 1 ] && ( *ptr == f_ppcGame[ f ][ 0 ] ) )
		{
		if ( length == 1 )
			good = true;
		else
			{
			f_ppcGame[ f ][ 1 ] = 1;
			int x = f % 4;
			int y = f / 4;
			for ( int i = 0; ! good && ( i < D_DIRECTIONS ); ++ i )
				{
				int nx = x + versor[ i ][ 0 ];
				int ny = y + versor[ i ][ 1 ]; 
				if ( ( nx >= 0 ) && ( nx < 4 ) && ( ny >= 0 ) && ( ny < 4 ) )
					good = is_good( ny * 4 + nx, ptr + 1, length - 1 );
				}
			}
		}
	f_ppcGame[ f ][ 1 ] = 0;
	return ( good );
	}

bool HBoggle::word_is_good( HString const& a_oWord, int a_iLength )
	{
	M_PROLOG
	bool good = false;
	char const* ptr = a_oWord;
	for ( int f = 0; f < boggle_data::D_MAXIMUM_WORD_LENGTH; ++ f )
		f_ppcGame[ f ][ 1 ] = 0;
	for ( int f = 0; f < boggle_data::D_MAXIMUM_WORD_LENGTH; ++ f )
		{
		if ( is_good( f, ptr, a_iLength ) )
			{
			good = true;
			break;
			}
		}
	if ( good )
		good = ! HSpellCheckerService::get_instance().spell_check( a_oWord );
	return ( good );
	M_EPILOG
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

