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

#define D_ATTR_BOARD	 				( D_FG_CYAN | D_BG_BLACK )
#define D_ATTR_NEUTRAL_SYSTEM	( D_FG_LIGHTGRAY | D_BG_BLACK )
#define D_ATTR_CURSOR  				( D_FG_WHITE | D_BG_BLACK )

using namespace std;
using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::tools;
using namespace stdhapi::tools::util;

namespace
{

char const * const n_pcSystemNames [ ] =
	{
	"Aldebaran",
	"Betelgeuse",
	"Canis Major",
	"Deneb",
	"Eridanus",
	"Fomalhaut",
	"Gemini",
	"Hydra",
	"Izar",
	"Jabhat al Akrab",
	"Kochab",
	"Lupus",
	"Monoceros",
	"Norma",
	"Orion",
	"Procyon",
	"Quantum",
	"Reticulum",
	"Sirius",
	"Taurus",
	"Ursa Minor",
	"Vega",
	"Warrior",
	"Xerkses",
	"Yarn",
	"Zubenelgenubi",
	"Primum",
	"Secundum",
	"Tertium",
	"Quartum",
	"Quintum",
	"Sextum",
	"Septimum",
	"Octavum",
	"Nonum",
	"Decimum"
	};

int n_piColors [ ] =
	{
	( D_FG_BRIGHTBLUE | D_BG_BLACK ),
	( D_FG_BRIGHTGREEN | D_BG_BLACK ),
	( D_FG_BRIGHTRED | D_BG_BLACK ),
	( D_FG_BRIGHTCYAN | D_BG_BLACK ),
	( D_FG_BRIGHTMAGENTA | D_BG_BLACK ),
	( D_FG_YELLOW | D_BG_BLACK ),
	( D_FG_BLUE | D_BG_BLACK ),
	( D_FG_GREEN | D_BG_BLACK ),
	( D_FG_RED | D_BG_BLACK ),
	( D_FG_CYAN | D_BG_BLACK ),
	( D_FG_MAGENTA | D_BG_BLACK ),
	( D_FG_BROWN | D_BG_BLACK )
	};

class HLogPad : public HControl
	{
	class HLogLine
		{
		typedef enum
			{
			D_NONE,
			D_ATTRIBUTE,
			D_TEXT,
			D_TEXT_EOL
			} type_t;
	protected:
		/*{*/
		type_t f_eType;
		int f_iAttribute;
		HString f_oText;
		/*}*/
	public:
		/*{*/
		HLogLine ( void );
		virtual ~HLogLine ( void );
		/*}*/
	protected:
		/*{*/
		/*}*/
		friend class HLogPad;
		};
protected:
	typedef HList < HLogLine > contents_t;
	/*{*/
	int f_iOffsetRow;
	int f_iOffsetColumn;
	int f_iAttribute;
	contents_t f_oContents;
	HString f_oVarTmpBuffer;
	/*}*/
public:
	/*{*/
	HLogPad ( HWindow *, int, int, int, int, char const * const );
	virtual ~HLogPad ( void );
	void add ( int, HString & );
	void add ( HString & );
	void add ( int );
	/*}*/
protected:
	/*{*/
	virtual void refresh ( void );
	/*}*/
	};

class HBoard;
class HClient;
class HGalaxyWindow;
class HSystem
	{
protected:
	/*{*/
	int f_iCoordinateX;
	int f_iCoordinateY;
	int f_iColor;
	int f_iProduction;
	int f_iFleet;
	/*}*/
public:
	/*{*/
	HSystem ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	friend class HBoard;
	friend class HClient;
	friend class HGalaxyWindow;
	};

typedef HArray < HSystem > systems_t;
typedef HMap < int, HString > emperors_t;

class HEventListener
	{
public:
	/*{*/
	virtual void on_show_system_info ( int ) = 0;
	/*}*/
	};

class HClient;
class HBoard : public HControl
	{
protected:
	/*{*/
	int f_iCursorY;
	int f_iCursorX;
	int f_iBoardSize;
	systems_t * f_poSystems;
	HEventListener & f_roListener;
	/*}*/
public:
	/*{*/
	HBoard ( HWindow *, HEventListener & );
	virtual ~HBoard ( void );
	virtual void refresh ( void );
	virtual int process_input ( int );
	void set_systems ( systems_t * );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	HBoard ( HBoard const & );
	HBoard & operator = ( HBoard const & );
	/*}*/
	friend class HClient;
	};

class HClient;
class HGalaxyWindow : public HWindow, public HEventListener
	{
protected:
	/*{*/
	HBoard * f_poBoard;
	HEditControl * f_poSystemName;
	HEditControl * f_poEmperorName;
	HEditControl * f_poProduction;
	HEditControl * f_poFleet;
	HLogPad * f_poLogPad;
	systems_t * f_poSystems;
	emperors_t * f_poEmperors;
	/*}*/
public:
	/*{*/
	HGalaxyWindow ( char const * const );
	virtual ~HGalaxyWindow ( void );
	virtual int init ( void );
	HBoard * get_board ( void );
	void set_data ( systems_t *, emperors_t * );
	/*}*/
protected:
	/*{*/
	virtual void on_show_system_info ( int );
	/*}*/
private:
	/*{*/
	HGalaxyWindow ( HGalaxyWindow const & );
	HGalaxyWindow & operator = ( HGalaxyWindow const & );
	/*}*/
	friend class HClient;
	};

class HClient : public HTUIProcess
	{
	typedef void ( HClient::*handler_t ) ( HString & );
	typedef HMap < HString, handler_t > handlers_t;
protected:
	/*{*/
	HSocket f_oSocket;
	HGalaxyWindow f_oWindow;
	HBoard * f_poBoard;
	systems_t f_oSystems;
	handlers_t f_oHandlers;
	emperors_t f_oEmperors;
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
	void process_command ( HString & );
	void handler_setup ( HString & );
	void handler_play ( HString & );
	void handler_msg ( HString & );
	/*}*/
private:
	/*{*/
	HClient ( HClient const & );
	HClient & operator = ( HClient const & );
	/*}*/
	};

HLogPad::HLogLine::HLogLine ( void )
	: f_eType ( D_NONE ), f_iAttribute ( - 1 ), f_oText ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HLogPad::HLogLine::~HLogLine ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HLogPad::HLogPad ( HWindow * a_poParent, int a_iRow, int a_iColumn,
		int a_iHeight, int a_iWidth, char const * const a_pcLabel )
	: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth, a_pcLabel ),
	f_iOffsetRow ( 0 ), f_iOffsetColumn ( 0 ), f_iAttribute ( 0 ),
	f_oContents ( ), f_oVarTmpBuffer ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HLogPad::~HLogPad ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HLogPad::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iColumn = 0;
	HLogLine * l_poLogLine;
	draw_label ( );
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	memset ( static_cast < char * > ( f_oVarTmpBuffer ), ' ', f_iWidthRaw );
	f_iAttribute = M_ATTR_DATA ( );
	for ( l_iCtr = 0; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		c_printf ( f_iRowRaw + l_iCtr, f_iColumnRaw, f_iAttribute, f_oVarTmpBuffer );
	if ( f_oContents.quantity ( ) )
		{
		l_poLogLine = & f_oContents.go ( f_iOffsetRow );
		for ( l_iCtr = 0;	l_poLogLine && ( l_iCtr < f_iHeightRaw ); )
			{
			if ( l_poLogLine->f_eType == HLogLine::D_ATTRIBUTE )
				f_iAttribute = l_poLogLine->f_iAttribute;
			else
				{
				c_printf ( f_iRowRaw + l_iCtr, f_iColumnRaw + l_iColumn, f_iAttribute, l_poLogLine->f_oText );
				if ( l_poLogLine->f_eType == HLogLine::D_TEXT_EOL )
					{
					l_iColumn = 0;
					l_iCtr ++;
					}
				else
					l_iColumn += l_poLogLine->f_oText.get_length ( );
				}
			l_poLogLine = f_oContents.to_tail ( 1, D_TREAT_AS_OPENED );
			}
		}
	return;
	M_EPILOG
	}

void HLogPad::add ( int a_iAttribute )
	{
	M_PROLOG
	HLogLine l_oLogLine;
	l_oLogLine.f_eType = HLogLine::D_ATTRIBUTE;
	l_oLogLine.f_iAttribute = a_iAttribute;
	f_oContents.add_tail ( ) = l_oLogLine;
	return;
	M_EPILOG
	}

void HLogPad::add ( HString & a_roText )
	{
	M_PROLOG
	int l_iIndexNL = 0, l_iIndexChar = 0;
	HLogLine l_oLogLine;
	HLogLine * l_poLogLine = NULL;
	if ( f_oContents.quantity ( ) )
		l_poLogLine = & f_oContents.tail ( );
	if ( ! l_poLogLine || ( l_poLogLine->f_eType != HLogLine::D_TEXT ) )
		{
		l_poLogLine = & l_oLogLine;
		l_poLogLine->f_eType = HLogLine::D_TEXT;
		l_poLogLine->f_oText = "";
		}
	f_oVarTmpBuffer = a_roText;
	while ( static_cast < char * > ( f_oVarTmpBuffer ) [ 0 ] )
		{
		l_iIndexNL = f_oVarTmpBuffer.find_one_of ( "\r\n" );
		if ( l_iIndexNL >= 0 )
			{
			l_poLogLine->f_oText += f_oVarTmpBuffer.left ( l_iIndexNL );
			l_poLogLine->f_eType = HLogLine::D_TEXT_EOL;
			l_iIndexChar = f_oVarTmpBuffer.find_other_than ( "\r\n", l_iIndexNL + 1 );
			if ( l_iIndexChar >= 0 )
				f_oVarTmpBuffer = f_oVarTmpBuffer.mid ( l_iIndexChar );
			else
				f_oVarTmpBuffer = "";
			}
		else
			{
			l_poLogLine->f_oText += f_oVarTmpBuffer;
			f_oVarTmpBuffer = "";
			}
		if ( l_poLogLine == & l_oLogLine )
			f_oContents.add_tail ( l_poLogLine );
		l_poLogLine = & l_oLogLine;
		l_poLogLine->f_eType = HLogLine::D_TEXT;
		l_poLogLine->f_oText = "";
		}
	if ( ( l_iIndexNL = f_oContents.quantity ( ) ) > f_iHeightRaw )
		f_iOffsetRow = l_iIndexNL - f_iHeightRaw;
	n_bNeedRepaint = true;
	refresh ( );
	return;
	M_EPILOG
	}

void HLogPad::add ( int a_iAttribute, HString & a_roText )
	{
	M_PROLOG
	add ( a_iAttribute );
	add ( a_roText );
	return;
	M_EPILOG
	}

HSystem::HSystem ( void ) : f_iCoordinateX ( - 1 ), f_iCoordinateY ( - 1 ),
														f_iColor ( - 1 ),
														f_iProduction ( - 1 ), f_iFleet ( - 1 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HBoard::HBoard ( HWindow * a_poParent, HEventListener & a_roListener )
	: HControl ( a_poParent, 0, 1, 0, 0, "&galaxy\n" ),
	f_iCursorY ( 0 ), f_iCursorX ( 0 ), f_iBoardSize ( - 1 ),
	f_poSystems ( NULL ), f_roListener ( a_roListener )
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
	int l_iCtr = 0, l_iSystems = 0, l_iSysNo = - 1, l_iCoordX = - 1, l_iCoordY = - 1;
	int l_iColor = - 1;
	HString l_oPen;
	HBoard::draw_label ( );
	if ( f_bFocused )
		M_IRV ( curs_set ( D_CURSOR_INVISIBLE ) );
	if ( f_iBoardSize >= 0 )
		{
		f_iHeight = f_iBoardSize + 1 /* for label */ + 2 /* for borders */;
		f_iWidth = f_iBoardSize * 3 /* for System */ + 2 /* for borders */;
		l_oPen = ',';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			l_oPen += "-+-";
		l_oPen += '.';
		hconsole::c_printf ( f_iRowRaw, f_iColumnRaw, D_ATTR_BOARD, l_oPen );
		l_oPen = '}';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			l_oPen += " - ";
		l_oPen += '{';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			hconsole::c_printf ( f_iRowRaw + l_iCtr + 1, f_iColumnRaw, D_ATTR_BOARD, l_oPen );
		l_oPen = '`';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			l_oPen += "-+-";
		l_oPen += '\'';
		hconsole::c_printf ( f_iRowRaw + f_iBoardSize + 1, f_iColumnRaw, D_ATTR_BOARD, l_oPen );
		if ( ( l_iSystems = f_poSystems->get_size ( ) ) )
			{
			for ( l_iCtr = 0; l_iCtr < l_iSystems; l_iCtr ++ )
				{
				l_iCoordX = ( * f_poSystems ) [ l_iCtr ].f_iCoordinateX;
				l_iCoordY = ( * f_poSystems ) [ l_iCtr ].f_iCoordinateY;
				l_iColor = ( * f_poSystems ) [ l_iCtr ].f_iColor;
				hconsole::c_printf ( f_iRowRaw + 1 + l_iCoordY,
						f_iColumnRaw + 1 + l_iCoordX * 3,
						l_iColor >= 0 ? n_piColors [ l_iColor ] : D_ATTR_NEUTRAL_SYSTEM, "(%c)",
						l_iCtr + ( l_iCtr < 26 ? 'A' : ( l_iCtr < 35 ? '1' - 26 : '0' - 35 ) ) );
				if ( ( l_iCoordX == f_iCursorX ) && ( l_iCoordY == f_iCursorY ) )
					l_iSysNo = l_iCtr;
				}
			f_roListener.on_show_system_info ( l_iSysNo );
			}
		hconsole::c_printf ( f_iRowRaw + 1 + f_iCursorY, f_iColumnRaw + 1 + f_iCursorX * 3, D_ATTR_CURSOR, "{" );
		hconsole::c_printf ( f_iRowRaw + 1 + f_iCursorY, f_iColumnRaw + 3 + f_iCursorX * 3, D_ATTR_CURSOR, "}" );
		}
	M_EPILOG
	}

int HBoard::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCode = 0;
	a_iCode = HControl::process_input ( a_iCode );
	if ( f_iBoardSize >= 0 )
		{
		switch ( a_iCode )
			{
			case ( KEY_DOWN ):
				{
				if ( f_iCursorY < ( f_iBoardSize - 1 ) )
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
				if ( f_iCursorX < ( f_iBoardSize - 1 ) )
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
		}
	return ( l_iCode );
	M_EPILOG
	}

void HBoard::set_systems ( systems_t * a_poSystems )
	{
	f_poSystems = a_poSystems;
	}

HGalaxyWindow::HGalaxyWindow ( char const * const a_pcWindowTitle )
	: HWindow ( a_pcWindowTitle ), f_poBoard ( NULL ),
	f_poSystemName ( NULL ), f_poEmperorName ( NULL ),
	f_poProduction ( NULL ), f_poFleet ( NULL ), f_poLogPad ( NULL ),
	f_poSystems ( NULL ), f_poEmperors ( NULL )
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
	f_poBoard = new HBoard ( this, * this );
	f_poBoard->set_systems ( f_poSystems );
	f_poBoard->enable ( true );
	f_poBoard->set_focus ( );
	f_poSystemName = new HEditControl ( this, 1, 64, 1, 16, " System name \n", 64, "", D_MASK_EXTENDED );
	f_poEmperorName = new HEditControl ( this, 4, 64, 1, 16, " Emperor \n", 64, "", D_MASK_EXTENDED );
	f_poProduction = new HEditControl ( this, 7, 64, 1, 7, "Product\n", 6, "", D_MASK_DIGITS );
	f_poFleet = new HEditControl ( this, 7, 72, 1, 7, "Fleet\n", 6, "", D_MASK_DIGITS );
	f_poLogPad = new HLogPad ( this, 10, 64, - 3, - 1, " Event log \n" );
	return ( 0 );
	M_EPILOG
	}

HBoard * HGalaxyWindow::get_board ( void )
	{
	return ( f_poBoard );
	}

void HGalaxyWindow::set_data ( systems_t * a_poSystems, emperors_t * a_poEmperors )
	{
	f_poSystems = a_poSystems;
	f_poEmperors = a_poEmperors;
	return;
	}

void HGalaxyWindow::on_show_system_info ( int a_iSystem )
	{
	int l_iColor = - 1;
	HString l_oName;
	if ( a_iSystem >= 0 )
		{
		f_poSystemName->set ( n_pcSystemNames [ a_iSystem ] );
		if ( ( * f_poSystems ) [ a_iSystem ].f_iProduction >= 0 )
			f_poProduction->set ( HString ( ( * f_poSystems ) [ a_iSystem ].f_iProduction ) );
		if ( ( * f_poSystems ) [ a_iSystem ].f_iFleet >= 0 )
			f_poFleet->set ( HString ( ( * f_poSystems ) [ a_iSystem ].f_iFleet ) );
		l_iColor = ( * f_poSystems ) [ a_iSystem ].f_iColor;
		if ( ( l_iColor >= 0 ) && f_poEmperors->get ( l_iColor, l_oName ) )
			f_poEmperorName->set ( l_oName );
		}
	else
		{
		f_poSystemName->set ( "" );
		f_poEmperorName->set ( "" );
		f_poProduction->set ( "" );
		f_poFleet->set ( "" );
		}
	}

HClient::HClient ( char const * const a_pcProgramName )
	: f_oSocket ( ), f_oWindow ( a_pcProgramName ), f_poBoard ( NULL ),
	f_oSystems ( 0 ), f_oHandlers ( 16 ), f_oEmperors ( 16 )
	{
	M_PROLOG
	f_oHandlers [ "SETUP" ] = & HClient::handler_setup;
	f_oHandlers [ "PLAY" ] = & HClient::handler_play;
	f_oHandlers [ "MSG" ] = & HClient::handler_msg;
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
	f_oWindow.set_data ( & f_oSystems, & f_oEmperors );
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
		l_oCommand = l_oMessage.split ( ":", 0 );
		l_oCommand = l_oMessage.split ( ":", 0 );
		l_oArgument = l_oMessage.mid ( l_oCommand.get_length ( ) + 1 );
		l_iMsgLength = l_oArgument.get_length ( );
		if ( ( l_iMsgLength > 1 ) && ( l_oCommand == "GLX" ) )
			process_command ( l_oArgument );
		else
			f_bLoop = false;
		}
	else if ( l_iMsgLength < 0 )
		f_bLoop = false;
	return ( 0 );
	M_EPILOG
	}

void HClient::process_command ( HString & a_roCommand )
	{
	M_PROLOG
	HString l_oMnemonic;
	HString l_oArgument;
	handler_t HANDLER;
	l_oMnemonic = a_roCommand.split ( ":", 0 );
	l_oArgument = a_roCommand.mid ( l_oMnemonic.get_length ( ) + 1 );
	if ( f_oHandlers.get ( l_oMnemonic, HANDLER ) )
		( this->*HANDLER ) ( l_oArgument );
	else
		f_bLoop = false;
	return;
	M_EPILOG
	}

void HClient::handler_setup ( HString & a_roCommand )
	{
	M_PROLOG
	int l_iIndex = - 1, l_iCoordX = - 1, l_iCoordY = - 1;
	HString l_oVariable;
	HString l_oValue;
	l_oVariable = a_roCommand.split ( "=", 0 );
	l_oValue = a_roCommand.split ( "=", 1 );
	if ( l_oVariable == "board_size" )
		f_oWindow.f_poBoard->f_iBoardSize = strtol ( l_oValue, NULL, 10 );
	else if ( l_oVariable == "systems" )
		{
		if ( f_oSystems.get_size ( ) )
			f_bLoop = false;
		else
			f_oSystems = HArray < HSystem > ( setup.f_iSystems = strtol ( l_oValue, NULL, 10 ) );
		}
	else if ( l_oVariable == "system_coordinates" )
		{
		l_iIndex = strtol ( l_oValue.split ( ",", 0 ), NULL, 10 );
		l_iCoordX = strtol ( l_oValue.split ( ",", 1 ), NULL, 10 );
		l_iCoordY = strtol ( l_oValue.split ( ",", 2 ), NULL, 10 );
		f_oSystems [ l_iIndex ].f_iCoordinateX = l_iCoordX;
		f_oSystems [ l_iIndex ].f_iCoordinateY = l_iCoordY;
		}
	else if ( l_oVariable == "emperor" )
		{
		l_iIndex = strtol ( l_oValue.split ( ",", 0 ), NULL, 10 );
		f_oEmperors [ l_iIndex ] = l_oValue.split ( ",", 1 );
		}
	else if ( l_oVariable == "ok" )
		f_oWindow.refresh ( );
	return;
	M_EPILOG
	}

void HClient::handler_play ( HString & a_roCommand )
	{
	M_PROLOG
	int l_iSysNo = - 1;
	HString l_oVariable;
	HString l_oValue;
	l_oVariable = a_roCommand.split ( "=", 0 );
	l_oValue = a_roCommand.split ( "=", 1 );
	if ( l_oVariable == "system_info" )
		{
		l_iSysNo = strtol ( l_oValue.split ( ",", 0 ), NULL, 10 );
		f_oSystems [ l_iSysNo ].f_iProduction = strtol ( l_oValue.split ( ",", 1 ), NULL, 10 );
		f_oSystems [ l_iSysNo ].f_iColor = strtol ( l_oValue.split ( ",", 2 ), NULL, 10 );
		f_oSystems [ l_iSysNo ].f_iFleet = strtol ( l_oValue.split ( ",", 3 ), NULL, 10 );
		}
	return;
	M_EPILOG
	}

void HClient::handler_msg ( HString & a_roMessage )
	{
	M_PROLOG
	a_roMessage += '\n';
	f_oWindow.f_poLogPad->add ( a_roMessage );
	return;
	M_EPILOG
	}

}

int main_client ( void )
	{
	HClient l_oClient ( setup.f_oLogin );
	l_oClient.init_client ( setup.f_oHost, setup.f_iPort );
	l_oClient.run ( );
	return ( 0 );
	}

