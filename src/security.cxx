/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	security.cxx - this file is integral part of `gameground' project.

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

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "security.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace gameground
{

/* Dangerous characters and their mappings:
 * " - \Q
 * ' = \A
 */

yaal::hcore::HString escape( yaal::hcore::HString const& evil_ )
	{
	HString str( evil_.get_length() * 2, true );
	str = evil_;
	str.replace( "\"", "\\Q" ).replace( "'", "\\A" );
	return ( str );
	}

yaal::hcore::HString unescape( yaal::hcore::HString const& safe_ )
	{
	HString str( safe_ );
	str.replace( "\\A", "'" ).replace( "\\Q", "\"" );
	return ( str );
	}

bool is_sha1( yaal::hcore::HString const& str_ )
	{
	int len( static_cast<int>( str_.get_length() ) );
	char const SHA1chars[] = "01234567890aAbBcCdDeEfF";
	int idx( static_cast<int>( str_.find_other_than( SHA1chars ) ) );
	return ( ( len == 40 ) && ( idx == HString::npos ) );
	}

}

