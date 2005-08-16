/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	galaxy.c - this file is integral part of `galaxy' project.

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

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "galaxy.h"

#include "setup.h"

#define D_ATTR_BOARD	 ( D_FG_CYAN | D_BG_BLACK )
#define D_ATTR_CURSOR  ( D_FG_WHITE | D_BG_BLACK )

using namespace std;
using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::tools;
using namespace stdhapi::tools::util;

class HBoard;
class HSystem
	{
protected:
	/*{*/
	/*}*/
public:
	/*{*/
	/*}*/
protected:
	/*{*/
	/*}*/
	friend class HBoard;
	};

class HBoard : public HControl
	{
protected:
	/*{*/
	int f_iCursorY;
	int f_iCursorX;
	/*}*/
public:
	/*{*/
	HBoard ( HWindow * );
	virtual ~HBoard ( void );
	virtual void refresh ( void );
	virtual int process_input ( int );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

class HGalaxyWindow : public HWindow
	{
protected:
	/*{*/
	HBoard * f_poBoard;
	/*}*/
public:
	/*{*/
	HGalaxyWindow ( char const * const );
	virtual ~HGalaxyWindow ( void );
	virtual int init ( void );
	HBoard * get_board ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	HGalaxyWindow ( HGalaxyWindow const & );
	HGalaxyWindow & operator = ( HGalaxyWindow const & );
	/*}*/
	};

class HClient : public HTUIProcess
	{
protected:
	/*{*/
	HSocket f_oSocket;
	HGalaxyWindow f_oWindow;
	HBoard * f_poBoard;
	/*}*/
public:
	/*{*/
	HClient ( char const * const );
	virtual ~HClient ( void );
	void init_client ( HString &, int );
	int handler_message ( int );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	HClient ( HClient const & );
	HClient & operator = ( HClient const & );
	/*}*/
	};

HBoard::HBoard ( HWindow * a_poParent )
	: HControl ( a_poParent, 1, 1,
			setup.f_iBoardSize + 1 /* for label */ + 2 /* for borders */,
			setup.f_iBoardSize * 3 /* for System */ + 2 /* for borders */ ,
			"&galaxy\n" ), f_iCursorY ( 0 ), f_iCursorX ( 0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HBoard::~HBoard ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HBoard::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	HString l_oPen;
	HBoard::draw_label ( );
	if ( f_bFocused )
		M_IRV ( curs_set ( D_CURSOR_INVISIBLE ) );
	l_oPen = ',';
	for ( l_iCtr = 0; l_iCtr < setup.f_iBoardSize; l_iCtr ++ )
		l_oPen += "-+-";
	l_oPen += '.';
	hconsole::c_printf ( f_iRowRaw, f_iColumnRaw, D_ATTR_BOARD, l_oPen );
	l_oPen = '}';
	for ( l_iCtr = 0; l_iCtr < setup.f_iBoardSize; l_iCtr ++ )
		l_oPen += " - ";
	l_oPen += '{';
	for ( l_iCtr = 0; l_iCtr < setup.f_iBoardSize; l_iCtr ++ )
		hconsole::c_printf ( f_iRowRaw + l_iCtr + 1, f_iColumnRaw, D_ATTR_BOARD, l_oPen );
	l_oPen = '`';
	for ( l_iCtr = 0; l_iCtr < setup.f_iBoardSize; l_iCtr ++ )
		l_oPen += "-+-";
	l_oPen += '\'';
	hconsole::c_printf ( f_iRowRaw + setup.f_iBoardSize + 1, f_iColumnRaw, D_ATTR_BOARD, l_oPen );
	hconsole::c_printf ( f_iRowRaw + 1 + f_iCursorY, f_iColumnRaw + 1 + f_iCursorX * 3, D_ATTR_CURSOR, "{" );
	hconsole::c_printf ( f_iRowRaw + 1 + f_iCursorY, f_iColumnRaw + 3 + f_iCursorX * 3, D_ATTR_CURSOR, "}" );
	M_EPILOG
	}

int HBoard::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCode = 0;
	a_iCode = HControl::process_input ( a_iCode );
	switch ( a_iCode )
		{
		case ( KEY_DOWN ):
			{
			if ( f_iCursorY < ( setup.f_iBoardSize - 1 ) )
				f_iCursorY ++;
			break;
			}
		case ( KEY_UP ):
			{
			if ( f_iCursorY > 0 )
				f_iCursorY --;
			break;
			}
		case ( KEY_LEFT ):
			{
			if ( f_iCursorX > 0 )
				f_iCursorX --;
			break;
			}
		case ( KEY_RIGHT ):
			{
			if ( f_iCursorX < ( setup.f_iBoardSize - 1 ) )
				f_iCursorX ++;
			break;
			}
		default :
			{
			l_iCode = a_iCode;
			break;
			}
		}
	if ( ! l_iCode )
		refresh ( );
	return ( l_iCode );
	M_EPILOG
	}

HGalaxyWindow::HGalaxyWindow ( char const * const a_pcWindowTitle )
	: HWindow ( a_pcWindowTitle ), f_poBoard ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HGalaxyWindow::~HGalaxyWindow ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HGalaxyWindow::init ( void )
	{
	M_PROLOG
	HWindow::init ( );
	f_poBoard = new HBoard ( this );
	f_poBoard->enable ( true );
	f_poBoard->set_focus ( );
	return ( 0 );
	M_EPILOG
	}

HBoard * HGalaxyWindow::get_board ( void )
	{
	return ( f_poBoard );
	}

HClient::HClient ( char const * const a_pcProgramName )
	: f_oSocket ( ), f_oWindow ( a_pcProgramName ), f_poBoard ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HClient::~HClient ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HClient::init_client ( HString & a_roHost, int a_iPort )
	{
	M_PROLOG
	HString l_oMessage;
	f_oSocket.connect ( a_roHost, a_iPort );
	M_REGISTER_FILE_DESCRIPTOR_HANDLER ( f_oSocket.get_file_descriptor ( ), HClient::handler_message );
	l_oMessage = "GLX:LOGIN:";
	l_oMessage += setup.f_oLogin + '\n';
	f_oSocket.write_until_eos ( l_oMessage );
	f_poBoard = f_oWindow.get_board ( );
	HTUIProcess::init_tui ( "galaxy", & f_oWindow );
	return;
	M_EPILOG
	}

int HClient::handler_message ( int )
	{
	M_PROLOG
	int l_iMsgLength = 0;
	HString l_oMessage;
	HString l_oArgument;
	HString l_oCommand;
	if ( ( l_iMsgLength = f_oSocket.read_until ( l_oMessage ) ) > 0 )
		{
//		fprintf ( stdout, "<-%s\n", static_cast < char * > ( l_oMessage ) );
		l_oCommand = l_oMessage.split ( ":", 0 );
		l_oArgument = l_oMessage.split ( ":", 1 );
		l_iMsgLength = l_oArgument.get_length ( );
		if ( l_iMsgLength > 1 )
			{
			if ( l_oCommand == "SAY" )
				{
				l_oMessage = "MSG:" + l_oArgument + '\n';
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
//		fprintf ( stdout, "->%s", static_cast < char * > ( l_oMessage ) );
		}
	else if ( l_iMsgLength < 0 )
		f_bLoop = false;
	return ( 0 );
	M_EPILOG
	}

int main_client ( void )
	{
	HClient l_oClient ( setup.f_oLogin );
	l_oClient.init_client ( setup.f_oHost, setup.f_iPort );
	l_oClient.run ( );
	return ( 0 );
	}

