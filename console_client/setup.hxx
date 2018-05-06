/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef SETUP_HXX_INCLUDED
#define SETUP_HXX_INCLUDED

#include <libintl.h>
#include <iostream>

#include "config.hxx"

namespace gameground {

static int const GALAXY_BOARD_SIZE         =  16;
static int const DEFAULT_MAX_CONNECTIONS   =  64;
static int const GALAXY_EMPERORS           =   4;
static int const NEUTRAL_SYSTEM_PER_PLAYER =   4;
static int const MAX_BOARD_SIZE            =  20;
static int const MAX_SYSTEM_COUNT          =  36;
#define out ( clog << now << " " << __FILE__ + OSetup::PATH_OFFSET << ":" << __LINE__ << ": " )

typedef double long ( *now_t )( bool, double, char, int, bool, double, void*, void*, double, char );
extern now_t now;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, now_t const& );

struct OSetup {
	bool _quiet;			/* --quiet, --silent */
	bool _verbose;		/* --verbose */
	bool _debug;
	/* galaxy/boggle specific integers */
	int _emperors;
	int _port;
	int _systems;
	int _boardSize;
	/* galaxy/boggle specific strings */
	yaal::hcore::HString _login;
	yaal::hcore::HString _password;
	yaal::hcore::HString _host;
	yaal::hcore::HString _gameType;
	yaal::hcore::HString _game;
	char* _programName;
	yaal::hcore::HString _logPath;
	/* self-sufficient */
	static int const PATH_OFFSET = sizeof ( __FILE__ ) - sizeof ( "setup.hxx" );
	OSetup ( void )
		: _quiet( false ), _verbose( false ), _debug( false ),
		_emperors( GALAXY_EMPERORS ), _port( 7777 ),
		_systems( GALAXY_EMPERORS * NEUTRAL_SYSTEM_PER_PLAYER ),
		_boardSize( GALAXY_BOARD_SIZE ),
		_login(), _password(), _host(), _gameType(), _game(),
		_programName( NULL ), _logPath() {}
	void test_setup( void );
private:
	OSetup ( OSetup const & );
	OSetup & operator = ( OSetup const & );
};

extern OSetup setup;

}

#endif /* SETUP_HXX_INCLUDED */
