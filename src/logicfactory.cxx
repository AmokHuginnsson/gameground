/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logicfactory.cxx - this file is integral part of `gameground' project.

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

#include <libintl.h>

#include <yaal/yaal.hxx>

M_VCSID( "$Id: "__ID__" $" )
#include "logicfactory.hxx"

#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground
{

void HLogicFactory::register_logic_creator( HString const& info_, HLogicCreatorInterface* instatiator_ )
	{
	M_PROLOG
	HString name = get_token( info_, ":", 0 );
	creators_t::iterator it = _creators.find( name );
	if ( it != _creators.end() )
		M_THROW( _( "Logic already registered" ), errno );
	OCreator creator;
	creator._instatiator = instatiator_;
	creator._info = info_;
	_creators[ name ] = creator;
	return;
	M_EPILOG
	}

HLogic::ptr_t HLogicFactory::create_logic( HString const& type_, HString const& argv_ )
	{
	M_PROLOG
	HLogic::ptr_t logic;
	creators_t::iterator it = _creators.find( type_ );
	if ( it != _creators.end() )
		logic = it->second._instatiator->new_instance( argv_ );
	return ( logic );
	M_EPILOG
	}

bool HLogicFactory::is_type_valid( yaal::hcore::HString const& type_ )
	{
	M_PROLOG
	bool valid = false;
	creators_t::iterator it = _creators.find( type_ );
	if ( it != _creators.end() )
		valid = true;
	return ( valid );
	M_EPILOG
	}

HLogicFactory::creators_t::iterator HLogicFactory::begin( void )
	{
	return ( _creators.begin() );
	}

HLogicFactory::creators_t::iterator HLogicFactory::end( void )
	{
	return ( _creators.end() );
	}

int HLogicFactory::life_time( int lifeTime_ )
	{
	return ( lifeTime_ );
	}

void HLogicFactory::initialize_globals( void )
	{
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(); it != _creators.end(); ++ it )
		it->second._instatiator->initialize_globals();
	return;
	M_EPILOG
	}

}

