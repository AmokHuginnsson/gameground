/*
---            `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	setup.hxx - this file is integral part of `gameground' project.

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

#ifndef SETUP_HXX_INCLUDED
#define SETUP_HXX_INCLUDED

#include <libintl.h>
#include <iostream>

namespace gameground
{

extern char const* const DATABASE_PATH;
extern char const* const DATABASE_LOGIN;
extern char const* const DATABASE_PASSWORD;
static int const GALAXY_BOARD_SIZE         =  16;
static int const DEFAULT_MAX_CONNECTIONS   =  64;
static int const GALAXY_EMPERORS           =   4;
static int const NEUTRAL_SYSTEM_PER_PLAYER =   4;
static int const MAX_BOARD_SIZE            =  20;
static int const MAX_SYSTEM_COUNT          =  36;
static int const BOGGLE_PLAYERS            =   2;
static int const BOGGLE_ROUND_TIME         = 180;
static int const BOGGLE_INTER_ROUND_DELAY  =  15;
static int const BOGGLE_MAX_ROUNDS         =   5;
static int const GO_GOBAN_SIZE             =  19;
static int const GO_KOMI                   =   5;
static int const GO_HANDICAPS              =   0;
static int const GO_MAINTIME               =  30;
static int const GO_BYOYOMI_PERIODS        =   5;
static int const GO_BYOYOMI_TIME           =   1;
#define out ( cout << now << " " << __FILE__ + OSetup::PATH_OFFSET << ":" << __LINE__ << ": " )

typedef double long ( *now_t )( bool, double, char, int, bool, double, void*, void*, double, char );
extern now_t now;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, now_t const& );

inline std::ostream& operator << ( std::ostream& o, yaal::hcore::HString const& s )
	{ return ( o << s.raw() ); }

struct OSetup
	{
	bool f_bQuiet;			/* --quiet, --silent */
	int f_iVerbose;		/* --verbose */
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
	yaal::hcore::HString f_oDatabasePath;
	yaal::hcore::HString f_oDatabaseLogin;
	yaal::hcore::HString f_oDatabasePassword;
	char* f_pcProgramName;
	yaal::hcore::HString f_oLogPath;
	/* self-sufficient */
	static int const PATH_OFFSET = sizeof ( __FILE__ ) - sizeof ( "setup.hxx" );
	OSetup ( void )
		: f_bQuiet( false ), f_iVerbose( 0 ),
		f_iMaxConnections( DEFAULT_MAX_CONNECTIONS ),
		f_iEmperors( GALAXY_EMPERORS ), f_iPort( 7777 ),
		f_iSystems( GALAXY_EMPERORS * NEUTRAL_SYSTEM_PER_PLAYER ),
		f_iBoardSize( GALAXY_BOARD_SIZE ), f_iPlayers( BOGGLE_PLAYERS ),
		f_iRoundTime( BOGGLE_ROUND_TIME ), f_iMaxRounds( BOGGLE_MAX_ROUNDS ),
		f_iInterRoundDelay( BOGGLE_INTER_ROUND_DELAY ),
		f_iGobanSize( GO_GOBAN_SIZE ), f_iKomi( GO_KOMI ), f_iHandicaps( GO_HANDICAPS ),
		f_iMainTime( GO_MAINTIME ), f_iByoYomiPeriods( GO_BYOYOMI_PERIODS ),
		f_iByoYomiTime( GO_BYOYOMI_TIME ),
		f_oLogin(), f_oHost(), f_oGameType(), f_oGame(),
		f_oConsoleCharset(), f_oAspellLang(), f_oDatabasePath( DATABASE_PATH ),
		f_oDatabaseLogin( DATABASE_LOGIN ), f_oDatabasePassword( DATABASE_PASSWORD ),
		f_pcProgramName( NULL ),
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

#endif /* SETUP_HXX_INCLUDED */
