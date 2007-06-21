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

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )
#include "galaxyd.h"

#include "setup.h"

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
														f_iEmperor ( - 1 ), f_oAttackers ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HSystem::do_round ( HGalaxy & a_roGalaxy )
	{
	M_PROLOG
	int l_iColor = 0;
	HFleet * l_poFleet = NULL;
	HSocket::ptr_t l_oSocket;
	HString l_oMessage;
	if ( f_iEmperor >= 0 )
		f_iFleet += f_iProduction;
	else
		f_iFleet = f_iProduction;
	if ( f_oAttackers.size ( ) )
		{
		l_poFleet = & f_oAttackers.go ( 0 );
		while ( l_poFleet )
			{
			l_poFleet->f_iArrivalTime --;
			if ( l_poFleet->f_iArrivalTime <= 0 )
				{
				l_oSocket = a_roGalaxy.get_socket ( f_iEmperor );
				l_iColor = a_roGalaxy.get_color ( f_iEmperor );
				if ( l_poFleet->f_iEmperor == f_iEmperor ) /* reinforcements */
					{
					f_iFleet += l_poFleet->f_iSize;
					if ( !! l_oSocket )
						{
						l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
								'r', f_iId, f_iProduction, l_iColor, f_iFleet );
						l_oSocket->write_until_eos ( l_oMessage );
						}
					}
				else if ( l_poFleet->f_iSize <= f_iFleet ) /* failed attack */
					{
					f_iFleet -= l_poFleet->f_iSize;
					l_iColor = a_roGalaxy.get_color ( l_poFleet->f_iEmperor );
					if ( !! l_oSocket ) /* defender */
						{
						l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
								's', f_iId, f_iProduction,
								l_iColor, f_iFleet );
						l_oSocket->write_until_eos ( l_oMessage );
						}
					l_oSocket = a_roGalaxy.get_socket ( l_poFleet->f_iEmperor );
					if ( !! l_oSocket ) /* attacker */
						{
						l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
								'f', f_iId, f_iProduction, a_roGalaxy.get_color ( f_iEmperor ), - 1 );
						l_oSocket->write_until_eos ( l_oMessage );
						}
					}
				else if ( l_poFleet->f_iSize > f_iFleet )
					{
					f_iEmperor = l_poFleet->f_iEmperor;
					f_iFleet = l_poFleet->f_iSize - f_iFleet;
					l_iColor = a_roGalaxy.get_color ( f_iEmperor );
					if ( !! l_oSocket ) /* loser */
						{
						l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
								'd', f_iId, f_iProduction, l_iColor, - 1 );
						l_oSocket->write_until_eos ( l_oMessage );
						}
					l_oSocket = a_roGalaxy.get_socket ( f_iEmperor );
					if ( !! l_oSocket ) /* winner */
						{
						l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
								'c', f_iId, f_iProduction, l_iColor, f_iFleet );
						l_oSocket->write_until_eos ( l_oMessage );
						}
					}
				f_oAttackers.remove_element ( attackers_t::D_TREAT_AS_OPENED, & l_poFleet );
				if ( l_poFleet && f_oAttackers.size ( ) )
					{
					l_poFleet = & f_oAttackers.present ( );
					continue;
					}
				else
					break;
				}
			else
				{
				a_roGalaxy.mark_alive ( l_poFleet->f_iEmperor );
				l_poFleet = f_oAttackers.to_tail ( 1, attackers_t::D_TREAT_AS_OPENED );
				}
			}
		}
	if ( f_iEmperor >= 0 )
		{
		l_oSocket = a_roGalaxy.get_socket ( f_iEmperor );
		if ( !! l_oSocket )
			{
			l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
					'i', f_iId, - 1, a_roGalaxy.get_color ( f_iEmperor ), f_iFleet );
			l_oSocket->write_until_eos ( l_oMessage );
			}
		}
	a_roGalaxy.mark_alive ( f_iEmperor );
	return;
	M_EPILOG
	}

HGalaxy::HGalaxy ( int a_iBoardSize, int a_iSystems, int a_iEmperors )
	: f_iBoardSize ( a_iBoardSize ), f_iSystems ( a_iSystems ),
	f_iEmperors ( a_iEmperors ), f_iRound ( 0 ), f_iReady ( 0 ),
	f_oSystems ( a_iSystems + a_iEmperors ), f_oSocket ( NULL ),
	f_oHandlers ( 16 ), f_oNames ( a_iEmperors ), f_oColors ( a_iEmperors ),
	f_oEmperorCounter ( a_iEmperors, 0 )
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
	for ( l_iCtr = 0; l_iCtr < a_iEmperors; l_iCtr ++ )
		f_oColors [ l_iCtr ] = - 1;
	for ( l_iCtr = 0; l_iCtr < ( a_iEmperors + a_iSystems ); l_iCtr ++ )
		f_oSystems [ l_iCtr ].f_iProduction = f_oSystems [ l_iCtr ].f_iFleet = l_oRandom.rnd ( 16 );
	f_oHandlers [ "LOGIN" ] = & HGalaxy::handler_login;
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

void HGalaxy::set_socket ( HSocket::ptr_t a_poSocket )
	{
	M_PROLOG
	M_ASSERT ( a_poSocket );
	f_oSocket = a_poSocket;
	return;
	M_EPILOG
	}

void HGalaxy::process_command ( int a_iFileDescriptor, HString& a_roCommand )
	{
	M_PROLOG
	HString l_oMnemonic;
	HString l_oArgument;
	handler_t HANDLER;
	l_oMnemonic = a_roCommand.split ( ":", 0 );
	l_oArgument = a_roCommand.mid ( l_oMnemonic.get_length ( ) + 1 );
	if ( f_oHandlers.get ( l_oMnemonic, HANDLER ) )
		( this->*HANDLER ) ( a_iFileDescriptor, l_oArgument );
	else
		f_oSocket->shutdown_client ( a_iFileDescriptor );
	return;
	M_EPILOG
	}

void HGalaxy::handler_login ( int a_iFileDescriptor, HString& a_roName )
	{
	M_PROLOG
	int l_iCtr = 0, l_iSysNo = - 1, l_iEmperor = - 1;
	HSocket::ptr_t l_oClient;
	HString l_oName;
	HString l_oMessage;
	l_iSysNo = assign_system ( a_iFileDescriptor, l_iEmperor );
	l_oMessage.format ( "GLX:MSG:Emperor ;$%d;", l_iEmperor );
	f_oNames [ a_iFileDescriptor ] = a_roName;
	l_oMessage += a_roName;
	l_oMessage += ";$12; invaded the galaxy.\n";
	broadcast ( l_oMessage );
	l_oClient = f_oSocket->get_client ( a_iFileDescriptor );
	l_oMessage.format ( "GLX:SETUP:board_size=%d\n", f_iBoardSize );
	l_oClient->write_until_eos ( l_oMessage );
	l_oMessage.format ( "GLX:SETUP:systems=%d\n", f_iEmperors + f_iSystems );
	l_oClient->write_until_eos ( l_oMessage );
	for ( l_iCtr = 0; l_iCtr < f_iEmperors; l_iCtr ++ )
		{
		if ( ( f_oColors [ l_iCtr ] >= 0 )
				&& ( f_oNames.get ( f_oColors [ l_iCtr ], l_oName ) ) )
			{
			l_oMessage.format ( "GLX:SETUP:emperor=%d,%s\n",
					l_iCtr, static_cast < char const * const > ( l_oName ) );
			broadcast ( l_oMessage );
			if ( l_iCtr != l_iEmperor )
				{
				l_oMessage.format ( "GLX:MSG:Emperor ;$%d;", l_iCtr );
				l_oMessage += l_oName;
				l_oMessage += ";$12; invaded the galaxy.\n";
				l_oClient->write_until_eos ( l_oMessage );
				}
			}
		}
	for ( l_iCtr = 0; l_iCtr < ( f_iEmperors + f_iSystems ); l_iCtr ++ )
		{
		l_oMessage.format ( "GLX:SETUP:system_coordinates=%d,%d,%d\n",
				l_iCtr, f_oSystems [ l_iCtr ].f_iCoordinateX,
				f_oSystems [ l_iCtr ].f_iCoordinateY );
		l_oClient->write_until_eos ( l_oMessage );
		}
	f_oSystems [ l_iSysNo ].f_iProduction = 10;
	f_oSystems [ l_iSysNo ].f_iFleet = 10;
	l_oMessage.format ( "GLX:PLAY:system_info=%c,%d,%d,%d,%d\n",
			'c', l_iSysNo, f_oSystems [ l_iSysNo ].f_iProduction, l_iEmperor,
			f_oSystems [ l_iSysNo ].f_iFleet );
	l_oClient->write_until_eos ( l_oMessage );
	f_iReady ++;
	if ( f_iReady >= f_iEmperors )
		{
		broadcast ( "GLX:SETUP:ok\n" );
		f_iReady = 0;
		}
	return;
	M_EPILOG
	}

int HGalaxy::assign_system ( int a_iFileDescriptor, int& a_riColor )
	{
	M_PROLOG
	int l_iCtr = - 1;
	HRandomizer l_oRnd;
	l_oRnd.set ( time ( NULL ) );
	a_riColor = 0;
	while ( f_oColors [ a_riColor ] >= 0 )
		a_riColor ++;
	f_oColors [ a_riColor ] = a_iFileDescriptor;
	while ( f_oSystems [ l_iCtr = l_oRnd.rnd ( f_iEmperors + f_iSystems ) ].f_iEmperor >= 0 )
		;
	f_oSystems [ l_iCtr ].f_iEmperor = a_iFileDescriptor;
	return ( l_iCtr );
	M_EPILOG
	}

void HGalaxy::broadcast ( HString const & a_roMessage )
	{
	M_PROLOG
	int l_iFileDescriptor = - 1;
	HSocket::ptr_t l_oClient;
	f_oSocket->rewind_client_list ( );
	while ( f_oSocket->get_client_next ( l_iFileDescriptor, l_oClient ) )
		l_oClient->write_until_eos ( a_roMessage );
	return;
	M_EPILOG
	}

void HGalaxy::handler_message ( int a_iFileDescriptor, HString & a_roMessage )
	{
	M_PROLOG
	HString l_oName;
	HString l_oMessage;
	if ( f_oNames.get ( a_iFileDescriptor, l_oName ) )
		{
		l_oMessage = "GLX:MSG:$";
		l_oMessage += get_color ( a_iFileDescriptor );
		l_oMessage += ';';
		l_oMessage += l_oName;
		l_oMessage += ";$12;: ";
		l_oMessage += a_roMessage;
		l_oMessage += '\n';
		broadcast ( l_oMessage );
		}
	else
		f_oSocket->shutdown_client ( a_iFileDescriptor );
	return;
	M_EPILOG
	}

void HGalaxy::handler_play ( int a_iFileDescriptor, HString & a_roCommand )
	{
	M_PROLOG
	int l_iSource = - 1, l_iDestination = - 1, l_iDX = 0, l_iDY = 0;
	int l_iCtr = 0, l_iDead = 0, l_iColor = 0;
	HString l_oVariable;
	HString l_oValue;
	HFleet l_oFleet;
	l_oVariable = a_roCommand.split ( "=", 0 );
	l_oValue = a_roCommand.split ( "=", 1 );
	if ( l_oVariable == "move" )
		{
		l_oFleet.f_iEmperor = a_iFileDescriptor;
		l_iSource = strtol ( l_oValue.split ( ",", 0 ), NULL, 10 );
		l_iDestination = strtol ( l_oValue.split ( ",", 1 ), NULL, 10 );
		l_oFleet.f_iSize = strtol ( l_oValue.split ( ",", 2 ), NULL, 10 );
		if ( ( l_iSource == l_iDestination )
				&& ( f_oSystems [ l_iSource ].f_iEmperor != a_iFileDescriptor )
				&& ( f_oSystems [ l_iSource ].f_iFleet < l_oFleet.f_iSize ) )
			f_oSocket->shutdown_client ( a_iFileDescriptor );
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
		l_iColor = get_color ( a_iFileDescriptor );
		if ( ( l_iColor >= 0 ) && ( f_oEmperorCounter [ l_iColor ] >= 0 ) )
			f_iReady ++;
		if ( f_iReady >= f_iEmperors )
			{
			f_iReady = 0;
			for ( l_iCtr = 0; l_iCtr < f_iEmperors; l_iCtr ++ )
				if ( f_oEmperorCounter [ l_iCtr ] > 0 )
					f_oEmperorCounter [ l_iCtr ] = 0;
			for ( l_iCtr = 0; l_iCtr < ( f_iSystems + f_iEmperors ); l_iCtr ++ )
				f_oSystems [ l_iCtr ].do_round ( * this );
			for ( l_iCtr = 0; l_iCtr < f_iEmperors; l_iCtr ++ )
				{
				if ( ! f_oEmperorCounter [ l_iCtr ] )
					{
					f_oEmperorCounter [ l_iCtr ] = - 1;
					if ( f_oNames.get ( f_oColors [ l_iCtr ], l_oValue ) )
						{
						l_oVariable.format ( "GLX:MSG:Once mighty empire of ;$%d;%s;$12; fall in ruins.\n",
								l_iCtr, static_cast < char const * const > ( l_oValue ) );
						broadcast ( l_oVariable );
						}
					}
				if ( f_oEmperorCounter [ l_iCtr ] >= 0 )
					f_iReady ++;
				else
					l_iDead ++;
				}
			if ( f_iReady == 1 )
				{
				for ( l_iCtr = 0; l_iCtr < f_iEmperors; l_iCtr ++ )
					{
					if ( ( f_oEmperorCounter [ l_iCtr ] > 0 ) && f_oNames.get ( f_oColors [ l_iCtr ], l_oValue ) )
						{
						l_oVariable.format ( "GLX:MSG:The invincible ;$%d;%s;$12; crushed the galaxy.\n",
								l_iCtr, static_cast < char const * const > ( l_oValue ) );
						broadcast ( l_oVariable );
						}
					}
				}
			f_iRound ++;
			l_oValue.format ( "GLX:PLAY:round=%d\n", f_iRound );
			broadcast ( l_oValue );
			f_iReady = l_iDead;
			}
		}
	return;
	M_EPILOG
	}

HSocket::ptr_t HGalaxy::get_socket ( int a_iFileDescriptor )
	{
	M_PROLOG
	return ( f_oSocket->get_client ( a_iFileDescriptor ) );
	M_EPILOG
	}

int HGalaxy::get_color ( int a_iEmperor )
	{
	M_PROLOG
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iEmperors; l_iCtr ++ )
		{
		if ( f_oColors [ l_iCtr ] == a_iEmperor )
			return ( l_iCtr );
		}
	return ( - 1 );
	M_EPILOG
	}

void HGalaxy::mark_alive ( int a_iEmperor )
	{
	M_PROLOG
	if ( a_iEmperor >= 0 )
		a_iEmperor = get_color ( a_iEmperor );
	if ( a_iEmperor >= 0 )
		f_oEmperorCounter [ a_iEmperor ] ++;
	M_EPILOG
	}

}
