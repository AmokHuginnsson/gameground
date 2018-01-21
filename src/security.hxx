/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GAMEGROUND_SECURITY_HXX_INCLUDED
#define GAMEGROUND_SECURITY_HXX_INCLUDED 1

#include <yaal/hcore/hstring.hxx>
#include <yaal/tools/escape.hxx>

namespace gameground {

extern yaal::tools::util::EscapeTable _escapeTable_;
bool is_sha1( yaal::hcore::HString const& );

}

#endif /* #ifndef GAMEGROUND_SECURITY_HXX_INCLUDED */

