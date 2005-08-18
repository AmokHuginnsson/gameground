/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	galaxyd.c - this file is integral part of `galaxy' project.

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

#include <stdhapi.h>
M_CVSID ( "$CVSHeader$" );
#include "galaxyd.h"

#include "setup.h"

using namespace std;
using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::tools;
using namespace stdhapi::tools::util;

namespace
{

class HSystem;
class HGalaxy;
class HFleet
	{
protected:
	/*{*/
	int f_iSize;
	int f_iEmperor;
	int f_iArrivalTime;
	/*}*/
public:
	/*{*/
	/*}*/
protected:
	/*{*/
	/*}*/
	friend class HSystem;
	friend class HGalaxy;
	};

class HGalaxy;
class HSystem
	{
protected:
	/*{*/
	int f_iCoordinateX;
	int f_iCoordinateY;
	int f_iProduction;
	int f_iFleet;
	int f_iEmperor;
	HList < HFleet > f_oAttackers;
	/*}*/
public:
	/*{*/
	HSystem ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	HSystem ( HSystem const & );
	HSystem * operator = ( HSystem const & );
	/*}*/
	friend class HGalaxy;
	};

class HGalaxy
	{
	typedef void ( HGalaxy::*handler_t ) ( int, HString & );
	typedef HMap < HString, handler_t > handlers_t;
	typedef HMap < int, HString > names_t;
	typedef HArray < int > colors_t;
protected:
	/*{*/
	int f_iBoardSize;
	int f_iSystems;
	int f_iEmperors;
	int f_iRound;
	HArray < HSystem > f_oSystems;
	HSocket * f_poSocket;
	handlers_t f_oHandlers;
	names_t f_oNames;
	colors_t f_oColors;
	/*}*/
public:
	/*{*/
	HGalaxy ( int, int, int );
	virtual ~HGalaxy ( void );
	void set_socket ( HSocket * );
	void process_command ( int, HString & );
	/*}*/
protected:
	/*{*/
	void handler_login ( int, HString & );
	int assign_system ( int, int & );
	void broadcast ( HString & );
	void handler_message ( int, HString & );
	void handler_move ( int, HString & );
	/*}*/
private:
	/*{*/
	HGalaxy ( HGalaxy const & );
	HGalaxy & operator = ( HGalaxy const & );
	/*}*/
	friend class HServer;
	};

class HServer : public HProcess
	{
protected:
	/*{*/
	int f_iEmperors;
	HSocket f_oSocket;
	HGalaxy & f_roGalaxy;
	/*}*/
public:
	/*{*/
	HServer ( int, HGalaxy & );
	int init_server ( int );
	using HProcess::run;
	/*}*/
protected:
	/*{*/
	int handler_connection ( int );
	int handler_message ( int );
	/*}*/
	};

HSystem::HSystem ( void ) : f_iCoordinateX ( - 1 ), f_iCoordinateY ( - 1 ),
														f_iProduction ( - 1 ), f_iFleet ( - 1 ),
														f_iEmperor ( - 1 ), f_oAttackers ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HGalaxy::HGalaxy ( int a_iBoardSize, int a_iSystems, int a_iEmperors )
	: f_iBoardSize ( a_iBoardSize ), f_iSystems ( a_iSystems ),
	f_iEmperors ( a_iEmperors ), f_iRound ( 0 ),
	f_oSystems ( a_iSystems + a_iEmperors ), f_poSocket ( NULL ),
	f_oHandlers ( 16 ), f_oNames ( a_iEmperors ), f_oColors ( a_iEmperors )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCtrLoc = 0;
	HRandomizer l_oRandom;
	HSystem * l_poSystem = NULL;
	l_oRandom.set ( time ( NULL ) );
	for ( l_iCtr = 0; l_iCtr < ( a_iEmperors + a_iSystems ); l_iCtr ++ )
		{
		l_poSystem = & f_oSystems [ l_iCtr ];
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
	f_oHandlers [ "MOVE" ] = & HGalaxy::handler_login;
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

void HGalaxy::set_socket ( HSocket * a_poSocket )
	{
	M_PROLOG
	M_ASSERT ( a_poSocket );
	f_poSocket = a_poSocket;
	return;
	M_EPILOG
	}

void HGalaxy::process_command ( int a_iFileDescriptor, HString & a_roCommand )
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
		f_poSocket->shutdown_client ( a_iFileDescriptor );
	return;
	M_EPILOG
	}

void HGalaxy::handler_login ( int a_iFileDescriptor, HString & a_roName )
	{
	M_PROLOG
	int l_iCtr = 0, l_iSysNo = - 1, l_iEmperor = - 1;
	HSocket * l_poClient = NULL;
	HString l_oName;
	HString l_oMessage = "GLX:MSG:Emperor ";
	f_oNames [ a_iFileDescriptor ] = a_roName;
	l_oMessage += a_roName;
	l_oMessage += " invaded the galaxy.\n";
	broadcast ( l_oMessage );
	l_iSysNo = assign_system ( a_iFileDescriptor, l_iEmperor );
	l_poClient = f_poSocket->get_client ( a_iFileDescriptor );
	l_oMessage.format ( "GLX:SETUP:board_size=%d\n", f_iBoardSize );
	l_poClient->write_until_eos ( l_oMessage );
	l_oMessage.format ( "GLX:SETUP:systems=%d\n", f_iEmperors + f_iSystems );
	l_poClient->write_until_eos ( l_oMessage );
	for ( l_iCtr = 0; l_iCtr < f_iEmperors; l_iCtr ++ )
		{
		if ( ( f_oColors [ l_iCtr ] >= 0 )
				&& ( f_oNames.get ( f_oColors [ l_iCtr ], l_oName ) ) )
			{
			l_oMessage.format ( "GLX:SETUP:emperor=%d,%s\n",
					l_iCtr, static_cast < char * > ( l_oName ) );
			broadcast ( l_oMessage );
			}
		}
	for ( l_iCtr = 0; l_iCtr < ( f_iEmperors + f_iSystems ); l_iCtr ++ )
		{
		l_oMessage.format ( "GLX:SETUP:system_coordinates=%d,%d,%d\n",
				l_iCtr, f_oSystems [ l_iCtr ].f_iCoordinateX,
				f_oSystems [ l_iCtr ].f_iCoordinateY );
		l_poClient->write_until_eos ( l_oMessage );
		}
	f_oSystems [ l_iSysNo ].f_iProduction = 10;
	f_oSystems [ l_iSysNo ].f_iFleet = 10;
	l_oMessage.format ( "GLX:PLAY:system_info=%d,%d,%d,%d\n",
			l_iSysNo, f_oSystems [ l_iSysNo ].f_iProduction, l_iEmperor,
			f_oSystems [ l_iSysNo ].f_iFleet );
	l_poClient->write_until_eos ( l_oMessage );
	l_poClient->write_until_eos ( "GLX:SETUP:ok\n" );
	return;
	M_EPILOG
	}

int HGalaxy::assign_system ( int a_iFileDescriptor, int & a_riColor )
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

void HGalaxy::broadcast ( HString & a_roMessage )
	{
	M_PROLOG
	int l_iFileDescriptor = - 1;
	HSocket * l_poClient = NULL;
	f_poSocket->rewind_client_list ( );
	while ( f_poSocket->get_client_next ( l_iFileDescriptor, l_poClient ) )
		l_poClient->write_until_eos ( a_roMessage );
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
		l_oMessage = "GLX:MSG:";
		l_oMessage += l_oName;
		l_oMessage += ": ";
		l_oMessage += a_roMessage;
		l_oMessage += '\n';
		broadcast ( l_oMessage );
		}
	else
		f_poSocket->shutdown_client ( a_iFileDescriptor );
	return;
	M_EPILOG
	}

void HGalaxy::handler_move ( int a_iFileDescriptor, HString & a_roMove )
	{
	M_PROLOG
	int l_iSource = - 1, l_iDestination = - 1, l_iDX = 0, l_iDY = 0;
	HFleet l_oFleet;
	l_oFleet.f_iEmperor = a_iFileDescriptor;
	l_iSource = strtol ( a_roMove.split ( "'", 0 ), NULL, 10 );
	l_iDestination = strtol ( a_roMove.split ( "'", 1 ), NULL, 10 );
	l_oFleet.f_iSize = strtol ( a_roMove.split ( "'", 2 ), NULL, 10 );
	if ( ( l_iSource == l_iDestination )
			|| ( f_oSystems [ l_iSource ].f_iEmperor != a_iFileDescriptor )
			|| ( f_oSystems [ l_iSource ].f_iFleet < l_oFleet.f_iSize ) )
		f_poSocket->shutdown_client ( a_iFileDescriptor );
	else
		{
		l_iDX = f_oSystems [ l_iSource ].f_iCoordinateX - f_oSystems [ l_iDestination ].f_iCoordinateX;
		l_iDY = f_oSystems [ l_iSource ].f_iCoordinateY - f_oSystems [ l_iDestination ].f_iCoordinateY;
		l_oFleet.f_iArrivalTime = f_iRound + static_cast < int > ( sqrt ( l_iDX * l_iDX + l_iDY * l_iDY ) + 0.5 );
		f_oSystems [ l_iDestination ].f_oAttackers.add_head ( ) = l_oFleet;
		}
	return;
	M_EPILOG
	}

HServer::HServer ( int a_iPlayers, HGalaxy & a_roGalaxy )
	: HProcess ( a_iPlayers ), f_iEmperors ( a_iPlayers ),
	f_oSocket ( HSocket::D_DEFAULTS, a_iPlayers ), f_roGalaxy ( a_roGalaxy )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HServer::init_server ( int a_iPort )
	{
	M_PROLOG
	f_oSocket.listen ( "0.0.0.0", a_iPort );
	M_REGISTER_FILE_DESCRIPTOR_HANDLER ( f_oSocket.get_file_descriptor ( ), HServer::handler_connection );
	HProcess::init ( 3600 );
	f_roGalaxy.set_socket ( & f_oSocket );
	return ( 0 );
	M_EPILOG
	}

int HServer::handler_connection ( int )
	{
	M_PROLOG
	HSocket * l_poClient = f_oSocket.accept ( );
	M_ASSERT ( l_poClient );
	M_REGISTER_FILE_DESCRIPTOR_HANDLER ( l_poClient->get_file_descriptor ( ), HServer::handler_message );
	if ( f_oSocket.get_client_count ( ) >= f_iEmperors )
		{
		unregister_file_descriptor_handler ( f_oSocket.get_file_descriptor ( ) );
		f_oSocket.close ( );
		}
	fprintf ( stdout, "%s\n", static_cast < char * > ( l_poClient->get_host_name ( ) ) );
	return ( 0 );
	M_EPILOG
	}

int HServer::handler_message ( int a_iFileDescriptor )
	{
	M_PROLOG
	int l_iMsgLength = 0;
	HString l_oMessage;
	HString l_oArgument;
	HString l_oCommand;
	HSocket * l_poClient = f_oSocket.get_client ( a_iFileDescriptor );
	if ( l_poClient )
		{
		if ( ( l_iMsgLength = l_poClient->read_until ( l_oMessage ) ) > 0 )
			{
			fprintf ( stdout, "<-%s\n", static_cast < char * > ( l_oMessage ) );
			l_oCommand = l_oMessage.split ( ":", 0 );
			l_oArgument = l_oMessage.mid ( l_oCommand.get_length ( ) + 1 );
			l_iMsgLength = l_oArgument.get_length ( );
			if ( ( l_iMsgLength > 1 ) && ( l_oCommand == "GLX" ) )
				f_roGalaxy.process_command ( a_iFileDescriptor, l_oArgument );
			else if ( l_oCommand == "QUIT" )
				f_bLoop = false;
			}
		else if ( l_iMsgLength < 0 )
			{
			unregister_file_descriptor_handler ( a_iFileDescriptor );
			f_oSocket.shutdown_client ( a_iFileDescriptor );
			if ( ! f_oSocket.get_client_count ( ) )
				f_bLoop = false;
			}
		}
	else
		{
		unregister_file_descriptor_handler ( a_iFileDescriptor );
		if ( ! f_oSocket.get_client_count ( ) )
			f_bLoop = false;
		}
	return ( 0 );
	M_EPILOG
	}

}

int main_server ( void )
	{
	HGalaxy l_oGalaxy ( setup.f_iBoardSize, setup.f_iSystems, setup.f_iEmperors );
	HServer l_oServer ( setup.f_iEmperors, l_oGalaxy );
	l_oServer.init_server ( setup.f_iPort );
	l_oServer.run ( );
	return ( 0 );
	}

