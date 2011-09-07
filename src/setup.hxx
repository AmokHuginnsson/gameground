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

#include "config.hxx"

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
extern char const* const BOGGLE_SCORING_SYSTEM;
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
static int const SET_STARTUP_PLAYERS       =   2;
#define out ( clog << now << " " << __FILE__ + OSetup::PATH_OFFSET << ":" << __LINE__ << ": " )

#ifndef NDEBUG
#define SENDF( sock ) tee( sock, out )
#define SEND( sock ) tee( sock, clog )
#else /* #ifndef NDEBUG */
#define SEND( sock ) ( sock )
#define SENDF( sock ) ( sock )
#endif /* #else #ifndef NDEBUG */

typedef double long ( *now_t )( bool, double, char, int, bool, double, void*, void*, double, char );
extern now_t now;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, now_t const& );

struct OSetup
	{
	bool _quiet;			/* --quiet, --silent */
	bool _verbose;		/* --verbose */
	bool _debug;
	int	_maxConnections;
	/* galaxy/boggle specific integers */
	int _emperors;
	int _port;
	int _systems;
	int _boardSize;
	int _boggleStarupPlayers;
	int _setStartupPlayers;
	int _roundTime;
	int _maxRounds;
	int _interRoundDelay;
	int _gobanSize;
	int _komi;
	int _handicaps;
	int _mainTime;
	int _byoYomiPeriods;
	int _byoYomiTime;
	/* galaxy/boggle specific strings */
	yaal::hcore::HString _scoringSystem;
	yaal::hcore::HString _consoleCharset;
	yaal::hcore::HString _aspellLang;
	yaal::hcore::HString _databasePath;
	yaal::hcore::HString _databaseLogin;
	yaal::hcore::HString _databasePassword;
	char* _programName;
	yaal::hcore::HString _logPath;
	/* self-sufficient */
	static int const PATH_OFFSET = sizeof ( __FILE__ ) - sizeof ( "setup.hxx" );
	OSetup ( void )
		: _quiet( false ), _verbose( false ), _debug( false ),
		_maxConnections( DEFAULT_MAX_CONNECTIONS ),
		_emperors( GALAXY_EMPERORS ), _port( 7777 ),
		_systems( GALAXY_EMPERORS * NEUTRAL_SYSTEM_PER_PLAYER ),
		_boardSize( GALAXY_BOARD_SIZE ), _boggleStarupPlayers( BOGGLE_PLAYERS ),
		_setStartupPlayers( SET_STARTUP_PLAYERS ),
		_roundTime( BOGGLE_ROUND_TIME ), _maxRounds( BOGGLE_MAX_ROUNDS ),
		_interRoundDelay( BOGGLE_INTER_ROUND_DELAY ),
		_gobanSize( GO_GOBAN_SIZE ), _komi( GO_KOMI ), _handicaps( GO_HANDICAPS ),
		_mainTime( GO_MAINTIME ), _byoYomiPeriods( GO_BYOYOMI_PERIODS ),
		_byoYomiTime( GO_BYOYOMI_TIME ), _scoringSystem( BOGGLE_SCORING_SYSTEM ),
		_consoleCharset(), _aspellLang(), _databasePath( DATABASE_PATH ),
		_databaseLogin( DATABASE_LOGIN ), _databasePassword( DATABASE_PASSWORD ),
		_programName( NULL ),
		_logPath() {}
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
