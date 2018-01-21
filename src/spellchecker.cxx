/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <string.h>
#include <aspell.h>

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hlog.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "spellchecker.hxx"

#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace gameground {

inline char const* lang_id_to_str( HSpellChecker::LANGUAGE langId_ ) {
	char const* langStr( nullptr );
	switch ( langId_ ) {
		case ( HSpellChecker::LANGUAGE::ENGLISH ) : langStr = "en"; break;
		case ( HSpellChecker::LANGUAGE::POLISH ) : langStr = "pl"; break;
	}
	return ( langStr );
}

/*! \brief Simple constructor.
 *
 * it inializes dictionary
 */
HSpellChecker::HSpellChecker( LANGUAGE language_ )
	: _spellChecker( nullptr )
	, _spellConfig( nullptr )
	, _utf8() {
	M_PROLOG
	AspellCanHaveError* possible_err = nullptr;

	if ( setup._aspellLang.is_empty() ) {
		/* jesli nie chcemy aspella to wywalamy go z pamieci */
		cleanup();
		M_THROW( "Maybe console_charset, aspell_lang variable is not set!\n", 0 );
	}

	hcore::log( LOG_LEVEL::INFO ) << "aspell_init - ";

	_spellConfig = new_aspell_config();
	::aspell_config_replace( static_cast<AspellConfig*>( _spellConfig ), "encoding", "UTF-8" );
	::aspell_config_replace( static_cast<AspellConfig*>( _spellConfig ), "lang", lang_id_to_str( language_ ) );
	possible_err = new_aspell_speller( static_cast<AspellConfig*>( _spellConfig ) );

	int err = 0;
	if ( ( err = static_cast<int>( aspell_error_number( possible_err ) ) ) != 0 ) {
		HString msg = "Aspell error: ";
		msg += aspell_error_message( possible_err );
		cleanup();
		hcore::log( LOG_LEVEL::INFO ) << "failure" << endl;
		M_THROW( msg, err );
	} else {
		_spellChecker = to_aspell_speller( possible_err );
		hcore::log( LOG_LEVEL::INFO ) << "success" << endl;
	}
	return;
	M_EPILOG
}

HSpellChecker::HSpellChecker( HSpellChecker&& other_ )
	: _spellChecker( yaal::move( other_._spellChecker ) )
	, _spellConfig( yaal::move( other_._spellConfig ) )
	, _utf8( yaal::move( other_._utf8 ) ) {
	other_._spellConfig = nullptr;
	other_._spellChecker = nullptr;
}

HSpellChecker::~HSpellChecker( void ) {
	cleanup();
}

/*! \brief spell_check - checks spelling of a word
 *
 * it checks if the given word is correct
 */
bool HSpellChecker::spell_check( HString const& what_ ) {
	M_PROLOG
	M_ASSERT( !! what_ );
	_utf8.assign( what_ );
	return ( aspell_speller_check( static_cast<AspellSpeller*>( _spellChecker ), _utf8.c_str(), static_cast<int>( _utf8.byte_count() ) ) == 0 );
	M_EPILOG
}

void HSpellChecker::cleanup( void ) {
	if ( _spellChecker ) {
		delete_aspell_speller( static_cast<AspellSpeller*>( _spellChecker ) );
	}
	_spellChecker = nullptr;
	if ( _spellConfig ) {
		delete_aspell_config( static_cast<AspellConfig*>( _spellConfig ) );
	}
	_spellConfig = nullptr;
	return;
}

}

