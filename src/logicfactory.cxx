/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logicfactory.cxx - this file is integral part of `galaxy' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <libintl.h>

#include <yaal/yaal.h>

M_VCSID ( "$Id$" )
#include "logicfactory.h"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

void HLogicFactory::register_logic_creator( HString const& a_oName, creator_t CREATOR )
	{
	M_PROLOG
	creators_t::HIterator it = f_oCreators.find( a_oName );
	if ( it != f_oCreators.end() )
		M_THROW( _( "Logic already registered" ), errno );
	f_oCreators[ a_oName ] = CREATOR;
	return;
	M_EPILOG
	}

HLogic::ptr_t HLogicFactory::create_logic( HString const& a_oType, HString const& a_oArgv )
	{
	M_PROLOG
	HLogic::ptr_t l_oLogic;
	creators_t::HIterator it = f_oCreators.find( a_oType );
	if ( it != f_oCreators.end() )
		l_oLogic = ( it->second )( a_oArgv );
	return ( l_oLogic );
	M_EPILOG
	}

bool HLogicFactory::is_type_valid( yaal::hcore::HString const& a_oType )
	{
	M_PROLOG
	bool valid = false;
	creators_t::HIterator it = f_oCreators.find( a_oType );
	if ( it != f_oCreators.end() )
		valid = true;
	return ( valid );
	M_EPILOG
	}

