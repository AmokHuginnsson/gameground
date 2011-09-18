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

namespace gameground {

static int const GALAXY_BOARD_SIZE         =  16;
static int const DEFAULT_MAX_CONNECTIONS   =  64;
static int const GALAXY_EMPERORS           =   4;
static int const NEUTRAL_SYSTEM_PER_PLAYER =   4;
static int const MAX_BOARD_SIZE            =  20;
static int const MAX_SYSTEM_COUNT          =  36;
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
