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

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground
{

void HLogicFactory::register_logic_creator( HString const& a_oInfo, HLogicCreatorInterface* a_poInstatiator )
	{
	M_PROLOG
	HString l_oName = get_token( a_oInfo, ":", 0 );
	creators_t::iterator it = f_oCreators.find( l_oName );
	if ( it != f_oCreators.end() )
		M_THROW( _( "Logic already registered" ), errno );
	OCreator l_oCreator;
	l_oCreator.f_poInstatiator = a_poInstatiator;
	l_oCreator.f_oInfo = a_oInfo;
	f_oCreators[ l_oName ] = l_oCreator;
	return;
	M_EPILOG
	}

HLogic::ptr_t HLogicFactory::create_logic( HString const& a_oType, HString const& a_oArgv )
	{
	M_PROLOG
	HLogic::ptr_t l_oLogic;
	creators_t::iterator it = f_oCreators.find( a_oType );
	if ( it != f_oCreators.end() )
		l_oLogic = it->second.f_poInstatiator->new_instance( a_oArgv );
	return ( l_oLogic );
	M_EPILOG
	}

bool HLogicFactory::is_type_valid( yaal::hcore::HString const& a_oType )
	{
	M_PROLOG
	bool valid = false;
	creators_t::iterator it = f_oCreators.find( a_oType );
	if ( it != f_oCreators.end() )
		valid = true;
	return ( valid );
	M_EPILOG
	}

HLogicFactory::creators_t::iterator HLogicFactory::begin( void )
	{
	return ( f_oCreators.begin() );
	}

HLogicFactory::creators_t::iterator HLogicFactory::end( void )
	{
	return ( f_oCreators.end() );
	}

int HLogicFactory::life_time( int a_iLifeTime )
	{
	return ( a_iLifeTime );
	}

void HLogicFactory::initialize_globals( void )
	{
	M_PROLOG
	for ( creators_t::iterator it = f_oCreators.begin(); it != f_oCreators.end(); ++ it )
		it->second.f_poInstatiator->initialize_globals();
	return;
	M_EPILOG
	}

}

