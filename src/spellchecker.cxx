/* $Id$ */

/*
 *  (C) Copyright 2002-2003 Wojtek Kaniewski <wojtekka@irc.pl>
 *                          Wojtek Bojdo³ <wojboj@htcon.pl>
 *                          Pawe³ Maziarz <drg@infomex.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h>
#include <aspell.h>

#include <yaal/yaal.h>

#include "spellchecker.h"

#include "setup.h"

using namespace yaal;
using namespace yaal::hcore;

/*! \brief Simple constructor.
 * 
 * it inializes dictionary
 */
HSpellChecker::HSpellChecker( void ) : f_pxSpellChecker( NULL ), f_pxSpellConfig( NULL )
	{
	M_PROLOG
	AspellCanHaveError* possible_err = NULL;

	if ( setup.f_oConsoleCharset.is_empty() || setup.f_oAspellLang.is_empty() )
		{
		/* jesli nie chcemy aspella to wywalamy go z pamieci */
		cleanup();
		M_THROW( "Maybe console_charset, aspell_lang variable is not set!\n", 0 );
		}

	hcore::log( LOG_TYPE::D_INFO ) << "aspell_init - ";

	f_pxSpellConfig = new_aspell_config();
	aspell_config_replace( static_cast<AspellConfig*>( f_pxSpellConfig ), "encoding", setup.f_oConsoleCharset );
	aspell_config_replace( static_cast<AspellConfig*>( f_pxSpellConfig ), "lang", setup.f_oAspellLang );
	possible_err = new_aspell_speller( static_cast<AspellConfig*>( f_pxSpellConfig ) );

	int err = 0;
	if ( ( err = aspell_error_number( possible_err ) ) != 0 )
		{
		HString msg = "Aspell error: "; 
		msg += aspell_error_message( possible_err );
		cleanup();
		hcore::log( LOG_TYPE::D_INFO ) << "failure" << endl;
		M_THROW( msg, err );
		}
	else
		{
		f_pxSpellChecker = to_aspell_speller( possible_err );
		hcore::log( LOG_TYPE::D_INFO ) << "success" << endl;
		}
	return;
	M_EPILOG
	}

HSpellChecker::~HSpellChecker( void )
	{
	cleanup();
	}

/*! \brief spell_check - checks spelling of a word
 *
 * it checks if the given word is correct
 */
bool HSpellChecker::spell_check( char const* const what )
	{
	M_PROLOG
	M_ASSERT( what != NULL );
	return ( aspell_speller_check( static_cast<AspellSpeller*>( f_pxSpellChecker ), what, ::strlen( what ) ) == 0 );
	M_EPILOG
	}

void HSpellChecker::cleanup( void )
	{
	if ( f_pxSpellChecker )
		delete_aspell_speller( static_cast<AspellSpeller*>( f_pxSpellChecker ) );
	f_pxSpellChecker = NULL;
	if ( f_pxSpellConfig )
		delete_aspell_config( static_cast<AspellConfig*>( f_pxSpellConfig ) );
	f_pxSpellConfig = NULL;
	return;
	}

