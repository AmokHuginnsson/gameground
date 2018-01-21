/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hexception.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "security.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground {

/* Dangerous characters and their mappings:
 * " - \Q
 * ' = \A
 */

char const _rawToSafe_[] = "\\\n\"':,";
char const _safeToRaw_[] = "EnQACK";

EscapeTable _escapeTable_( _rawToSafe_, static_cast<int>( sizeof ( _rawToSafe_ ) - 1 ), _safeToRaw_, static_cast<int>( sizeof ( _safeToRaw_ ) - 1 ) );

bool is_sha1( yaal::hcore::HString const& str_ ) {
	M_PROLOG
	int len( static_cast<int>( str_.get_length() ) );
	char const SHA1chars[] = "01234567890aAbBcCdDeEfF";
	int idx( static_cast<int>( str_.find_other_than( SHA1chars ) ) );
	return ( ( len == 40 ) && ( idx == HString::npos ) );
	M_EPILOG
}

}

