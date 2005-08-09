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

struct OSetup
	{
	OSetup ( void ) : f_bQuiet ( false ), f_bVerbose ( false ),
										f_bHelp ( false ), f_bServer ( false ),
										f_bClient ( false ), f_iPlayers ( 4 ),
										f_iPort ( 7777 ),
										f_pcProgramName ( NULL ),
										f_oLogPath ( ) {}
	void test_setup ( void )
		{
		if ( f_bServer && f_bClient )
			stdhapi::tools::util::failure ( 1, _ ( "galaxy cannot be server and client at the same time\n" ) );
		if ( ! ( f_bServer || f_bClient ) )
			stdhapi::tools::util::failure ( 2, _ ( "galaxy must be server or client\n" ) );
		if ( f_bServer && ( f_iPlayers < 2 ) )
			stdhapi::tools::util::failure ( 3,
					_ ( "galaxy is multiplayer game and makes sense only for at least two players\n" ) );
		if ( f_iPort < 1024 )
			stdhapi::tools::util::failure ( 4, _ ( "galaxy galaxy cannot run on restricted ports\n" ) );
		}
	bool f_bQuiet;			/* --quiet, --silent */
	bool f_bVerbose;		/* --verbose */
	bool f_bHelp;
	/* galaxy specific bools */
	bool f_bServer;
	bool f_bClient;
	/* galaxy specific integers */
	int f_iPlayers;
	int f_iPort;
	char * f_pcProgramName;
	stdhapi::hcore::HString f_oLogPath;
private:
	OSetup ( OSetup const & );
	OSetup & operator = ( OSetup const & );
	};

extern OSetup setup;

#endif /* __SETUP_H */
