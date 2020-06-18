/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>

#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hlog.hxx>
#include <yaal/tools/hthreadpool.hxx>
#include <yaal/tools/util.hxx>
#include <yaal/hconsole/console.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "setup.hxx"
#include "options.hxx"
#include "galaxy.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace gameground;

namespace gameground {

OSetup setup;

}

int main( int argc_, char* argv_[] ) {
	HScopeExitCall secTP( call( &HThreadPool::stop, &HThreadPool::get_instance() ) );
	HScopeExitCall sec( call( &HSignalService::stop, &HSignalService::get_instance() ) );
	M_PROLOG
/* variables declarations for main loop: */
	int opt = 0;
	HConsole& cons = HConsole::get_instance();
/* end. */
	try {
/* TO-DO: enter main loop code here */
		HSignalService::get_instance();
		setup._programName = argv_[ 0 ];
		opt = handle_program_options( argc_, argv_ );
		setup._logPath.replace( ".log", "_" + setup._login + ".log" );
		hcore::log.rehash( setup._logPath, setup._programName );
		setup.test_setup();
/* *BOOM* */
		if ( ! cons.is_enabled() )
			cons.enter_curses (); /* enabling ncurses ablilities */
		if ( ( cons.get_height() >= 25 ) && ( cons.get_width() >= 80 ) )
			opt = main_client();
		else
			opt = 1;
		if ( cons.is_enabled() )
			cons.leave_curses (); /* ending ncurses sesion */
		if ( opt )
			cout << "Your terminal is too small." << endl;
		cerr << _( "Done" ) << endl;
/* ... there is the place main loop ends. :OD-OT */
	} catch ( int err ) {
		if ( cons.is_enabled ( ) )
			cons.leave_curses (); /* ending ncurses sesion */
		opt = err;
	} catch ( ... ) {
		if ( cons.is_enabled ( ) )
			cons.leave_curses (); /* ending ncurses sesion */
		throw;
	}
	return ( opt );
	M_FINAL
}

