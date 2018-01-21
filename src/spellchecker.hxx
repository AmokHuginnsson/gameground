/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef SPELLCHECKER_HXX_INCLUDED
#define SPELLCHECKER_HXX_INCLUDED

#include <yaal/hcore/hstring.hxx>

namespace gameground {

class HSpellChecker {
public:
	typedef HSpellChecker this_type;
	enum class LANGUAGE {
		ENGLISH = 0,
		POLISH = 1
	};
private:
	void* _spellChecker; /* AspellSpeller */
	void* _spellConfig; /* AspellConfig */
	yaal::hcore::HUTF8String _utf8;
public:
	HSpellChecker( LANGUAGE );
	HSpellChecker( HSpellChecker&& );
	~HSpellChecker( void );
	bool spell_check( yaal::hcore::HString const& );
private:
	void cleanup( void );
	HSpellChecker( HSpellChecker const& );
	HSpellChecker& operator = ( HSpellChecker const& );
};

}

#endif /* SPELLCHECKER_HXX_INCLUDED */
