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

#define D_BOARD_SIZE 16
#define D_DEFAULT_PLAYERS						4
#define D_NEUTRAL_SYSTEM_PER_PLAYER	4
#define D_MAX_BOARD_SIZE						20
#define D_MAX_SYSTEM_COUNT					36

struct OSetup
	{
	bool f_bQuiet;			/* --quiet, --silent */
	bool f_bVerbose;		/* --verbose */
	bool f_bHelp;
	/* galaxy specific bools */
	bool f_bServer;
	bool f_bClient;
	/* galaxy specific integers */
	int f_iEmperors;
	int f_iPort;
	int f_iSystems;
	int f_iBoardSize;
	/* galaxy specific strings */
	stdhapi::hcore::HString f_oLogin;
	stdhapi::hcore::HString f_oHost;
	char * f_pcProgramName;
	stdhapi::hcore::HString f_oLogPath;
	/* self-sufficient */
	OSetup ( void ) : f_bQuiet ( false ), f_bVerbose ( false ),
										f_bHelp ( false ), f_bServer ( false ),
										f_bClient ( false ), f_iEmperors ( D_DEFAULT_PLAYERS ),
										f_iPort ( 7777 ),
										f_iSystems ( D_DEFAULT_PLAYERS * D_NEUTRAL_SYSTEM_PER_PLAYER ),
										f_iBoardSize ( D_BOARD_SIZE ),
										f_oLogin ( ), f_oHost ( ), f_pcProgramName ( NULL ),
										f_oLogPath ( ) {}
	void test_setup ( void )
		{
		M_PROLOG
		if ( f_bServer && f_bClient )
			stdhapi::tools::util::failure ( 1,
					_ ( "galaxy cannot be server and client at the same time\n" ) );
		if ( ! ( f_bServer || f_bClient ) )
			stdhapi::tools::util::failure ( 2,
					_ ( "galaxy must be server or client\n" ) );
		if ( f_bServer && ( f_iEmperors < 2 ) )
			stdhapi::tools::util::failure ( 3,
					_ ( "galaxy is multiplayer game and makes sense"
						" only for at least two players\n" ) );
		if ( f_iPort < 1024 )
			stdhapi::tools::util::failure ( 4,
					_ ( "galaxy cannot run on restricted ports\n" ) );
		if ( f_bClient && f_oHost.is_empty ( ) )
			stdhapi::tools::util::failure ( 5,
					_ ( "as a client You must specify server host\n" ) );
		if ( f_bClient && f_oLogin.is_empty ( ) )
			stdhapi::tools::util::failure ( 6,
					_ ( "as a player You must specify Your name\n" ) );
		if ( f_bServer && ( ( f_iBoardSize < 6 ) || ( f_iBoardSize > D_MAX_BOARD_SIZE ) ) )
			stdhapi::tools::util::failure ( 7, _ ( "bad board size specified\n" ) );
		if ( f_bServer && ( ( f_iEmperors + f_iSystems ) > D_MAX_SYSTEM_COUNT ) )
			stdhapi::tools::util::failure ( 8, _ ( "bad total system count\n" ) );
		if ( f_bServer && ( f_iSystems < 0 ) )
			stdhapi::tools::util::failure ( 9, _ ( "neutral system count has to be nonnegative number\n" ) );
		return;
		M_EPILOG
		}
private:
	OSetup ( OSetup const & );
	OSetup & operator = ( OSetup const & );
	};

extern OSetup setup;

#endif /* __SETUP_H */
