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

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "galaxyd.h"

#include "setup.h"

using namespace std;
using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::tools;
using namespace stdhapi::tools::util;

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
	};

class HSystem
	{
protected:
	/*{*/
	int f_iCoordinatesX;
	int f_iCoordinatesY;
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
	};

class HGalaxy
	{
protected:
	/*{*/
	int f_iBoardSize;
	int f_iSystems;
	int f_iEmperors;
	int f_iRound;
	HArray < HSystem > f_oSystems;
	/*}*/
public:
	/*{*/
	HGalaxy ( int, int, int );
	virtual ~HGalaxy ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

class HServer : public HProcess
	{
protected:
	/*{*/
	int f_iEmperors;
	HSocket f_oSocket;
	HGalaxy & f_oGalaxy;
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

HSystem::HSystem ( void ) : f_iCoordinatesX ( - 1 ), f_iCoordinatesY ( - 1 ),
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
	f_oSystems ( a_iSystems + a_iEmperors )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HGalaxy::~HGalaxy ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HServer::HServer ( int a_iPlayers, HGalaxy & a_roGalaxy )
	: HProcess ( a_iPlayers ), f_iEmperors ( a_iPlayers ),
	f_oSocket ( HSocket::D_DEFAULTS, a_iPlayers ), f_oGalaxy ( a_roGalaxy )
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
	int l_iFileDescriptor = - 1;
	int l_iMsgLength = 0;
	HString l_oMessage;
	HString l_oArgument;
	HString l_oCommand;
	HSocket * l_poClient = f_oSocket.get_client ( a_iFileDescriptor );
	M_ASSERT ( l_poClient );
	if ( ( l_iMsgLength = l_poClient->read_until ( l_oMessage ) ) > 0 )
		{
		fprintf ( stdout, "<-%s\n", static_cast < char * > ( l_oMessage ) );
		l_oCommand = l_oMessage.split ( ":", 0 );
		l_oArgument = l_oMessage.split ( ":", 1 );
		l_iMsgLength = l_oArgument.get_length ( );
		if ( l_iMsgLength > 1 )
			{
			if ( l_oCommand == "SAY" )
				{
				l_oMessage = "MSG:" + l_oArgument + '\n';
				f_oSocket.rewind_client_list ( );
				while ( f_oSocket.get_client_next ( l_iFileDescriptor, l_poClient ) )
					if ( l_iFileDescriptor != a_iFileDescriptor )
						l_poClient->write_until_eos ( l_oMessage );
				}
			else if ( l_oCommand == "LOGIN" )
				{
				}
			}
		else
			{
			if ( l_oCommand == "QUIT" )
				f_bLoop = false;
			}
		fprintf ( stdout, "->%s", static_cast < char * > ( l_oMessage ) );
		}
	else if ( l_iMsgLength < 0 )
		{
		unregister_file_descriptor_handler ( a_iFileDescriptor );
		f_oSocket.shutdown_client ( a_iFileDescriptor );
		if ( ! f_oSocket.get_client_count ( ) )
			f_bLoop = false;
		}
	return ( 0 );
	M_EPILOG
	}

int main_server ( void )
	{
	HGalaxy l_oGalaxy ( setup.f_iBoardSize, setup.f_iSystems, setup.f_iEmperors );
	HServer l_oServer ( setup.f_iEmperors, l_oGalaxy );
	l_oServer.init_server ( setup.f_iPort );
	l_oServer.run ( );
	return ( 0 );
	}

