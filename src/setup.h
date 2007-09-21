/*
---            `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	setup.h - this file is integral part of `galaxy' project.

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

#define D_BOARD_SIZE 								16
#define D_DEFAULT_MAX_CONNECTIONS		64
#define D_DEFAULT_PLAYERS						4
#define D_NEUTRAL_SYSTEM_PER_PLAYER	4
#define D_MAX_BOARD_SIZE						20
#define D_MAX_SYSTEM_COUNT					36
#define D_PLAYERS 									4
#define D_ROUND_TIME								180
#define D_MAX_ROUNDS								5

struct OSetup
	{
	bool f_bQuiet;			/* --quiet, --silent */
	bool f_bVerbose;		/* --verbose */
	bool f_bHelp;
	/* galaxy specific bools */
	bool f_bServer;
	bool f_bClient;
	int	f_iMaxConnections;
	/* galaxy specific integers */
	int f_iEmperors;
	int f_iPort;
	int f_iSystems;
	int f_iBoardSize;
	int f_iPlayers;
	int f_iRoundTime;
	int f_iMaxRounds;
	/* galaxy specific strings */
	yaal::hcore::HString f_oLogin;
	yaal::hcore::HString f_oHost;
	char * f_pcProgramName;
	yaal::hcore::HString f_oLogPath;
	/* self-sufficient */
	OSetup ( void ) : f_bQuiet ( false ), f_bVerbose ( false ),
										f_bHelp ( false ), f_bServer ( false ),
										f_bClient ( false ), f_iMaxConnections( D_DEFAULT_MAX_CONNECTIONS ),
										f_iEmperors ( D_DEFAULT_PLAYERS ),
										f_iPort ( 7777 ),
										f_iSystems ( D_DEFAULT_PLAYERS * D_NEUTRAL_SYSTEM_PER_PLAYER ),
										f_iBoardSize ( D_BOARD_SIZE ), f_iPlayers( D_PLAYERS ),
										f_iRoundTime( D_ROUND_TIME ), f_iMaxRounds( D_MAX_ROUNDS ),
										f_oLogin ( ), f_oHost ( ), f_pcProgramName ( NULL ),
										f_oLogPath ( ) {}
	void test_setup ( void )
		{
		M_PROLOG
		if ( f_bServer && f_bClient )
			yaal::tools::util::failure ( 1,
					_( "galaxy cannot be server and client at the same time\n" ) );
		if ( ! ( f_bServer || f_bClient ) )
			yaal::tools::util::failure ( 2,
					_( "galaxy must be server or client\n" ) );
		if ( f_bServer && ( f_iMaxConnections < 2 ) )
			yaal::tools::util::failure ( 3,
					_( "this server hosts multiplayer games only\n" ) );
		char* l_pcMessage = NULL;
		if ( test_glx_emperors( f_iEmperors, l_pcMessage ) )
			yaal::tools::util::failure ( 4, l_pcMessage );
		if ( f_iPort < 1024 )
			yaal::tools::util::failure ( 5,
					_( "galaxy cannot run on restricted ports\n" ) );
		if ( f_bClient && f_oHost.is_empty ( ) )
			yaal::tools::util::failure ( 6,
					_( "as a client You must specify server host\n" ) );
		if ( f_bClient && f_oLogin.is_empty ( ) )
			yaal::tools::util::failure ( 7,
					_( "as a player You must specify Your name\n" ) );
		if ( test_glx_board_size( f_iBoardSize, l_pcMessage ) )
			yaal::tools::util::failure ( 8, l_pcMessage );
		if ( test_glx_emperors_systems( f_iEmperors, f_iSystems, l_pcMessage ) )
			yaal::tools::util::failure ( 9, l_pcMessage );
		if ( test_glx_systems( f_iSystems, l_pcMessage ) )
			yaal::tools::util::failure ( 10, l_pcMessage );
		return;
		M_EPILOG
		}
	static bool test_glx_emperors( int a_iEmperors, char*& a_rpcMessage )
		{
		return ( ( a_iEmperors < 2 )
				&& ( a_rpcMessage = _( "galaxy is multiplayer game and makes sense"
						" only for at least two players\n" ) ) );
		}
	static bool test_glx_emperors_systems( int a_iEmperors, int a_iSystems, char*& a_rpcMessage )
		{
		return ( ( ( a_iEmperors + a_iSystems ) > D_MAX_SYSTEM_COUNT )
				&& ( a_rpcMessage = _( "bad total system count\n" ) ) );
		}
	static bool test_glx_systems( int a_iSystems, char*& a_rpcMessage )
		{
		return ( ( a_iSystems < 0 )
				&& ( a_rpcMessage = _( "neutral system count has to be nonnegative number\n" ) ) );
		}
	static bool test_glx_board_size( int a_iBoardSize, char*& a_rpcMessage )
		{
		return ( ( ( a_iBoardSize < 6 ) || ( a_iBoardSize > D_MAX_BOARD_SIZE ) )
				&& ( a_rpcMessage = _( "bad board size specified\n" ) ) );
		}
private:
	OSetup ( OSetup const & );
	OSetup & operator = ( OSetup const & );
	};

extern OSetup setup;

#endif /* __SETUP_H */
