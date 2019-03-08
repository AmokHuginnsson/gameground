#include "language.hxx"

namespace gameground {

char const* lang_id_to_str( LANGUAGE langId_ ) {
	char const* langStr( nullptr );
	switch ( langId_ ) {
		case ( LANGUAGE::ENGLISH ) : langStr = "en"; break;
		case ( LANGUAGE::POLISH ) : langStr = "pl"; break;
	}
	return ( langStr );
}

}

