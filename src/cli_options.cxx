/*
---             `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	cli_options.cxx - this file is integral part of `galaxy' project.

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
  
/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */

#define D_SERVER_SHORT			"G"
#define D_SERVER_LONG				"server"
#define D_CLIENT_SHORT			"C"
#define D_CLIENT_LONG				"client"
#define D_NEW_GAME_SHORT		"N"
#define D_NEW_GAME_LONG			"new"
#define D_JOIN_GAME_SHORT		"J"
#define D_JOIN_GAME_LONG		"join"
#define D_EMPERORS_SHORT		"E"
#define D_EMPERORS_LONG			"emperors"
#define D_PORT_SHORT				"P"
#define D_PORT_LONG					"port"
#define	D_HOST_SHORT				"H"
#define D_HOST_LONG					"host"
#define D_LOGIN_SHORT				"L"
#define D_LOGIN_LONG				"login"
#define D_SYSTEMS_SHORT			"S"
#define D_SYSTEMS_LONG			"systems"
#define D_BOARD_SIZE_SHORT	"B"
#define D_BOARD_SIZE_LONG		"board"
#define D_QUIET_SHORT				"q"
#define D_QUIET_LONG				"quiet"
#define D_SILENT_LONG				"silent"
#define D_VERBOSE_SHORT			"v"
#define D_VERBOSE_LONG			"verbose"
#define D_HELP_SHORT				"h"
#define D_HELP_LONG					"help"
#define D_VERSION_SHORT			"V"
#define D_VERSION_LONG			"version"

void usage ( void ) __attribute__ ( ( __noreturn__ ) );
void usage ( void )
	{
	printf ( "%s - "
"game about conquering the universum.\n", setup.f_pcProgramName );
	printf ( "Usage: %s [OPTION]... [FILE]...\n", setup.f_pcProgramName );
	printf (
"Options:\n"
"  -"D_SERVER_SHORT			", --"D_SERVER_LONG			""   "               setup a game server\n"
"  -"D_CLIENT_SHORT			", --"D_CLIENT_LONG			""   "               connect to server\n"
"  -"D_NEW_GAME_SHORT		", --"D_NEW_GAME_LONG		""" {type},{name}    create game of type {type} and named {name}\n"
"  -"D_JOIN_GAME_SHORT	", --"D_JOIN_GAME_LONG	"" " {name}          join to game named {name}\n"
"  -"D_PORT_SHORT				", --"D_PORT_LONG				"" " {number}        set port number\n"
"  -"D_HELP_SHORT				", --"D_HELP_LONG				"" " {name}          select host to connect\n"
"  -"D_LOGIN_SHORT			", --"D_LOGIN_LONG			""  " {name}         set Your player name\n"
"  -"D_EMPERORS_SHORT		", --"D_EMPERORS_LONG		""     " {count}     set number of players\n"
"  -"D_SYSTEMS_SHORT		", --"D_SYSTEMS_LONG		""    " {count}      set number of neutral systems\n"
"  -"D_BOARD_SIZE_SHORT	", --"D_BOARD_SIZE_LONG	""  " {size}         size of the galaxy board\n"
"  -"D_QUIET_SHORT			", --"D_QUIET_LONG", --"D_SILENT_LONG"      inhibit usual output\n"
"  -"D_VERBOSE_SHORT		", --"D_VERBOSE_LONG		""    "              print more information\n"
"  -"D_HELP_SHORT				", --"D_HELP_LONG				"" "                 display this help and exit\n"
"  -"D_VERSION_SHORT		", --"D_VERSION_LONG		""    "              output version information and exit\n" );
	throw ( setup.f_bHelp ? 0 : 1 );
	}

void version( void ) __attribute__ ( ( __noreturn__ ) );
void version( void )
	{
	printf ( "`galaxy' %s\n", VER );
	throw ( 0 );
	}

void set_verbosity( void )
	{
	++ setup.f_iVerbose;
	return;
	}

int decode_switches ( int a_iArgc, char ** a_ppcArgv )
	{
	M_PROLOG
	int l_iUnknown = 0, l_iNonOption = 0;
	OOption l_psOptions[] =
		{
			{ D_SERVER_LONG,			D_SERVER_SHORT,			OOption::D_NONE, D_BOOL, &setup.f_bServer,	NULL },
			{ D_CLIENT_LONG,			D_CLIENT_SHORT,			OOption::D_NONE, D_BOOL, &setup.f_bClient,	NULL },
			{ D_NEW_GAME_LONG,		D_NEW_GAME_SHORT,		OOption::D_REQUIRED, D_HSTRING, &setup.f_oGameType,	NULL },
			{ D_JOIN_GAME_LONG,		D_JOIN_GAME_SHORT,	OOption::D_REQUIRED, D_HSTRING, &setup.f_oGame,	NULL },
			{ D_EMPERORS_LONG,		D_EMPERORS_SHORT,		OOption::D_REQUIRED, D_INT, &setup.f_iEmperors,	NULL },
			{ D_PORT_LONG,				D_PORT_SHORT,				OOption::D_REQUIRED, D_INT, &setup.f_iPort,	NULL },
			{ D_LOGIN_LONG,	 			D_LOGIN_SHORT,			OOption::D_REQUIRED, D_HSTRING,	&setup.f_oLogin, NULL },
			{ D_SYSTEMS_LONG,			D_SYSTEMS_SHORT,		OOption::D_REQUIRED, D_INT,			&setup.f_iSystems, NULL },
			{ D_BOARD_SIZE_LONG,	D_BOARD_SIZE_SHORT,	OOption::D_REQUIRED, D_INT,			&setup.f_iBoardSize, NULL },
			{ D_HOST_LONG,				D_HOST_SHORT,				OOption::D_REQUIRED, D_HSTRING,	&setup.f_oHost, NULL },
			{ D_QUIET_LONG	,			D_QUIET_SHORT,			OOption::D_NONE,	D_BOOL,	&setup.f_bQuiet,		NULL },
			{ D_SILENT_LONG,			D_QUIET_SHORT,			OOption::D_NONE,	D_BOOL,	&setup.f_bQuiet,		NULL },
			{ D_VERBOSE_LONG,			D_VERBOSE_SHORT,		OOption::D_NONE,	D_VOID,	NULL,	set_verbosity },
			{ D_HELP_LONG,				D_HELP_SHORT,				OOption::D_NONE,	D_BOOL,	&setup.f_bHelp,		usage },
			{ D_VERSION_LONG,			D_VERSION_SHORT,		OOption::D_NONE,	D_VOID,	NULL,								version }
		};
	l_iNonOption = cl_switch::decode_switches ( a_iArgc, a_ppcArgv, l_psOptions,
			sizeof ( l_psOptions ) / sizeof ( OOption ), & l_iUnknown );
	if ( l_iUnknown > 0 )
		usage ( );
	return ( l_iNonOption );
	M_EPILOG
	}

