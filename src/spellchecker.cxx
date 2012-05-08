#include <string.h>
#include <aspell.h>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )

#include "spellchecker.hxx"

#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace gameground {

/*! \brief Simple constructor.
 *
 * it inializes dictionary
 */
HSpellChecker::HSpellChecker( void ) : _spellChecker( NULL ), _spellConfig( NULL ) {
	M_PROLOG
	AspellCanHaveError* possible_err = NULL;

	if ( setup._consoleCharset.is_empty() || setup._aspellLang.is_empty() ) {
		/* jesli nie chcemy aspella to wywalamy go z pamieci */
		cleanup();
		M_THROW( "Maybe console_charset, aspell_lang variable is not set!\n", 0 );
	}

	hcore::log( LOG_TYPE::INFO ) << "aspell_init - ";

	_spellConfig = new_aspell_config();
	::aspell_config_replace( static_cast<AspellConfig*>( _spellConfig ), "encoding", setup._consoleCharset.raw() );
	::aspell_config_replace( static_cast<AspellConfig*>( _spellConfig ), "lang", setup._aspellLang.raw() );
	possible_err = new_aspell_speller( static_cast<AspellConfig*>( _spellConfig ) );

	int err = 0;
	if ( ( err = aspell_error_number( possible_err ) ) != 0 ) {
		HString msg = "Aspell error: ";
		msg += aspell_error_message( possible_err );
		cleanup();
		hcore::log( LOG_TYPE::INFO ) << "failure" << endl;
		M_THROW( msg, err );
	} else {
		_spellChecker = to_aspell_speller( possible_err );
		hcore::log( LOG_TYPE::INFO ) << "success" << endl;
	}
	return;
	M_EPILOG
}

HSpellChecker::~HSpellChecker( void ) {
	cleanup();
}

/*! \brief spell_check - checks spelling of a word
 *
 * it checks if the given word is correct
 */
bool HSpellChecker::spell_check( HString const& what ) {
	M_PROLOG
	M_ASSERT( !! what );
	return ( aspell_speller_check( static_cast<AspellSpeller*>( _spellChecker ), what.raw(), static_cast<int>( what.length() ) ) == 0 );
	M_EPILOG
}

void HSpellChecker::cleanup( void ) {
	if ( _spellChecker )
		delete_aspell_speller( static_cast<AspellSpeller*>( _spellChecker ) );
	_spellChecker = NULL;
	if ( _spellConfig )
		delete_aspell_config( static_cast<AspellConfig*>( _spellConfig ) );
	_spellConfig = NULL;
	return;
}

}

