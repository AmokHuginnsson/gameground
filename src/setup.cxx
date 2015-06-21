/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  setup.cxx - this file is integral part of `gameground' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <iostream>

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/tools/util.hxx>
#include <yaal/dbwrapper/hdatabase.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "setup.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace yaal::dbwrapper;

namespace gameground {

char const* const DATABASE_PATH     = "gameground";
char const* const DATABASE_LOGIN    = "gameground";
char const* const DATABASE_PASSWORD = "g4m3gr0und";
char const* const BOGGLE_SCORING_SYSTEM = "original";

now_t now;

HStreamInterface& operator << ( HStreamInterface& stream, now_t const& ) {
	static int const TIMESTAMP_SIZE = 16;
	time_t currentTime = ::time( NULL );
	tm* brokenTime = ::localtime( &currentTime );
	char buffer[ TIMESTAMP_SIZE ];
	::memset( buffer, 0, TIMESTAMP_SIZE );
	::strftime( buffer, TIMESTAMP_SIZE, "%b %d %H:%M:%S", brokenTime );
	stream << buffer;
	return ( stream );
}

void OSetup::test_setup( void ) {
	M_PROLOG
	if ( _quiet && _verbose ) {
		yaal::tools::util::failure( 1,
				_( "quiet and verbose options are exclusive\n" ) );
	}
	if ( _verbose ) {
		clog.reset( make_pointer<HFile>( stdout, HFile::OWNERSHIP::EXTERNAL ) );
	} else {
		std::clog.rdbuf( NULL );
	}
	if ( _quiet ) {
		cout.reset();
		std::cout.rdbuf( NULL );
	}
	if ( _maxConnections < 2 ) {
		yaal::tools::util::failure ( 3,
				_( "this server hosts multiplayer games only\n" ) );
	}
	if ( _port < 1024 ) {
		yaal::tools::util::failure ( 5,
				_( "galaxy cannot run on restricted ports\n" ) );
	}
	char* message( NULL );
	if ( test_glx_emperors( _emperors, message ) ) {
		yaal::tools::util::failure( 4, "%s", message );
	}
	if ( test_glx_board_size( _boardSize, message ) ) {
		yaal::tools::util::failure( 8, "%s", message );
	}
	if ( test_glx_emperors_systems( _emperors, _systems, message ) ) {
		yaal::tools::util::failure( 9, "%s", message );
	}
	if ( test_glx_systems( _systems, message ) ) {
		yaal::tools::util::failure( 10, "%s", message );
	}
	HDataBase::ptr_t db = HDataBase::get_connector();
	db->connect( setup._databasePath, setup._databaseLogin, setup._databasePassword );
	return;
	M_EPILOG
}

bool OSetup::test_glx_emperors( int emperors_, char*& message_ ) {
	return ( ( emperors_ < 2 )
			&& ( message_ = _( "galaxy is multiplayer game and makes sense"
					" only for at least two players\n" ) ) );
}

bool OSetup::test_glx_emperors_systems( int emperors_, int systems_, char*& message_ ) {
	return ( ( ( emperors_ + systems_ ) > MAX_SYSTEM_COUNT )
			&& ( message_ = _( "bad total system count\n" ) ) );
}

bool OSetup::test_glx_systems( int systems_, char*& message_ ) {
	return ( ( systems_ < 0 )
			&& ( message_ = _( "neutral system count has to be nonnegative number\n" ) ) );
}

bool OSetup::test_glx_board_size( int boardSize_, char*& message_ ) {
	return ( ( ( boardSize_ < 6 ) || ( boardSize_ > MAX_BOARD_SIZE ) )
			&& ( message_ = _( "bad board size specified\n" ) ) );
}

}

