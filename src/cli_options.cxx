/*
---             `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	cli_options.cxx - this file is integral part of `gameground' project.

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

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )

#include "cli_options.h"
#include "version.h"
#include "setup.h"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace gameground
{
  
/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */

typedef HPair<OOption*,int> option_info_t;

void usage( void* ) __attribute__((__noreturn__));
void usage( void* arg )
	{
	option_info_t* info = static_cast<option_info_t*>( arg );
	util::show_help( info->first, info->second, setup.f_pcProgramName, "GameGround - universal networked multiplayer game server" );
	throw ( setup.f_bHelp ? 0 : 1 );
	}

void version( void* ) __attribute__ ( ( __noreturn__ ) );
void version( void* )
	{
	printf ( "`gameground' %s\n", VER );
	throw ( 0 );
	}

void set_verbosity( void* )
	{
	++ setup.f_iVerbose;
	return;
	}

int decode_switches ( int a_iArgc, char ** a_ppcArgv )
	{
	M_PROLOG
	int l_iUnknown = 0, l_iNonOption = 0;
	simple_callback_t v( set_verbosity, NULL );
	simple_callback_t help( usage, NULL );
	simple_callback_t version_call( version, NULL );
	OOption l_psOptions[] =
		{
			{ "server", D_BOOL, &setup.f_bServer, "G", OOption::D_NONE, NULL, "setup a game server", NULL },
			{ "client", D_BOOL, &setup.f_bClient, "C", OOption::D_NONE, NULL, "connect to server", NULL },
			{ "new", D_HSTRING, &setup.f_oGameType, "N", OOption::D_REQUIRED, "type,name", "create game of type {type} and named {name}", NULL },
			{ "join", D_HSTRING, &setup.f_oGame, "J", OOption::D_REQUIRED, "name", "join to game named {name}", NULL },
			{ "emperors", D_INT, &setup.f_iEmperors, "E", OOption::D_REQUIRED, "count", "set number of players", NULL },
			{ "port", D_INT, &setup.f_iPort, "P", OOption::D_REQUIRED, "num", "set port number", NULL },
			{ "login", D_HSTRING, &setup.f_oLogin, "L", OOption::D_REQUIRED, "nane", "set your player name", NULL },
			{ "systems", D_INT, &setup.f_iSystems, "S", OOption::D_REQUIRED, "count", "set number of neutral systems", NULL },
			{ "board", D_INT, &setup.f_iBoardSize, "B", OOption::D_REQUIRED, "size", "size of the galaxy board", NULL },
			{ "host", D_HSTRING, &setup.f_oHost, "H", OOption::D_REQUIRED, "addr", "select host to connect", NULL },
			{ "quiet", D_BOOL, &setup.f_bQuiet, "q", OOption::D_NONE, NULL, "inhibit usual output", NULL },
			{ "silent", D_BOOL, &setup.f_bQuiet, "q", OOption::D_NONE, NULL, "inhibit usual output", NULL },
			{ "verbose", D_VOID, &setup.f_iVerbose, "v", OOption::D_NONE, NULL, "print more information", &v },
			{ "help", D_BOOL, &setup.f_bHelp, "h", OOption::D_NONE, NULL, "display this help and exit", &help },
			{ "version", D_VOID, NULL, "V", OOption::D_NONE, NULL, "output version information and exit", &version_call }
		};
	option_info_t info( l_psOptions, sizeof ( l_psOptions ) / sizeof ( OOption ) );
	help.second = &info;
	l_iNonOption = cl_switch::decode_switches( a_iArgc, a_ppcArgv, l_psOptions,
			info.second, &l_iUnknown );
	if ( l_iUnknown > 0 )
		usage( &info );
	return ( l_iNonOption );
	M_EPILOG
	}

}

