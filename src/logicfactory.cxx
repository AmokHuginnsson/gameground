/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include <yaal/hcore/macro.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "logicfactory.hxx"

#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace gameground {

void HLogicFactory::register_logic_creator( HString const& name_, HLogicCreatorInterface* instatiator_ ) {
	M_PROLOG
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() )
		M_THROW( _( "Logic already registered" ), errno );
	OCreator creator;
	creator._instantiator = instatiator_;
	_creators[ name_ ] = creator;
	return;
	M_EPILOG
}

HLogic::ptr_t HLogicFactory::create_logic( HString const& type_, HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	HLogic::ptr_t logic;
	creators_t::iterator it = _creators.find( type_ );
	if ( it != _creators.end() )
		logic = it->second._instantiator->new_instance( server_, id_, argv_ );
	return ( logic );
	M_EPILOG
}

bool HLogicFactory::is_type_valid( yaal::hcore::HString const& type_ ) {
	M_PROLOG
	bool valid = false;
	creators_t::iterator it = _creators.find( type_ );
	if ( it != _creators.end() )
		valid = true;
	return ( valid );
	M_EPILOG
}

HLogicFactory::creators_t::iterator HLogicFactory::begin( void ) {
	return ( _creators.begin() );
}

HLogicFactory::creators_t::iterator HLogicFactory::end( void ) {
	return ( _creators.end() );
}

int HLogicFactory::life_time( int lifeTime_ ) {
	return ( lifeTime_ );
}

void HLogicFactory::initialize_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(); it != _creators.end(); ++ it )
		it->second._instantiator->initialize_globals();
	return;
	M_EPILOG
}

void HLogicFactory::cleanup_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(); it != _creators.end(); ++ it )
		it->second._instantiator->cleanup_globals();
	return;
	M_EPILOG
}

}

