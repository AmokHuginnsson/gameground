/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	security.cxx - this file is integral part of `gameground' project.

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

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "security.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace gameground {

/* Dangerous characters and their mappings:
 * " - \Q
 * ' = \A
 */

yaal::hcore::HString escape( yaal::hcore::HString const& evil_ ) {
	M_PROLOG
	HString str;
	if ( ! evil_.is_empty() ) {
		str.hs_realloc( evil_.get_length() * 2 );
		str = evil_;
		str.replace( "\\", "\\E" ).replace( "\n", "\\n" ).replace( "\"", "\\Q" ).replace( "'", "\\A" ).replace( ":", "\\C" ).replace( ",", "\\K" );
	}
	return ( str );
	M_EPILOG
}

yaal::hcore::HString unescape( yaal::hcore::HString const& safe_ ) {
	M_PROLOG
	HString str( safe_ );
	str.replace( "\\K", "," ).replace( "\\C", ":" ).replace( "\\A", "'" ).replace( "\\Q", "\"" ).replace( "\\n", "\n" ).replace( "\\E", "\\" );
	return ( str );
	M_EPILOG
}

bool is_sha1( yaal::hcore::HString const& str_ ) {
	M_PROLOG
	int len( static_cast<int>( str_.get_length() ) );
	char const SHA1chars[] = "01234567890aAbBcCdDeEfF";
	int idx( static_cast<int>( str_.find_other_than( SHA1chars ) ) );
	return ( ( len == 40 ) && ( idx == HString::npos ) );
	M_EPILOG
}

}

