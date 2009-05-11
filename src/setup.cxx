/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	setup.cxx - this file is integral part of `gameground' project.

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

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "setup.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace yaal::dbwrapper;

namespace gameground
{

char const* const DATABASE_PATH     = "gameground";
char const* const DATABASE_LOGIN    = "gameground";
char const* const DATABASE_PASSWORD = "g4m3gr0und";

now_t now;

HStreamInterface& operator << ( HStreamInterface& stream, now_t const& )
	{
	static int const TIMESTAMP_SIZE = 16;
	time_t l_xCurrentTime = ::time( NULL );
	tm* l_psBrokenTime = ::localtime( &l_xCurrentTime );
	char l_pcBuffer[ TIMESTAMP_SIZE ];
	::memset( l_pcBuffer, 0, TIMESTAMP_SIZE );
	::strftime( l_pcBuffer, TIMESTAMP_SIZE, "%b %d %H:%M:%S", l_psBrokenTime );
	stream << l_pcBuffer;
	return ( stream );
	}

void OSetup::test_setup( void )
	{
	M_PROLOG
#ifdef __GAMEGROUND_SERVER__
	if ( f_iMaxConnections < 2 )
		yaal::tools::util::failure ( 3,
				_( "this server hosts multiplayer games only\n" ) );
	if ( f_iPort < 1024 )
		yaal::tools::util::failure ( 5,
				_( "galaxy cannot run on restricted ports\n" ) );
#endif /* __GAMEGROUND_SERVER__ */
#ifdef __GAMEGROUND_CLIENT__
	if ( f_oHost.is_empty() )
		yaal::tools::util::failure( 6,
				_( "as a client you must specify server host\n" ) );
	if ( f_oLogin.is_empty() )
		yaal::tools::util::failure( 7,
				_( "as a player you must specify Your name\n" ) );
	if ( f_oGameType.is_empty() && f_oGame.is_empty() )
		yaal::tools::util::failure( 11, _( "as client you have to specify game to play\n" ) );
#endif /* __GAMEGROUND_CLIENT__ */
	char* l_pcMessage = NULL;
	if ( test_glx_emperors( f_iEmperors, l_pcMessage ) )
		yaal::tools::util::failure ( 4, l_pcMessage );
	if ( test_glx_board_size( f_iBoardSize, l_pcMessage ) )
		yaal::tools::util::failure ( 8, l_pcMessage );
	if ( test_glx_emperors_systems( f_iEmperors, f_iSystems, l_pcMessage ) )
		yaal::tools::util::failure ( 9, l_pcMessage );
	if ( test_glx_systems( f_iSystems, l_pcMessage ) )
		yaal::tools::util::failure ( 10, l_pcMessage );
	if ( ! ( f_oGameType.is_empty() || f_oGame.is_empty() ) )
		yaal::tools::util::failure ( 12, _( "creating new game is enought, you do not have to join it explicite\n" ) );
	if ( ! f_oGameType.is_empty() )
		{
		HString type = get_token( f_oGameType, ",", 0 );
		f_oGame = get_token( f_oGameType, ",", 1 );
		f_oGameType = type;
		if ( f_oGameType.is_empty() || f_oGame.is_empty() || ( f_oGameType == "" ) || ( f_oGame == "" ) )
			yaal::tools::util::failure ( 13, _( "when creating new game, you have specify both type and name of new game\n" ) );
		}
	HDataBase::ptr_t db = HDataBase::get_connector();
	db->connect( setup.f_oDatabasePath, setup.f_oDatabaseLogin, setup.f_oDatabasePassword );
	return;
	M_EPILOG
	}

bool OSetup::test_glx_emperors( int a_iEmperors, char*& a_rpcMessage )
	{
	return ( ( a_iEmperors < 2 )
			&& ( a_rpcMessage = _( "galaxy is multiplayer game and makes sense"
					" only for at least two players\n" ) ) );
	}

bool OSetup::test_glx_emperors_systems( int a_iEmperors, int a_iSystems, char*& a_rpcMessage )
	{
	return ( ( ( a_iEmperors + a_iSystems ) > MAX_SYSTEM_COUNT )
			&& ( a_rpcMessage = _( "bad total system count\n" ) ) );
	}

bool OSetup::test_glx_systems( int a_iSystems, char*& a_rpcMessage )
	{
	return ( ( a_iSystems < 0 )
			&& ( a_rpcMessage = _( "neutral system count has to be nonnegative number\n" ) ) );
	}

bool OSetup::test_glx_board_size( int a_iBoardSize, char*& a_rpcMessage )
	{
	return ( ( ( a_iBoardSize < 6 ) || ( a_iBoardSize > MAX_BOARD_SIZE ) )
			&& ( a_rpcMessage = _( "bad board size specified\n" ) ) );
	}

}

