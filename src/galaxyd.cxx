/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	galaxyd.cxx - this file is integral part of `galaxy' project.

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

#include <iostream>

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )
#include "galaxyd.h"

#include "setup.h"
#include "clientinfo.h"
#include "logicfactory.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace galaxy
{

HSystem::HSystem ( void ) : f_iId ( - 1 ),
														f_iCoordinateX ( - 1 ), f_iCoordinateY ( - 1 ),
														f_iProduction ( - 1 ), f_iFleet ( - 1 ),
														f_poEmperor ( NULL ), f_oAttackers ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HSystem::do_round( HGalaxy& a_roGalaxy )
	{
	M_PROLOG
	int l_iColor = 0;
	HString l_oMessage;
	if ( f_poEmperor != NULL )
		f_iFleet += f_iProduction;
	else
		f_iFleet = f_iProduction;
	if ( f_oAttackers.size() )
		{
		for ( attackers_t::iterator it = f_oAttackers.begin(); it != f_oAttackers.end(); )
			{
			it->f_iArrivalTime --;
			if ( it->f_iArrivalTime <= 0 )
				{
				l_iColor = a_roGalaxy.get_color( f_poEmperor );
				if ( it->f_poEmperor == f_poEmperor ) /* reinforcements */
					{
					f_iFleet += it->f_iSize;
					l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
							'r', f_iId, f_iProduction, l_iColor, f_iFleet );
					f_poEmperor->f_oSocket->write_until_eos ( l_oMessage );
					}
				else if ( it->f_iSize <= f_iFleet ) /* failed attack */
					{
					f_iFleet -= it->f_iSize;
					l_iColor = a_roGalaxy.get_color ( it->f_poEmperor );
					l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
							's', f_iId, f_iProduction,
							l_iColor, f_iFleet );
					f_poEmperor->f_oSocket->write_until_eos ( l_oMessage ); /* defender */
					l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
							'f', f_iId, f_iProduction, a_roGalaxy.get_color ( f_poEmperor ), - 1 );
					it->f_poEmperor->f_oSocket->write_until_eos ( l_oMessage ); /* attacker */
					}
				else if ( it->f_iSize > f_iFleet )
					{
					f_iFleet = it->f_iSize - f_iFleet;
					l_iColor = a_roGalaxy.get_color ( it->f_poEmperor );
					l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
							'd', f_iId, f_iProduction, l_iColor, - 1 );
					f_poEmperor->f_oSocket->write_until_eos ( l_oMessage ); /* loser */
					f_poEmperor = it->f_poEmperor;
					l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
							'c', f_iId, f_iProduction, l_iColor, f_iFleet );
					it->f_poEmperor->f_oSocket->write_until_eos ( l_oMessage ); /* winer */
					}
				attackers_t::iterator done = it;
				++ it;
				f_oAttackers.erase( done );
				}
			else
				{
				a_roGalaxy.mark_alive ( it->f_poEmperor );
				++ it;
				}
			}
		}
	if ( f_poEmperor != NULL )
		{
		l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
				'i', f_iId, - 1, a_roGalaxy.get_color ( f_poEmperor ), f_iFleet );
		f_poEmperor->f_oSocket->write_until_eos ( l_oMessage );
		}
	a_roGalaxy.mark_alive( f_poEmperor );
	return;
	M_EPILOG
	}

HGalaxy::HGalaxy( HString const& a_oName, int a_iBoardSize, int a_iSystems, int a_iEmperors )
	: HLogic( a_oName ), f_iBoardSize( a_iBoardSize ), f_iSystems( a_iSystems ),
	f_iEmperors( a_iEmperors ), f_iRound( 0 ), f_iReady( 0 ),
	f_oSystems( a_iSystems + a_iEmperors ),
	f_oHandlers( 16 ), f_oEmperors()
	{
	M_PROLOG
	int l_iCtr = 0, l_iCtrLoc = 0;
	HRandomizer l_oRandom;
	HSystem * l_poSystem = NULL;
	l_oRandom.set ( time ( NULL ) );
	for ( l_iCtr = 0; l_iCtr < ( a_iEmperors + a_iSystems ); l_iCtr ++ )
		{
		l_poSystem = & f_oSystems [ l_iCtr ];
		l_poSystem->f_iId = l_iCtr;
		l_poSystem->f_iCoordinateX = l_oRandom.rnd ( f_iBoardSize );
		l_poSystem->f_iCoordinateY = l_oRandom.rnd ( f_iBoardSize );
		if ( l_iCtr )
			{
			for ( l_iCtrLoc = 0; l_iCtrLoc < l_iCtr; l_iCtrLoc ++ )
				if ( ( l_poSystem->f_iCoordinateX
							== f_oSystems [ l_iCtrLoc ].f_iCoordinateX )
						&& ( l_poSystem->f_iCoordinateY
							== f_oSystems [ l_iCtrLoc ].f_iCoordinateY ) )
					break;
			if ( l_iCtrLoc < l_iCtr )
				{
				l_iCtr --;
				continue;
				}
			}
		}
	for ( l_iCtr = 0; l_iCtr < ( a_iEmperors + a_iSystems ); l_iCtr ++ )
		f_oSystems [ l_iCtr ].f_iProduction = f_oSystems [ l_iCtr ].f_iFleet = l_oRandom.rnd ( 16 );
	f_oHandlers [ "PLAY" ] = & HGalaxy::handler_play;
	f_oHandlers [ "SAY" ] = & HGalaxy::handler_message;
	return;
	M_EPILOG
	}

HGalaxy::~HGalaxy ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HGalaxy::process_command( OClientInfo* a_poClientInfo, HString const& a_roCommand )
	{
	M_PROLOG
	HString l_oMnemonic;
	HString l_oArgument;
	handler_t HANDLER;
	l_oMnemonic = a_roCommand.split ( ":", 0 );
	l_oArgument = a_roCommand.mid ( l_oMnemonic.get_length ( ) + 1 );
	if ( f_oHandlers.get ( l_oMnemonic, HANDLER ) )
		( this->*HANDLER ) ( a_poClientInfo, l_oArgument );
	else
		kick_client( a_poClientInfo );
	return;
	M_EPILOG
	}

bool HGalaxy::do_accept( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	bool rejected = false;
	int l_iCtr = 0, l_iColor = -1, l_iSysNo = -1;
	HString l_oMessage;
	if ( f_iReady < ( f_iEmperors - 1 ) )
		{
		l_iSysNo = assign_system( a_poClientInfo ); /* assign mother system for new emperor */
		l_oMessage.format ( "GLX:MSG:Emperor ;$%d;", l_iColor = get_emperor_info( a_poClientInfo )->f_iColor );
		l_oMessage += a_poClientInfo->f_oName;
		l_oMessage += ";$12; invaded the galaxy.\n";
		broadcast( NULL, l_oMessage ); /* inform every emperor about new rival */
		l_oMessage.format ( "GLX:SETUP:board_size=%d\n", f_iBoardSize );
		a_poClientInfo->f_oSocket->write_until_eos( l_oMessage ); /* send setup info to new emperor */
		l_oMessage.format ( "GLX:SETUP:systems=%d\n", f_iEmperors + f_iSystems );
		a_poClientInfo->f_oSocket->write_until_eos( l_oMessage );
		l_oMessage.format ( "GLX:SETUP:emperor=%d,%s\n",
				l_iColor, static_cast<char const* const>( a_poClientInfo->f_oName ) );
		broadcast( NULL, l_oMessage ); /* send setup information about new rival to all emperors */
		for ( emperors_t::HIterator it = f_oEmperors.begin(); it != f_oEmperors.end(); ++ it )
			{
			if ( it->first != a_poClientInfo )
				{
				int l_iClr = 0;
				l_oMessage.format ( "GLX:SETUP:emperor=%d,%s\n",
						l_iClr = it->second.f_iColor,
						static_cast<char const* const>( it->first->f_oName ) );
				a_poClientInfo->f_oSocket->write_until_eos ( l_oMessage );
				l_oMessage.format ( "GLX:MSG:Emperor ;$%d;", l_iClr );
				l_oMessage += it->first->f_oName;
				l_oMessage += ";$12; invaded the galaxy.\n";
				a_poClientInfo->f_oSocket->write_until_eos ( l_oMessage );
				}
			}
		for ( l_iCtr = 0; l_iCtr < ( f_iEmperors + f_iSystems ); l_iCtr ++ )
			{
			l_oMessage.format ( "GLX:SETUP:system_coordinates=%d,%d,%d\n",
					l_iCtr, f_oSystems [ l_iCtr ].f_iCoordinateX,
					f_oSystems [ l_iCtr ].f_iCoordinateY );
			a_poClientInfo->f_oSocket->write_until_eos ( l_oMessage );
			}
		f_oSystems [ l_iSysNo ].f_iProduction = 10;
		f_oSystems [ l_iSysNo ].f_iFleet = 10;
		l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
				'c', l_iSysNo, f_oSystems [ l_iSysNo ].f_iProduction, l_iColor,
				f_oSystems [ l_iSysNo ].f_iFleet );
		a_poClientInfo->f_oSocket->write_until_eos ( l_oMessage );
		f_iReady ++;
		if ( f_iReady >= f_iEmperors )
			{
			broadcast ( NULL, "GLX:SETUP:ok\n" );
			f_iReady = 0;
			}
		rejected = false;
		}
	else
		rejected = true;
	return ( rejected );
	M_EPILOG
	}

int HGalaxy::assign_system( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	OEmperorInfo info;
	HRandomizer l_oRnd;
	l_oRnd.set ( time ( NULL ) );
	typedef HSet<int> integer_set_t;
	integer_set_t l_oUsed;
	for ( emperors_t::HIterator it = f_oEmperors.begin(); it != f_oEmperors.end(); ++ it )
		l_oUsed.insert( it->second.f_iColor );
	int l_iCtr = 0;
	for ( integer_set_t::HIterator it = l_oUsed.begin(); it != l_oUsed.end(); ++it, ++ l_iCtr )
		if ( *it != l_iCtr )
			break;
	info.f_iColor = l_iCtr;
	info.f_iSystems = 1;
	int l_iRivals = f_oEmperors.size();
	f_oEmperors[ a_poClientInfo ] = info;
	int l_iMotherSystem = l_oRnd.rnd ( f_iEmperors + f_iSystems - l_iRivals );
	l_iCtr = 0;
	for ( int i = 0; i < l_iMotherSystem; ++ i, ++ l_iCtr )
		if ( f_oSystems[ l_iCtr ].f_poEmperor != NULL )
			++ l_iCtr;
	f_oSystems [ l_iCtr ].f_poEmperor = a_poClientInfo;
	return ( l_iCtr );
	M_EPILOG
	}

void HGalaxy::broadcast( OClientInfo*, HString const& a_roMessage )
	{
	M_PROLOG
	for ( clients_t::HIterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		(*it)->f_oSocket->write_until_eos ( a_roMessage );
	return;
	M_EPILOG
	}

void HGalaxy::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "GLX:MSG:$";
	l_oMessage += get_emperor_info( a_poClientInfo )->f_iColor;
	l_oMessage += ';';
	l_oMessage += a_poClientInfo->f_oName;
	l_oMessage += ";$12;: ";
	l_oMessage += a_roMessage;
	l_oMessage += '\n';
	broadcast( NULL, l_oMessage );
	return;
	M_EPILOG
	}

void HGalaxy::handler_play ( OClientInfo* a_poClientInfo, HString const& a_roCommand )
	{
	M_PROLOG
	int l_iSource = - 1, l_iDestination = - 1, l_iDX = 0, l_iDY = 0;
	int l_iCtr = 0, l_iDead = 0;
	HString l_oVariable;
	HString l_oValue;
	HFleet l_oFleet;
	l_oVariable = a_roCommand.split ( "=", 0 );
	l_oValue = a_roCommand.split ( "=", 1 );
	if ( l_oVariable == "move" )
		{
		l_oFleet.f_poEmperor = a_poClientInfo;
		l_iSource = strtol ( l_oValue.split ( ",", 0 ), NULL, 10 );
		l_iDestination = strtol ( l_oValue.split ( ",", 1 ), NULL, 10 );
		l_oFleet.f_iSize = strtol ( l_oValue.split ( ",", 2 ), NULL, 10 );
		if ( ( l_iSource == l_iDestination )
				&& ( f_oSystems [ l_iSource ].f_poEmperor != a_poClientInfo )
				&& ( f_oSystems [ l_iSource ].f_iFleet < l_oFleet.f_iSize ) )
			kick_client( a_poClientInfo );
		else
			{
			f_oSystems [ l_iSource ].f_iFleet -= l_oFleet.f_iSize;
			l_iDX = f_oSystems [ l_iSource ].f_iCoordinateX - f_oSystems [ l_iDestination ].f_iCoordinateX;
			l_iDY = f_oSystems [ l_iSource ].f_iCoordinateY - f_oSystems [ l_iDestination ].f_iCoordinateY;
			l_iDX = ( l_iDX >= 0 ) ? l_iDX : - l_iDX;
			l_iDY = ( l_iDY >= 0 ) ? l_iDY : - l_iDY;
			l_iDX = ( ( f_iBoardSize - l_iDX ) < l_iDX ) ? f_iBoardSize - l_iDX : l_iDX;
			l_iDY = ( ( f_iBoardSize - l_iDY ) < l_iDY ) ? f_iBoardSize - l_iDY : l_iDY;
			l_oFleet.f_iArrivalTime = static_cast < int > ( sqrt ( l_iDX * l_iDX + l_iDY * l_iDY ) + 0.5 );
			f_oSystems [ l_iDestination ].f_oAttackers.add_head ( ) = l_oFleet;
			}
		}
	else if ( l_oVariable == "end_round" )
		{
		OEmperorInfo* l_poInfo = get_emperor_info( a_poClientInfo );
		M_ASSERT( l_poInfo );
		if ( l_poInfo->f_iSystems >= 0 )
			f_iReady ++;
		if ( f_iReady >= f_iEmperors )
			{
			f_iReady = 0;
			for ( emperors_t::HIterator it = f_oEmperors.begin(); it != f_oEmperors.end(); ++ it )
				if ( it->second.f_iSystems > 0 )
					it->second.f_iSystems = 0;
			for ( l_iCtr = 0; l_iCtr < ( f_iSystems + f_iEmperors ); l_iCtr ++ )
				f_oSystems [ l_iCtr ].do_round( *this );
			for ( emperors_t::HIterator it = f_oEmperors.begin(); it != f_oEmperors.end(); ++ it )
				{
				if ( ! it->second.f_iSystems )
					{
					it->second.f_iSystems = -1;
					l_oVariable.format( "GLX:MSG:Once mighty empire of ;$%d;%s;$12; fall in ruins.\n",
							it->second.f_iColor, static_cast<char const* const>( it->first->f_oName ) );
					broadcast( NULL, l_oVariable );
					}
				if ( it->second.f_iSystems >= 0 )
					f_iReady ++;
				else
					l_iDead ++;
				}
			if ( f_iReady == 1 )
				{
				for ( emperors_t::HIterator it = f_oEmperors.begin(); it != f_oEmperors.end(); ++ it )
					{
					if ( it->second.f_iSystems > 0 )
						{
						l_oVariable.format( "GLX:MSG:The invincible ;$%d;%s;$12; crushed the galaxy.\n",
								it->second.f_iColor, static_cast<char const* const>( it->first->f_oName ) );
						broadcast( NULL, l_oVariable );
						}
					}
				}
			f_iRound ++;
			l_oValue.format ( "GLX:PLAY:round=%d\n", f_iRound );
			broadcast( NULL, l_oValue );
			f_iReady = l_iDead;
			}
		}
	return;
	M_EPILOG
	}

int HGalaxy::get_color ( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	return ( get_emperor_info( a_poClientInfo )->f_iColor );
	M_EPILOG
	}

void HGalaxy::mark_alive ( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	get_emperor_info( a_poClientInfo )->f_iSystems ++;
	return;
	M_EPILOG
	}

HGalaxy::OEmperorInfo* HGalaxy::get_emperor_info( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	emperors_t::HIterator it = f_oEmperors.find( a_poClientInfo );
	M_ASSERT( it != f_oEmperors.end() );
	return ( &it->second );
	M_EPILOG
	}

void HGalaxy::do_kick( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	for ( int l_iCtr = 0; l_iCtr < ( f_iSystems + f_iEmperors ); ++ l_iCtr )
		{
		HSystem::attackers_t& l_oAttackers = f_oSystems[ l_iCtr ].f_oAttackers;
		for ( HSystem::attackers_t::iterator it = l_oAttackers.begin();
				it != l_oAttackers.end(); ++ it )
			{
			if ( it->f_poEmperor == a_poClientInfo )
				{
				HSystem::attackers_t::iterator kicked = it;
				++ it;
				l_oAttackers.erase( kicked );
				}
			}
		if ( f_oSystems[ l_iCtr ].f_poEmperor == a_poClientInfo )
			f_oSystems[ l_iCtr ].f_poEmperor = NULL;
		}
	f_oEmperors.remove( a_poClientInfo );
	-- f_iEmperors;
	return;
	M_EPILOG
	}

yaal::hcore::HString HGalaxy::get_info() const
	{
	return ( HString( "glx," ) + f_oName + "," + f_oEmperors.size() + "," + f_iEmperors + "," + f_iBoardSize + "," + f_iSystems );
	}

}

namespace logic_factory
{

HLogic::ptr_t create_logic_galaxy( HString const& a_oArgv )
	{
	M_PROLOG
	HString l_oName = a_oArgv.split( ",", 0 );
	HString l_oBoardSize = a_oArgv.split( ",", 1 );
	HString l_oSystems = a_oArgv.split( ",", 2 );
	HString l_oEmperors = a_oArgv.split( ",", 3 );
	int l_iBoardSize = strtol( l_oBoardSize, NULL, 10 );
	int l_iSystems = strtol( l_oSystems, NULL, 10 );
	int l_iEmperors = strtol( l_oEmperors, NULL, 10 );
	char* l_pcMessage = NULL;
	cout << "new glx: ( " << l_oName << " ) {" << endl;
	cout << "board_size = " << l_iBoardSize << endl;
	cout << "emperors = " << l_iEmperors << endl;
	cout << "systems = " << l_iSystems << endl;
	cout << "};" << endl;
	if ( OSetup::test_glx_emperors( l_iEmperors, l_pcMessage )
			|| OSetup::test_glx_emperors_systems( l_iEmperors, l_iSystems, l_pcMessage )
			|| OSetup::test_glx_systems( l_iSystems, l_pcMessage )
			|| OSetup::test_glx_board_size( l_iBoardSize, l_pcMessage ) )
		throw HLogicException( l_pcMessage );
	return ( HLogic::ptr_t( new galaxy::HGalaxy( l_oName,
					l_iBoardSize,
					l_iSystems,
					l_iEmperors ) ) );
	M_EPILOG
	}

namespace
{

bool registrar( void )
	{
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	HString l_oSetup;
	l_oSetup.format( "%s:%d,%d,%d", "glx", setup.f_iBoardSize, setup.f_iSystems, setup.f_iEmperors );
	factory.register_logic_creator( l_oSetup, create_logic_galaxy );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

