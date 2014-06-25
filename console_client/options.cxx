/*
---       `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

	rc_options.cxx - this file is integral part of `gameground' project.

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

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <yaal/hcore/hprogramoptionshandler.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "options.hxx"
#include "config.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground {

namespace {

bool set_variables( HString& option_, HString& value_ ) {
	static bool const GAMEGROUND_RC_DEBUG( !! ::getenv( "GAMEGROUND_RC_DEBUG" ) );
	if ( GAMEGROUND_RC_DEBUG ) {
		cout << "option: [" << option_ << "], value: [" << value_ << "]" << endl;
	}
	return ( true );
}

}

void version( void* ) __attribute__ ( ( __noreturn__ ) );
void version( void* ) {
	cout << PACKAGE_STRING << endl;
	throw ( 0 );
}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
int handle_program_options( int argc_, char** argv_ ) {
	M_PROLOG
	HProgramOptionsHandler po;
	OOptionInfo info( po, setup._programName, "GameGround-client - console client for GameGround - an universal networked multiplayer game server.", NULL );
	bool stop = false;
	po( "log_path", program_options_helper::option_value( setup._logPath ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path pointing to file for application logs", "path" )
		( "port", program_options_helper::option_value( setup._port ), "P", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set port number", "num" )
		( "board", program_options_helper::option_value( setup._boardSize ), "B", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "size of the galaxy board", "size" )
		( "emperors", program_options_helper::option_value( setup._emperors ), "E", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set number of players", "count" )
		( "systems", program_options_helper::option_value( setup._systems ), "S", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set number of neutral systems", "count" )
		( "host", program_options_helper::option_value( setup._host ), "H", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "select host to connect", "addr" )
		( "join", program_options_helper::option_value( setup._game ), "J", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "join to game named {name}", "name" )
		( "login", program_options_helper::option_value( setup._login ), "L", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set your player name", "name" )
		( "password", program_options_helper::option_value( setup._password ), 'p', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "password for your account", "password" )
		( "new", program_options_helper::option_value( setup._gameType ), "N", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "create game of type {type} and named {name}", "type,name" )
		( "quiet", program_options_helper::option_value( setup._quiet ), "q", HProgramOptionsHandler::OOption::TYPE::NONE, "inhibit usual output" )
		( "silent", program_options_helper::option_value( setup._quiet ), "q", HProgramOptionsHandler::OOption::TYPE::NONE, "inhibit usual output" )
		( "verbose", program_options_helper::option_value( setup._verbose ), "v", HProgramOptionsHandler::OOption::TYPE::NONE, "print more information" )
		( "debug", program_options_helper::option_value( setup._debug ), "d", HProgramOptionsHandler::OOption::TYPE::NONE, "run in debug mode" )
		( "help", program_options_helper::option_value( stop ), "h", HProgramOptionsHandler::OOption::TYPE::NONE, "display this help and stop", program_options_helper::callback( util::show_help, &info ) )
		( "dump-configuration", program_options_helper::option_value( stop ), "W", HProgramOptionsHandler::OOption::TYPE::NONE, "dump current configuration", program_options_helper::callback( util::dump_configuration, &info ) )
		( "version", program_options_helper::no_value, "V", HProgramOptionsHandler::OOption::TYPE::NONE, "output version information and stop", program_options_helper::callback( version, NULL ) );
	po.process_rc_file( "gameground-client", "", set_variables );
	if ( setup._logPath.is_empty() )
		setup._logPath = "gameground-client.log";
	int unknown = 0, nonOption = 0;
	nonOption = po.process_command_line( argc_, argv_, &unknown );
	if ( unknown > 0 ) {
		util::show_help( &info );
		throw unknown;
	}
	if ( stop )
		throw 0;
	return ( nonOption );
	M_EPILOG
}

}

