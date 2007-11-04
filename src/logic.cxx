/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logic.cxx - this file is integral part of `gameground' project.

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

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )
#include "logic.h"

#include "setup.h"
#include "clientinfo.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

HLogic::HLogic( HString const& a_oSymbol, HString const& a_oName )
	: f_oSymbol( a_oSymbol ), f_oHandlers( setup.f_iMaxConnections ),
	f_oClients(), f_oName( a_oName )
	{
	}

HLogic::~HLogic( void )
	{
	}

void HLogic::kick_client( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	do_kick( a_poClientInfo );
	a_poClientInfo->f_oLogic = HLogic::ptr_t();
	f_oClients.remove( a_poClientInfo );
	return;
	M_EPILOG
	}

bool HLogic::do_accept( OClientInfo* )
	{
	return ( true );
	}

void HLogic::do_kick( OClientInfo* )
	{
	return;
	}

bool HLogic::accept_client( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	bool rejected = false;
	if ( ! do_accept( a_poClientInfo ) )
		f_oClients.insert( a_poClientInfo );
	else
		rejected = true;
	return ( rejected );
	M_EPILOG
	}

int HLogic::active_clients( void ) const
	{
	M_PROLOG
	return ( f_oClients.size() );
	M_EPILOG
	}

HString const& HLogic::get_name( void ) const
	{
	return ( f_oName );
	}

void HLogic::process_command( OClientInfo* a_poClientInfo, HString const& a_roCommand )
	{
	M_PROLOG
	HString l_oMnemonic;
	HString l_oArgument;
	handler_t HANDLER;
	l_oArgument = a_roCommand;
	while ( ( l_oMnemonic = l_oArgument.split( ":", 0 ) ) == f_oSymbol )
		l_oArgument = l_oArgument.mid( l_oMnemonic.get_length() + 1 );
	l_oMnemonic = l_oArgument.split( ":", 0 );
	l_oArgument = l_oArgument.mid( l_oMnemonic.get_length() + 1 );
	if ( f_oHandlers.get( l_oMnemonic, HANDLER ) )
		( this->*HANDLER )( a_poClientInfo, l_oArgument );
	else
		kick_client( a_poClientInfo );
	return;
	M_EPILOG
	}

