/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef SETUP_HXX_INCLUDED
#define SETUP_HXX_INCLUDED

#include <libintl.h>

#include <yaal/hcore/hfile.hxx>

#include "config.hxx"

namespace gameground {

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
extern char const* const BOGGLE_LANGUAGE;
static int const BOGGLE_PLAYERS            =   2;
static int const BOGGLE_ROUND_TIME         = 180;
static int const BOGGLE_INTER_ROUND_DELAY  =  15;
static int const BOGGLE_MAX_ROUNDS         =   5;
static int const GO_GOBAN_SIZE             =  19;
static int const GO_KOMI                   = 550;
static int const GO_HANDICAPS              =   0;
static int const GO_MAINTIME               = 30 * 60;
static int const GO_BYOYOMI_PERIODS        =   5;
static int const GO_BYOYOMI_TIME           =  30;
static int const SET_STARTUP_PLAYERS       =   2;
static int const SET_DECK_COUNT            =   1;
#define OUT ( yaal::hcore::clog << now << " " << __FILE__ + OSetup::PATH_OFFSET << ":" << __LINE__ << ": " )

#ifndef NDEBUG
#define SENDF( sock ) tee( sock, OUT )
#define SEND( sock ) tee( sock, clog )
#else /* #ifndef NDEBUG */
#define SEND( sock ) ( sock )
#define SENDF( sock ) ( sock )
#endif /* #else #ifndef NDEBUG */

typedef double long ( *now_t )( bool, double, char, int, bool, double, void*, void*, double, char );
extern now_t now;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, now_t const& );

struct OSetup {
	bool _quiet;   /* --quiet, --silent */
	bool _verbose; /* --verbose */
	bool _debug;
	int _maxConnections;
	/* galaxy/boggle specific integers */
	int _emperors;
	int _port;
	int _systems;
	int _boardSize;
	int _boggleStarupPlayers;
	int _setStartupPlayers;
	int _setDeckCount;
	int _roundTime;
	int _boggleRounds;
	int _interRoundDelay;
	int _gobanSize;
	int _komi100;
	int _handicaps;
	int _mainTime;
	int _byoYomiPeriods;
	int _byoYomiTime;
	/* galaxy/boggle specific strings */
	yaal::hcore::HString _boggleLanguage;
	yaal::hcore::HString _scoringSystem;
	yaal::hcore::HString _aspellLang;
	yaal::hcore::HString _databasePath;
	yaal::hcore::HString _databaseLogin;
	yaal::hcore::HString _databasePassword;
	char* _programName;
	yaal::hcore::HString _logPath;
	/* self-sufficient */
	static int const PATH_OFFSET = sizeof ( __FILE__ ) - sizeof ( "setup.hxx" );
	OSetup ( void )
		: _quiet( false )
		, _verbose( false )
		, _debug( false )
		, _maxConnections( DEFAULT_MAX_CONNECTIONS )
		, _emperors( GALAXY_EMPERORS )
		, _port( 7777 )
		, _systems( GALAXY_EMPERORS * NEUTRAL_SYSTEM_PER_PLAYER )
		, _boardSize( GALAXY_BOARD_SIZE )
		, _boggleStarupPlayers( BOGGLE_PLAYERS )
		, _setStartupPlayers( SET_STARTUP_PLAYERS )
		, _setDeckCount( SET_DECK_COUNT )
		, _roundTime( BOGGLE_ROUND_TIME )
		, _boggleRounds( BOGGLE_MAX_ROUNDS )
		, _interRoundDelay( BOGGLE_INTER_ROUND_DELAY )
		, _gobanSize( GO_GOBAN_SIZE )
		, _komi100( GO_KOMI )
		, _handicaps( GO_HANDICAPS )
		, _mainTime( GO_MAINTIME )
		, _byoYomiPeriods( GO_BYOYOMI_PERIODS )
		, _byoYomiTime( GO_BYOYOMI_TIME )
		, _boggleLanguage( BOGGLE_LANGUAGE )
		, _scoringSystem( BOGGLE_SCORING_SYSTEM )
		, _aspellLang()
		, _databasePath( DATABASE_PATH )
		, _databaseLogin( DATABASE_LOGIN )
		, _databasePassword( DATABASE_PASSWORD )
		, _programName( NULL )
		, _logPath() {
	}
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
