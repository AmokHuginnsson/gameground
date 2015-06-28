/*
---       `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

  options.cxx - this file is integral part of `gameground' project.

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

#include "config.hxx"

#include <yaal/hcore/hprogramoptionshandler.hxx>
#include <yaal/hcore/hlog.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )

#include "options.hxx"
#include "setup.hxx"
#include "commit_id.hxx"

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

void version( void ) {
	cout << PACKAGE_STRING << ( setup._verbose ? "-" COMMIT_ID : "" ) << endl;
	return;
}

}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
int handle_program_options( int argc_, char** argv_ ) {
	M_PROLOG
	HProgramOptionsHandler po;
	OOptionInfo info( po, setup._programName, "GameGround - universal networked multiplayer game server.", NULL );
	bool stop = false;
	po(
		HProgramOptionsHandler::HOption()
		.long_form( "log_path" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "path pointing to file for application logs" )
		.recipient(	setup._logPath )
		.argument_name( "path" )
		.default_value( "gameground.log" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'D' )
		.long_form( "database_path" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "database connection path scheme" )
		.recipient( setup._databasePath )
		.argument_name( "path" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'U' )
		.long_form( "database_login" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "datbase connection user name" )
		.recipient( setup._databaseLogin )
		.argument_name( "user" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'p' )
		.long_form( "database_password" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "database connection password" )
		.recipient( setup._databasePassword )
		.argument_name( "password" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "aspell_lang" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "language used for spell checking" )
		.recipient( setup._aspellLang )
		.argument_name( "language" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "scoring_system" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "scoring system for boggle (original|fibonacci|geometric|longestwords)" )
		.recipient( setup._scoringSystem )
		.argument_name( "system" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "console_charset" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "charset encoding for current terminal" )
		.recipient( setup._consoleCharset )
		.argument_name( "charset" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'P' )
		.long_form( "port" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set port number" )
		.recipient( setup._port )
		.argument_name( "num" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'B' )
		.long_form( "board" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "size of the galaxy board" )
		.recipient( setup._boardSize )
		.argument_name( "size" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'E' )
		.long_form( "emperors" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set number of players" )
		.recipient( setup._emperors )
		.argument_name( "count" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'S' )
		.long_form( "systems" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set number of neutral systems" )
		.recipient( setup._systems )
		.argument_name( "count" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'q' )
		.long_form( "quiet" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "inhibit usual output" )
		.recipient( setup._quiet )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'q' )
		.long_form( "silent" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "inhibit usual output" )
		.recipient( setup._quiet )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'v' )
		.long_form( "verbose" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "print more information" )
		.recipient( setup._verbose )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'h' )
		.long_form( "help" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "display this help and stop" )
		.recipient( stop )
		.callback( call( &util::show_help, &info ) )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'W' )
		.long_form( "dump-configuration" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "dump current configuration" )
		.recipient( stop )
		.callback( call( &util::dump_configuration, &info ) )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'V' )
		.long_form( "version" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "output version information and stop" )
		.recipient( stop )
		.callback( call( &version ) )
	);
	po.process_rc_file( "gameground", "", set_variables );
	int unknown( 0 );
	int nonOption( po.process_command_line( argc_, argv_, &unknown ) );
	if ( stop || ( unknown > 0 ) ) {
		if ( unknown > 0 )
			util::show_help( &info );
		HLog::disable_auto_rehash();
		throw unknown;
	}
	return ( nonOption );
	M_EPILOG
}

}

