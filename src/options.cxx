/* Read gameground/LICENSE.md file for copyright and licensing information. */

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
	cout << PACKAGE_STRING << ( setup._verbose ? " " COMMIT_ID : "" ) << endl;
	return;
}

}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
int handle_program_options( int argc_, char** argv_ ) {
	M_PROLOG
	HProgramOptionsHandler po;
	HOptionInfo info( po );
	info.name( setup._programName ).intro( "GameGround - universal networked multiplayer game server." );
	bool help( false );
	bool conf( false );
	bool vers( false );
	po(
		HProgramOptionsHandler::HOption()
		.long_form( "log-path" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "path pointing to file for application logs" )
		.recipient(	setup._logPath )
		.argument_name( "path" )
		.default_value( "gameground.log" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'D' )
		.long_form( "database-path" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "database connection path scheme" )
		.recipient( setup._databasePath )
		.argument_name( "path" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'U' )
		.long_form( "database-login" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "datbase connection user name" )
		.recipient( setup._databaseLogin )
		.argument_name( "user" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'p' )
		.long_form( "database-password" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "database connection password" )
		.recipient( setup._databasePassword )
		.argument_name( "password" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "aspell-lang" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "language used for spell checking" )
		.recipient( setup._aspellLang )
		.argument_name( "language" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "scoring-system" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "scoring system for boggle (original|fibonacci|geometric|longestwords)" )
		.recipient( setup._scoringSystem )
		.argument_name( "system" )
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
		.recipient( help )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'W' )
		.long_form( "dump-configuration" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "dump current configuration" )
		.recipient( conf )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'V' )
		.long_form( "version" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "output version information and stop" )
		.recipient( vers )
	);
	po.process_rc_file( "", set_variables );
	int unknown( 0 );
	int nonOption( po.process_command_line( argc_, argv_, &unknown ) );
	if ( help || conf || vers || ( unknown > 0 ) ) {
		if ( help || ( unknown > 0 ) ) {
			util::show_help( info );
		} else if ( conf ) {
			util::dump_configuration( info );
		} else if ( vers ) {
			version();
		}
		HLog::disable_auto_rehash();
		throw unknown;
	}
	return nonOption;
	M_EPILOG
}

}

