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
#include <cstdio>

#include <yaal/hcore/hfile.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "setup.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground {

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
	if ( _host.is_empty() ) {
		yaal::tools::util::failure( 6,
				_( "as a client you must specify server host\n" ) );
	}
	if ( _login.is_empty() ) {
		yaal::tools::util::failure( 7,
				_( "as a player you must specify Your name\n" ) );
	}
	if ( _gameType.is_empty() && _game.is_empty() ) {
		yaal::tools::util::failure( 11, _( "as client you have to specify game to play\n" ) );
	}
	if ( _emperors < 2 ) {
		yaal::tools::util::failure( 4, _( "galaxy is multiplayer game and makes sense"
					" only for at least two players\n" ) );
	}
	if ( ( _boardSize < 6 ) || ( _boardSize > MAX_BOARD_SIZE ) ) {
		yaal::tools::util::failure( 8, _( "bad board size specified\n" ) );
	}
	if ( ( _emperors + _systems ) > MAX_SYSTEM_COUNT ) {
		yaal::tools::util::failure( 9, _( "bad total system count\n" ) );
	}
	if ( _systems < 0 ) {
		yaal::tools::util::failure( 10, _( "neutral system count has to be nonnegative number\n" ) );
	}
	if ( ! ( _gameType.is_empty() || _game.is_empty() ) ) {
		yaal::tools::util::failure( 12, _( "creating new game is enought, you do not have to join it explicite\n" ) );
	}
	if ( ! _gameType.is_empty() ) {
		HString type = get_token( _gameType, ",", 0 );
		_game = get_token( _gameType, ",", 1 );
		_gameType = type;
		if ( _gameType.is_empty() || _game.is_empty() || ( _gameType == "" ) || ( _game == "" ) ) {
			yaal::tools::util::failure ( 13, _( "when creating new game, you have specify both type and name of new game\n" ) );
		}
	}
	return;
	M_EPILOG
}

}

