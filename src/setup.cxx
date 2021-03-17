/* Read gameground/LICENSE.md file for copyright and licensing information. */

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
char const* const BOGGLE_LANGUAGE = "pl";

now_t now;

HStreamInterface& operator << ( HStreamInterface& stream, now_t const& ) {
	static int const TIMESTAMP_SIZE = 16;
	time_t currentTime = ::time( nullptr );
	tm* brokenTime = ::localtime( &currentTime );
	char buffer[ TIMESTAMP_SIZE ];
	::memset( buffer, 0, TIMESTAMP_SIZE );
	::strftime( buffer, TIMESTAMP_SIZE, "%b %d %H:%M:%S", brokenTime );
	stream << buffer;
	return stream;
}

void OSetup::test_setup( void ) {
	M_PROLOG
	if ( _quiet && _verbose ) {
		yaal::tools::util::failure( 1,
				_( "quiet and verbose options are exclusive\n" ) );
	}
	if ( _verbose ) {
		clog.reset_owned( make_pointer<HFile>( stdout, HFile::OWNERSHIP::EXTERNAL ) );
	} else {
		std::clog.rdbuf( nullptr );
	}
	if ( _quiet ) {
		cout.reset_owned();
		std::cout.rdbuf( nullptr );
	}
	if ( _maxConnections < 2 ) {
		yaal::tools::util::failure ( 3,
				_( "this server hosts multiplayer games only\n" ) );
	}
	if ( _port < 1024 ) {
		yaal::tools::util::failure ( 5,
				_( "galaxy cannot run on restricted ports\n" ) );
	}
	char* message( nullptr );
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

