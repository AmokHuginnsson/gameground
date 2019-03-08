/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GAMEGROUND_LANGUAGE_HXX_INCLUDED
#define GAMEGROUND_LANGUAGE_HXX_INCLUDED

namespace gameground {

enum class LANGUAGE {
	ENGLISH = 0,
	POLISH = 1
};
char const* lang_id_to_str( LANGUAGE );

}

#endif /* not GAMEGROUND_LANGUAGE_HXX_INCLUDED */

