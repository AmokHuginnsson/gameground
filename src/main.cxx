/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hlog.hxx>
#include <yaal/hcore/algorithm.hxx>
#include <yaal/tools/hthreadpool.hxx>
#include <yaal/tools/util.hxx>
M_VCSID( "$Id: " __ID__ " $" )

#include "setup.hxx"
#include "options.hxx"
#include "server.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace gameground;

namespace gameground {

OSetup setup;

int main_server( void ) {
	HServer server( setup._maxConnections );
	server.init_server( setup._port );
	server.run();
	return ( 0 );
}

}

int main( int argc_, char* argv_[] ) {
	HScopeExitCall secTP( call( &HThreadPool::stop, &HThreadPool::get_instance() ) );
	HScopeExitCall sec( call( &HSignalService::stop, &HSignalService::get_instance() ) );
	M_PROLOG
/* variables declarations for main loop: */
	int opt = 0;
/* end. */
	try {
	/* TO-DO: enter main loop code here */
		HSignalService::get_instance();
		setup._programName = argv_[ 0 ];
		opt = handle_program_options( argc_, argv_ );
		hcore::log.rehash( setup._logPath, setup._programName );
		setup.test_setup();
	/* *BOOM* */
		main_server();
	/* ... there is the place main loop ends. :OD-OT */
		cerr << _( "Done" ) << endl;
	} catch ( int err ) {
		opt = err;
	}
	return opt;
	M_FINAL
}

