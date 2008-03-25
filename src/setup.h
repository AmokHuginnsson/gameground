/*
---            `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	setup.h - this file is integral part of `gameground' project.

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

#ifndef __SETUP_H
#define __SETUP_H

#include <libintl.h>

namespace gameground
{

#define D_GALAXY_BOARD_SIZE 								16
#define D_DEFAULT_MAX_CONNECTIONS		64
#define D_GALAXY_EMPERORS						4
#define D_NEUTRAL_SYSTEM_PER_PLAYER	4
#define D_MAX_BOARD_SIZE						20
#define D_MAX_SYSTEM_COUNT					36
#define D_BOGGLE_PLAYERS 									4
#define D_BOGGLE_ROUND_TIME								180
#define D_BOGGLE_INTER_ROUND_DELAY					15
#define D_BOGGLE_MAX_ROUNDS								5
#define D_GO_GOBAN_SIZE 19
#define D_GO_KOMI 5
#define D_GO_HANDICAPS 0
#define D_GO_MAINTIME 30
#define D_GO_BYOYOMI_PERIODS 5
#define D_GO_BYOYOMI_TIME 1
#define out ( cout << __FILE__ + OSetup::PATH_OFFSET << ":" << __LINE__ << ": " )

struct OSetup
	{
	bool f_bQuiet;			/* --quiet, --silent */
	bool f_bHelp;
	int f_iVerbose;		/* --verbose */
	/* galaxy specific bools */
	bool f_bServer;
	bool f_bClient;
	int	f_iMaxConnections;
	/* galaxy/boggle specific integers */
	int f_iEmperors;
	int f_iPort;
	int f_iSystems;
	int f_iBoardSize;
	int f_iPlayers;
	int f_iRoundTime;
	int f_iMaxRounds;
	int f_iInterRoundDelay;
	int f_iGobanSize;
	int f_iKomi;
	int f_iHandicaps;
	int f_iMainTime;
	int f_iByoYomiPeriods;
	int f_iByoYomiTime;
	/* galaxy/boggle specific strings */
	yaal::hcore::HString f_oLogin;
	yaal::hcore::HString f_oHost;
	yaal::hcore::HString f_oGameType;
	yaal::hcore::HString f_oGame;
	yaal::hcore::HString f_oConsoleCharset;
	yaal::hcore::HString f_oAspellLang;
	char * f_pcProgramName;
	yaal::hcore::HString f_oLogPath;
	/* self-sufficient */
	static int const PATH_OFFSET = sizeof ( __FILE__ ) - sizeof ( "setup.h" );
	OSetup ( void ) : f_bQuiet( false ), f_bHelp( false ),
										f_iVerbose( 0 ), f_bServer( false ),
										f_bClient( false ), f_iMaxConnections( D_DEFAULT_MAX_CONNECTIONS ),
										f_iEmperors( D_GALAXY_EMPERORS ),
										f_iPort( 7777 ),
										f_iSystems( D_GALAXY_EMPERORS * D_NEUTRAL_SYSTEM_PER_PLAYER ),
										f_iBoardSize( D_GALAXY_BOARD_SIZE ), f_iPlayers( D_BOGGLE_PLAYERS ),
										f_iRoundTime( D_BOGGLE_ROUND_TIME ), f_iMaxRounds( D_BOGGLE_MAX_ROUNDS ),
										f_iInterRoundDelay( D_BOGGLE_INTER_ROUND_DELAY ),
										f_iGobanSize( D_GO_GOBAN_SIZE ), f_iKomi( D_GO_KOMI ), f_iHandicaps( D_GO_HANDICAPS ),
										f_iMainTime( D_GO_MAINTIME ), f_iByoYomiPeriods( D_GO_BYOYOMI_PERIODS ),
										f_iByoYomiTime( D_GO_BYOYOMI_TIME ),
										f_oLogin(), f_oHost(), f_oGameType(), f_oGame(),
										f_oConsoleCharset(), f_oAspellLang(), f_pcProgramName( NULL ),
										f_oLogPath() {}
	void test_setup( void );
	static bool test_glx_emperors( int, char*& );
	static bool test_glx_emperors_systems( int, int, char*& );
	static bool test_glx_systems( int, char*& );
	static bool test_glx_board_size( int, char*& );
private:
	OSetup ( OSetup const & );
	OSetup & operator = ( OSetup const & );
	};

extern OSetup setup;

}

#endif /* __SETUP_H */
