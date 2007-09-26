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

static char const n_ppcDices[ 16 ][ 6 ] =
	{
		{ 'A', 'A', 'D', 'E', '£', 'Y' },
		{ 'A', 'A', 'I', 'K', 'M', 'Y' },
		{ 'A', 'C', 'I', 'N', 'N', 'P' },
		{ 'A', 'E', 'I', 'O', 'U', 'Y' },
		{ 'A', 'E', 'O', 'Y', 'Z', 'Z' },
		{ 'A', 'G', 'L', 'O', 'R', 'S' },
		{ '¡', 'C', 'G', 'M', 'Ñ', 'T' },
		{ 'B', 'E', 'E', 'P', 'Z', '¬' },
		{ 'B', 'I', 'I', 'N', 'R', 'W' },
		{ 'C', 'M', 'O', 'S', 'T', '¯' },
		{ 'Æ', 'J', 'M', 'T', 'V', 'Z' },
		{ 'D', 'E', 'K', 'N', 'O', 'R' },
		{ 'D', 'H', 'Ó', 'N', 'W', 'Z' },
		{ 'E', 'I', 'L', 'S', '¦', 'W' },
		{ 'Ê', 'F', 'H', 'J', 'L', 'S' },
		{ 'I', 'K', '£', 'O', 'P', 'R' }
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

HBoggle::HBoggle( HString const& a_oName, int a_iPlayers, int a_iRoundTime, int a_iMaxRounds )
	: HLogic( a_oName ), f_iPlayers( a_iPlayers ), f_iRoundTime( a_iRoundTime ),
	f_iMaxRounds( a_iMaxRounds ), f_iRound( 0 ), f_oHandlers( 16 ), f_oPlayers()
	{
	M_PROLOG
	HRandomizer l_oRandom;
	l_oRandom.set( time ( NULL ) );
	f_oHandlers [ "PLAY" ] = & HBoggle::handler_play;
	f_oHandlers [ "SAY" ] = & HBoggle::handler_message;
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
	}

void HBoggle::send_map( void )
	{
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		{
		if ( i && ! ( i % 4 ) )
			cout << endl;
		cout << boggle_data::n_ppcDices[ f_ppiGame[ i ][ 0 ] ][ f_ppiGame[ i ][ 1 ] ];
		}
	cout << endl;
	return;
	}

void HBoggle::process_command( OClientInfo* a_poClientInfo, HString const& a_roCommand )
	{
	M_PROLOG
	HString l_oMnemonic;
	HString l_oArgument;
	handler_t HANDLER;
	l_oMnemonic = a_roCommand.split ( ":", 0 );
	l_oArgument = a_roCommand.mid ( l_oMnemonic.get_length ( ) + 1 );
	if ( f_oHandlers.get ( l_oMnemonic, HANDLER ) )
		( this->*HANDLER ) ( a_poClientInfo, l_oArgument );
	else
		kick_client( a_poClientInfo );
	return;
	M_EPILOG
	}

void HBoggle::broadcast( OClientInfo*, HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::HIterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		(*it)->f_oSocket->write_until_eos ( a_roMessage );
	return;
	M_EPILOG
	}

void HBoggle::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "GLX:MSG:$";
	l_oMessage += get_player_info( a_poClientInfo )->f_iColor;
	l_oMessage += ';';
	l_oMessage += a_poClientInfo->f_oName;
	l_oMessage += ";$12;: ";
	l_oMessage += a_roMessage;
	l_oMessage += '\n';
	broadcast( NULL, l_oMessage );
	return;
	M_EPILOG
	}

void HBoggle::handler_play ( OClientInfo*, HString const& )
	{
	return;
	}

HBoggle::OPlayerInfo* HBoggle::get_player_info( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	players_t::HIterator it = f_oPlayers.find( a_poClientInfo );
	M_ASSERT( it != f_oPlayers.end() );
	return ( &it->second );
	M_EPILOG
	}

bool HBoggle::do_accept( OClientInfo* )
	{
	M_PROLOG
	bool rejected = false;
//	int l_iCtr = 0, l_iColor = -1, l_iSysNo = -1;
//	HString l_oMessage;
	if ( f_oPlayers.size() < static_cast<size_t>( f_iPlayers - 1 ) )
		{
		rejected = false;
		}
	else
		rejected = true;
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
	HString l_oName = a_oArgv.split( ",", 0 );
	HString l_oPlayers = a_oArgv.split( ",", 0 );
	HString l_oRoundTime = a_oArgv.split( ",", 1 );
	HString l_oEmperors = a_oArgv.split( ",", 2 );
	int l_iPlayers = strtol( l_oPlayers, NULL, 10 );
	int l_iRoundTime = strtol( l_oRoundTime, NULL, 10 );
	int l_iMaxRounds = strtol( l_oEmperors, NULL, 10 );
	char* l_pcMessage = NULL;
	if ( OSetup::test_glx_emperors( l_iMaxRounds, l_pcMessage )
			|| OSetup::test_glx_emperors_systems( l_iMaxRounds, l_iRoundTime, l_pcMessage )
			|| OSetup::test_glx_systems( l_iRoundTime, l_pcMessage )
			|| OSetup::test_glx_board_size( l_iPlayers, l_pcMessage ) )
		throw HLogicException( l_pcMessage );
	return ( HLogic::ptr_t( new boggle::HBoggle( l_oName,
					l_iPlayers,
					l_iRoundTime,
					l_iMaxRounds ) ) );
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
	l_oSetup.format( "%s:%d,%d,%d", "bgl", setup.f_iPlayers, setup.f_iRoundTime, setup.f_iMaxRounds );
	factory.register_logic_creator( l_oSetup, create_logic_boggle );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

