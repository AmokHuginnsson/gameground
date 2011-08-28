/*
---       `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

	rc_options.cxx - this file is integral part of `gameground' project.

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

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )

#include "options.hxx"
#include "config.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground
{

bool set_variables( HString& option_, HString& value_ )
	{
	::fprintf( stdout, "option: [%s], value: [%s]\n",
			option_.raw(), value_.raw() );
	return ( false );
	}

void version( void* ) __attribute__ ( ( __noreturn__ ) );
void version( void* )
	{
	cout << PACKAGE_STRING << endl;
	throw ( 0 );
	}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
int handle_program_options( int argc_, char** argv_ )
	{
	M_PROLOG
	HProgramOptionsHandler po;
	OOptionInfo info( po, setup._programName, "GameGround - universal networked multiplayer game server.", NULL );
	bool stop = false;
	po( "log_path", program_options_helper::option_value( setup._logPath ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path pointing to file for application logs", "path" )
		( "database_path", program_options_helper::option_value( setup._databasePath ), 'D', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "database connection path scheme", "path" )
		( "database_login", program_options_helper::option_value( setup._databaseLogin ), 'U', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "datbase connection user name", "user" )
		( "database_password", program_options_helper::option_value( setup._databasePassword ), 'p', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "database connection password", "password" )
		( "aspell_lang", program_options_helper::option_value( setup._aspellLang ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "language used for spell checking", "language" )
		( "scoring_system", program_options_helper::option_value( setup._scoringSystem ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "scoring system for boggle (original|fibonacci|geometric|longestwords)", "system" )
		( "console_charset", program_options_helper::option_value( setup._consoleCharset ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "charset encoding for current terminal", "charset" )
		( "port", program_options_helper::option_value( setup._port ), "P", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set port number", "num" )
		( "board", program_options_helper::option_value( setup._boardSize ), "B", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "size of the galaxy board", "size" )
		( "emperors", program_options_helper::option_value( setup._emperors ), "E", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set number of players", "count" )
		( "systems", program_options_helper::option_value( setup._systems ), "S", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set number of neutral systems", "count" )
		( "quiet", program_options_helper::option_value( setup._quiet ), "q", HProgramOptionsHandler::OOption::TYPE::NONE, "inhibit usual output" )
		( "silent", program_options_helper::option_value( setup._quiet ), "q", HProgramOptionsHandler::OOption::TYPE::NONE, "inhibit usual output" )
		( "verbose", program_options_helper::option_value( setup._verbose ), "v", HProgramOptionsHandler::OOption::TYPE::NONE, "print more information" )
		( "debug", program_options_helper::option_value( setup._debug ), "d", HProgramOptionsHandler::OOption::TYPE::NONE, "run in debug mode" )
		( "help", program_options_helper::option_value( stop ), "h", HProgramOptionsHandler::OOption::TYPE::NONE, "display this help and stop", program_options_helper::callback( util::show_help, &info ) )
		( "dump-configuration", program_options_helper::option_value( stop ), "W", HProgramOptionsHandler::OOption::TYPE::NONE, "dump current configuration", program_options_helper::callback( util::dump_configuration, &info ) )
		( "version", program_options_helper::no_value, "V", HProgramOptionsHandler::OOption::TYPE::NONE, "output version information and stop", program_options_helper::callback( version, NULL ) );
	po.process_rc_file( "gameground", "", NULL );
	if ( setup._logPath.is_empty() )
		setup._logPath = "gameground.log";
	int unknown = 0, nonOption = 0;
	nonOption = po.process_command_line( argc_, argv_, &unknown );
	if ( unknown > 0 )
		{
		util::show_help( &info );
		throw unknown;
		}
	if ( stop )
		throw 0;
	return ( nonOption );
	M_EPILOG
	}

}

