/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	gameground.cxx - this file is integral part of `galaxy' project.

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

#include <iostream>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )

#include "galaxy.hxx"

#include "setup.hxx"

#define D_ATTR_BOARD	 				( COLORS::D_FG_CYAN | COLORS::D_BG_BLACK )
#define D_ATTR_NEUTRAL_SYSTEM	( COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK )
#define D_ATTR_CURSOR  				( COLORS::D_FG_WHITE | COLORS::D_BG_BLACK )

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground
{

namespace
{

char const * const n_ppcSystemNamesLatin [ ] =
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

char const * const n_ppcSystemNamesNorse [ ] =
	{
	"Aegir",
	"Balder",
	"C-Frey",
	"D-Tyr",
	"E-Frigg",
	"Freya",
	"Gullveig",
	"Hel",
	"Idun",
	"Jord",
	"Kvasir",
	"Loki",
	"Magni",
	"Njord",
	"Odin",
	"P-Forseti",
	"Q-Hod",
	"Ran",
	"Skadi",
	"Thor",
	"Ull",
	"Ve",
	"W-Vidar",
	"X-Sif",
	"Ymir",
	"Z-Heimdall",
	"Asgard",
	"Vanaheim",
	"Alfheim",
	"Jotunheim",
	"Hrimthursheim",
	"Muspellheim",
	"Midgard",
	"Svartalfheim",
	"Niflheim",
	"Yggdrasil"
	};

char const * const n_pcSymbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

char const * const * n_ppcSystemNames = NULL;

int n_piColors [ ] =
	{
	( COLORS::D_FG_BRIGHTBLUE | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_BRIGHTGREEN | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_BRIGHTRED | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_BRIGHTCYAN | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_BRIGHTMAGENTA | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_YELLOW | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_BLUE | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_GREEN | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_RED | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_CYAN | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_MAGENTA | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_BROWN | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK ),
	( COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK )
	};

class HBoard;
class HClient;
class HGalaxyWindow;

class HMove
	{
protected:
	/*{*/
	int f_iSourceSystem;
	int f_iDestinationSystem;
	int f_iFleet;
	/*}*/
public:
	/*{*/
	/*}*/
protected:
	/*{*/
	/*}*/
	friend class HGalaxyWindow;
	friend class HClient;
	};

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
typedef HHashMap < int, HString > emperors_t;
typedef HList < HMove > moves_t;
typedef enum
	{
	D_NORMAL,
	D_SELECT,
	D_INPUT,
	D_LOCKED
	} client_state_t;

class HEventListener
	{
protected:
	/*{*/
	int const & f_riRound;
	int const & f_riColor;
	client_state_t & f_reState;
	/*}*/
public:
	/*{*/
	HEventListener ( int const &, int const &, client_state_t & );
	virtual ~HEventListener ( void );
	virtual void on_show_system_info ( int ) = 0;
	virtual void make_move ( int, int ) = 0;
	int get_round ( void ) const;
	int get_color ( void ) const;
	client_state_t get_state ( void ) const;
	void set_state ( client_state_t );
	virtual void msg ( int, char const * const ) = 0;
	/*}*/
	};

class HBoard : public HControl
	{
protected:
	/*{*/
	int f_iCursorY;
	int f_iCursorX;
	int f_iBoardSize;
	int f_iSourceSystem;
	int f_iDestinationSystem;
	systems_t* f_poSystems;
	HEventListener& f_roListener;
	/*}*/
public:
	/*{*/
	HBoard( HWindow*, HEventListener& );
	virtual ~HBoard( void );
	void set_systems( systems_t* );
	/*}*/
protected:
	/*{*/
	int get_sys_no( int, int );
	int distance( int, int );
	virtual void do_refresh( void );
	virtual int do_process_input( int );
	virtual int do_click( mouse::OMouse& );
	/*}*/
private:
	/*{*/
	HBoard( HBoard const& );
	HBoard& operator = ( HBoard const& );
	/*}*/
	friend class HClient;
	friend class HGalaxyWindow;
	};

class HClient;
class HGalaxyWindow : public HWindow, public HEventListener
	{
protected:
	/*{*/
	HString f_oVarTmpBuffer;
	HBoard* f_poBoard;
	HEditControl* f_poSystemName;
	HEditControl* f_poEmperorName;
	HEditControl* f_poProduction;
	HEditControl* f_poFleet;
	HEditControl* f_poMessageInput;
	HLogPad* f_poLogPad;
	HClient* f_poClient;
	systems_t* f_poSystems;
	emperors_t* f_poEmperors;
	moves_t* f_poMoves;
	/*}*/
public:
	/*{*/
	typedef HPointer<HGalaxyWindow> ptr_t;
	HGalaxyWindow ( char const * const, HClient *, int &, int &, client_state_t & );
	virtual ~HGalaxyWindow ( void );
	virtual int init ( void );
	HBoard * get_board ( void );
	void set_data ( systems_t *, emperors_t *, moves_t * );
	/*}*/
protected:
	/*{*/
	int handler_enter( int, void const* );
	int handler_esc( int, void const* );
	int handler_space( int, void const* );
	virtual void on_show_system_info ( int );
	virtual void make_move ( int, int );
	virtual void msg ( int, char const * const );
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
	typedef HHashMap < HString, handler_t > handlers_t;
protected:
	/*{*/
	int f_iColor;
	int f_iRound;
	client_state_t f_eState;
	HSocket f_oSocket;
	HGalaxyWindow::ptr_t f_oWindow;
	HBoard * f_poBoard;
	systems_t f_oSystems;
	handlers_t f_oHandlers;
	emperors_t f_oEmperors;
	moves_t f_oMoves;
	/*}*/
public:
	/*{*/
	HClient ( char const * const );
	virtual ~HClient ( void );
	void init_client ( HString &, int );
	int handler_message ( int );
	void end_round ( void );
	void send_message ( HString const & );
	/*}*/
protected:
	/*{*/
	void process_command ( HString & );
	void handler_setup ( HString & );
	void handler_play ( HString & );
	void handler_msg ( HString & );
	void handler_error ( HString & );
	void handler_dummy( HString& );
	/*}*/
private:
	/*{*/
	HClient ( HClient const & );
	HClient & operator = ( HClient const & );
	/*}*/
	};

HSystem::HSystem ( void ) : f_iCoordinateX ( - 1 ), f_iCoordinateY ( - 1 ),
														f_iColor ( - 1 ),
														f_iProduction ( - 1 ), f_iFleet ( - 1 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HEventListener::HEventListener ( int const & a_riRound, int const & a_riColor,
		client_state_t & a_reState )
	: f_riRound ( a_riRound ), f_riColor ( a_riColor ), f_reState ( a_reState )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HEventListener::~HEventListener ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HEventListener::get_round ( void ) const
	{
	M_PROLOG
	return ( f_riRound );
	M_EPILOG
	}

int HEventListener::get_color ( void ) const
	{
	M_PROLOG
	return ( f_riColor );
	M_EPILOG
	}

client_state_t HEventListener::get_state ( void ) const
	{
	M_PROLOG
	return ( f_reState );
	M_EPILOG
	}

void HEventListener::set_state ( client_state_t a_eState )
	{
	M_PROLOG
	f_reState = a_eState;
	switch ( f_reState )
		{
		case ( D_LOCKED ):
			{
			msg ( COLORS::D_FG_WHITE, "A waiting for Galaxy events ..." );
			break;
			}
		case ( D_NORMAL ):
			{
			msg ( COLORS::D_FG_WHITE, "Make Your moves ..." );
			break;
			}
		case ( D_SELECT ):
			{
			msg ( COLORS::D_FG_WHITE, "Select destination for Your fleet ..." );
			break;
			}
		case ( D_INPUT ):
			{
			msg ( COLORS::D_FG_WHITE, "How many destroyers You wish to send ?" );
			break;
			}
		default :
			{
			break;
			}
		}
	return;
	M_EPILOG
	}

HBoard::HBoard ( HWindow * a_poParent, HEventListener & a_roListener )
	: HControl ( a_poParent, 0, 1, 0, 0, "&galaxy\n" ),
	f_iCursorY ( 0 ), f_iCursorX ( 0 ), f_iBoardSize ( - 1 ),
	f_iSourceSystem ( - 1 ), f_iDestinationSystem ( - 1 ),
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

void HBoard::do_refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0, l_iSystems = 0, l_iSysNo = - 1, l_iCoordX = - 1, l_iCoordY = - 1;
	int l_iColor = - 1, l_iRound = f_roListener.get_round ( );
	client_state_t l_eState = f_roListener.get_state ( );
	HString l_oPen;
	HBoard::draw_label ( );
	HConsole& cons = HCons::get_instance();
	if ( f_bFocused )
		cons.curs_set ( CURSOR::D_INVISIBLE );
	if ( f_iBoardSize >= 0 )
		{
		f_iHeight = f_iBoardSize + 1 /* for label */ + 2 /* for borders */;
		f_iWidth = f_iBoardSize * 3 /* for System */ + 2 /* for borders */;
		if ( f_iBoardSize < 12 )
			{
			cons.c_cmvprintf ( f_iRowRaw, f_iColumnRaw + 35, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, "{" );
			cons.c_cmvprintf ( f_iRowRaw + 1, f_iColumnRaw + 20, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, "-+--+--+--+--+-'" );
			}
		l_oPen = ',';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			l_oPen += "-+-";
		l_oPen += '.';
		cons.c_cmvprintf( f_iRowRaw, f_iColumnRaw, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, l_oPen.raw() );
		l_oPen = '}';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			l_oPen += " - ";
		l_oPen += '{';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			cons.c_cmvprintf ( f_iRowRaw + l_iCtr + 1, f_iColumnRaw, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, l_oPen.raw() );
		l_oPen = '`';
		for ( l_iCtr = 0; l_iCtr < f_iBoardSize; l_iCtr ++ )
			l_oPen += "-+-";
		l_oPen += '\'';
		cons.c_cmvprintf ( f_iRowRaw + f_iBoardSize + 1, f_iColumnRaw, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, l_oPen.raw() );
		if ( ( l_iSystems = static_cast<int>( f_poSystems->get_size() ) ) )
			{
			for ( l_iCtr = 0; l_iCtr < l_iSystems; l_iCtr ++ )
				{
				l_iCoordX = ( * f_poSystems ) [ l_iCtr ].f_iCoordinateX;
				l_iCoordY = ( * f_poSystems ) [ l_iCtr ].f_iCoordinateY;
				l_iColor = ( * f_poSystems ) [ l_iCtr ].f_iColor;
				cons.c_cmvprintf ( f_iRowRaw + 1 + l_iCoordY,
						f_iColumnRaw + 1 + l_iCoordX * 3,
						( ( l_iColor >= 0 ) && f_bFocused ) ? n_piColors [ l_iColor ] : D_ATTR_NEUTRAL_SYSTEM, "(%c)",
						l_iCtr + ( l_iCtr < 26 ? 'A' : ( l_iCtr < 35 ? '1' - 26 : '0' - 35 ) ) );
				if ( ( l_iCoordX == f_iCursorX ) && ( l_iCoordY == f_iCursorY ) )
					l_iSysNo = l_iCtr;
				}
			f_roListener.on_show_system_info ( l_iSysNo );
			}
		cons.c_cmvprintf ( f_iRowRaw + 1 + f_iCursorY,
				f_iColumnRaw + 1 + f_iCursorX * 3,
				f_bFocused ? D_ATTR_CURSOR : COLORS::D_ATTR_NORMAL, "{" );
		cons.c_cmvprintf ( f_iRowRaw + 1 + f_iCursorY,
				f_iColumnRaw + 3 + f_iCursorX * 3,
				f_bFocused ? D_ATTR_CURSOR : COLORS::D_ATTR_NORMAL, "}" );
		cons.c_cmvprintf ( f_iRowRaw - 1, f_iColumnRaw + 8, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, ",--{" );
		if ( l_iRound >= 0 )
			cons.c_cmvprintf ( f_iRowRaw - 1, f_iColumnRaw + 13, COLORS::D_ATTR_NORMAL, "%4d", l_iRound );
		cons.c_cmvprintf ( f_iRowRaw - 1, f_iColumnRaw + 17, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, "}--." );
		cons.c_cmvprintf ( f_iRowRaw - 1, f_iColumnRaw + 23, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, ",--{" );
		cons.c_cmvprintf ( f_iRowRaw - 1, f_iColumnRaw + 28, f_bFocused ? COLORS::D_FG_WHITE : COLORS::D_ATTR_NORMAL, "    " );
		if ( ( l_eState == D_SELECT ) || ( l_eState == D_INPUT ) )
			{
			l_iSysNo = get_sys_no ( f_iCursorX, f_iCursorY );
			if ( l_iSysNo >= 0 )
				{
				cons.c_cmvprintf ( f_iRowRaw - 1, f_iColumnRaw + 28,
						f_bFocused ? COLORS::D_FG_WHITE : COLORS::D_ATTR_NORMAL, "%4d",
						distance ( f_iSourceSystem, l_iSysNo ) + l_iRound );
				}
			}
		cons.c_cmvprintf ( f_iRowRaw - 1, f_iColumnRaw + 32, f_bFocused ? D_ATTR_BOARD : COLORS::D_ATTR_NORMAL, "}--." );
		}
	M_EPILOG
	}

int HBoard::do_process_input( int a_iCode )
	{
	M_PROLOG
	int l_iCode = 0, l_iSysNo = - 1;
	client_state_t l_eState = f_roListener.get_state ( );
	a_iCode = HControl::do_process_input ( a_iCode );
	if ( l_eState == D_LOCKED )
		return ( a_iCode == '\t' ? '\t' : '\r' );
	if ( f_iBoardSize >= 0 )
		{
		switch ( a_iCode )
			{
			case ( KEY_CODES::D_DOWN ):
				{
				if ( f_iCursorY < ( f_iBoardSize - 1 ) )
					f_iCursorY ++;
				else f_iCursorY = 0;
				break;
				}
			case ( KEY_CODES::D_UP ):
				{
				if ( f_iCursorY > 0 )
					f_iCursorY --;
				else f_iCursorY = f_iBoardSize - 1;
				break;
				}
			case ( KEY_CODES::D_LEFT ):
				{
				if ( f_iCursorX > 0 )
					f_iCursorX --;
				else f_iCursorX = f_iBoardSize - 1;
				break;
				}
			case ( KEY_CODES::D_RIGHT ):
				{
				if ( f_iCursorX < ( f_iBoardSize - 1 ) )
					f_iCursorX ++;
				else f_iCursorX = 0;
				break;
				}
			case ( KEY_CODES::D_HOME ):
				{
				f_iCursorX = 0;
				break;
				}
			case ( KEY_CODES::D_END ):
				{
				f_iCursorX = f_iBoardSize - 1;
				break;
				}
			case ( KEY_CODES::D_PAGE_UP ):
				{
				f_iCursorY = 0;
				break;
				}
			case ( KEY_CODES::D_PAGE_DOWN ):
				{
				f_iCursorY = f_iBoardSize - 1;
				break;
				}
			case ( '\r' ):
				{
				l_iSysNo = get_sys_no ( f_iCursorX, f_iCursorY );
				if ( l_iSysNo >= 0 )
					{
					if ( l_eState == D_NORMAL )
						{
						if ( ( * f_poSystems ) [ l_iSysNo ].f_iColor == f_roListener.get_color ( ) )
							{
							f_iSourceSystem = l_iSysNo;
							f_roListener.set_state ( D_SELECT );
							break;
							}
						}
					else if ( l_eState == D_SELECT )
						{
						f_iDestinationSystem = l_iSysNo;
						f_roListener.make_move ( f_iSourceSystem, l_iSysNo );
						return ( 0 );
						}
					}
				break;
				}
			default :
				{
				l_iCode = a_iCode;
				break;
				}
			}
		if ( ! l_iCode )
			{
			l_iSysNo = get_sys_no ( f_iCursorX, f_iCursorY );
			f_roListener.on_show_system_info( l_iSysNo );
			schedule_refresh();
			}
		}
	return ( l_iCode );
	M_EPILOG
	}

int HBoard::do_click( mouse::OMouse& a_rsMouse )
	{
	if ( ! HControl::do_click( a_rsMouse ) )
		return ( 1 );
	if ( ( a_rsMouse.f_iRow > 1 )
			&& ( a_rsMouse.f_iRow < ( f_iBoardSize + 2 ) )
			&& ( a_rsMouse.f_iColumn > 1 )
			&& ( a_rsMouse.f_iColumn < ( ( f_iBoardSize * 3 ) + 2 ) ) )
		{
		f_iCursorY = a_rsMouse.f_iRow - 2;
		f_iCursorX = ( a_rsMouse.f_iColumn - 2 ) / 3;
		f_roListener.on_show_system_info( get_sys_no ( f_iCursorX, f_iCursorY ) );
		schedule_refresh();
		}
	return ( 0 );
	}

void HBoard::set_systems ( systems_t * a_poSystems )
	{
	f_poSystems = a_poSystems;
	}

int HBoard::get_sys_no ( int a_iCoordX, int a_iCoordY )
	{
	M_PROLOG
	int l_iCtr = 0, l_iSystems = static_cast<int>( f_poSystems->get_size() );
	for ( l_iCtr = 0; l_iCtr < l_iSystems; l_iCtr ++ )
		if ( ( ( * f_poSystems ) [ l_iCtr ].f_iCoordinateX == a_iCoordX )
				&& ( ( * f_poSystems ) [ l_iCtr ].f_iCoordinateY == a_iCoordY ) )
			break;
	return ( l_iCtr < l_iSystems ? l_iCtr : - 1 );
	M_EPILOG
	}

int HBoard::distance ( int a_iSource, int a_iDestination )
	{
	M_PROLOG
	int l_iDX = 0, l_iDY = 0, l_iDistance = 0;
	if ( a_iSource != a_iDestination )
		{
		l_iDX = ( * f_poSystems ) [ a_iSource ].f_iCoordinateX - ( * f_poSystems ) [ a_iDestination ].f_iCoordinateX;
		l_iDY = ( * f_poSystems ) [ a_iSource ].f_iCoordinateY - ( * f_poSystems ) [ a_iDestination ].f_iCoordinateY;
		l_iDX = ( l_iDX >= 0 ) ? l_iDX : - l_iDX;
		l_iDY = ( l_iDY >= 0 ) ? l_iDY : - l_iDY;
		l_iDX = ( ( f_iBoardSize - l_iDX ) < l_iDX ) ? f_iBoardSize - l_iDX : l_iDX;
		l_iDY = ( ( f_iBoardSize - l_iDY ) < l_iDY ) ? f_iBoardSize - l_iDY : l_iDY;
		l_iDistance = static_cast < int > ( sqrt ( l_iDX * l_iDX + l_iDY * l_iDY ) + 0.5 );
		}
	return ( l_iDistance );
	M_EPILOG
	}

HGalaxyWindow::HGalaxyWindow ( char const * const a_pcWindowTitle,
		HClient * a_poClient,
		int & a_riRound, int & a_riColor, client_state_t & a_reState )
	: HWindow ( a_pcWindowTitle ),
	HEventListener ( a_riRound, a_riColor, a_reState ),
	f_oVarTmpBuffer ( ),
	f_poBoard ( NULL ), f_poSystemName ( NULL ), f_poEmperorName ( NULL ),
	f_poProduction ( NULL ), f_poFleet ( NULL ), f_poMessageInput ( NULL ),
	f_poLogPad ( NULL ),
	f_poClient ( a_poClient ),
	f_poSystems ( NULL ), f_poEmperors ( NULL ), f_poMoves ( NULL )
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
	f_poBoard->enable ( true );
	f_poBoard->set_focus();
	f_poBoard->set_systems ( f_poSystems );
	f_poSystemName = new HEditControl ( this, 1, 64, 1, 16, " System name \n", 64, "", n_pcMaskExtended );
	f_poEmperorName = new HEditControl ( this, 4, 64, 1, 16, " Emperor \n", 64, "", n_pcMaskExtended );
	f_poProduction = new HEditControl ( this, 7, 64, 1, 7, "Product\n", 6, "", n_pcMaskDigits );
	f_poFleet = new HEditControl ( this, 7, 72, 1, 7, "Fleet\n", 6, "", n_pcMaskDigits );
	f_poLogPad = new HLogPad ( this, 10, 64, - 5, - 1, " Event &log \n" );
	f_poMessageInput = new HEditControl ( this, - 4, 64, 1, - 1, " &Message \n", 255, "", n_pcMaskLoose );
	f_poLogPad->enable ( true );
	f_poMessageInput->enable ( true );
	register_postprocess_handler ( '\r', NULL, &HGalaxyWindow::handler_enter );
	register_postprocess_handler ( KEY_CODES::D_ESC, NULL, &HGalaxyWindow::handler_esc );
	register_postprocess_handler ( ' ', NULL, &HGalaxyWindow::handler_space );
	return ( 0 );
	M_EPILOG
	}

HBoard * HGalaxyWindow::get_board ( void )
	{
	return ( f_poBoard );
	}

void HGalaxyWindow::set_data ( systems_t * a_poSystems,
		emperors_t * a_poEmperors, moves_t * a_poMoves )
	{
	f_poSystems = a_poSystems;
	f_poEmperors = a_poEmperors;
	f_poMoves = a_poMoves;
	return;
	}

void HGalaxyWindow::on_show_system_info ( int a_iSystem )
	{
	M_PROLOG
	int l_iColor = - 1;
	HString l_oName;
	f_poSystemName->set ( "" );
	f_poEmperorName->set ( "" );
	f_poProduction->set ( "" );
	f_poFleet->set ( "" );
	if ( a_iSystem >= 0 )
		{
		f_poSystemName->set ( n_ppcSystemNames [ a_iSystem ] );
		if ( ( * f_poSystems ) [ a_iSystem ].f_iProduction >= 0 )
			f_poProduction->set ( HString ( ( * f_poSystems ) [ a_iSystem ].f_iProduction ) );
		if ( ( * f_poSystems ) [ a_iSystem ].f_iFleet >= 0 )
			f_poFleet->set ( HString ( ( * f_poSystems ) [ a_iSystem ].f_iFleet ) );
		l_iColor = ( * f_poSystems ) [ a_iSystem ].f_iColor;
		if ( ( l_iColor >= 0 ) && f_poEmperors->get ( l_iColor, l_oName ) )
			f_poEmperorName->set ( l_oName );
		}
	return;
	M_EPILOG
	}

void HGalaxyWindow::make_move ( int a_iSourceSystem, int a_iDestinationSystem )
	{
	M_PROLOG
	if ( a_iSourceSystem != a_iDestinationSystem )
		{
		set_state ( D_INPUT );
		f_poBoard->enable ( false );
		f_poLogPad->enable ( false );
		f_poFleet->enable ( true );
		f_poFleet->set_focus ( );
		}
	else
		handler_esc ( 0, NULL );
	return;
	M_EPILOG
	}

int HGalaxyWindow::handler_enter( int a_iCode, void const* )
	{
	M_PROLOG
	int l_iFleet = 0;
	HMove * l_poMove = NULL;
	if ( (*f_oFocusedChild) == f_poMessageInput )
		{
		if ( f_poMessageInput->get().get<HString const&>().find_other_than( n_pcWhiteSpace ) >= 0 )
			{
			f_oVarTmpBuffer = "cmd:glx:say:";
			f_oVarTmpBuffer += f_poMessageInput->get ( ).get<char const*>();
			f_oVarTmpBuffer += "\n";
			f_poClient->send_message ( f_oVarTmpBuffer );
			f_poMessageInput->set ( "" );
			}
		a_iCode = 0;
		}
	else if ( f_reState == D_INPUT )
		{
		if ( (*f_oFocusedChild) == f_poFleet )
			{
			l_iFleet = lexical_cast<int>( f_poFleet->get().get<char const*>() );
			if ( ( l_iFleet > 0 )
					&& ( l_iFleet <= ( * f_poSystems ) [ f_poBoard->f_iSourceSystem ].f_iFleet ) )
				{
				f_poFleet->enable ( false );
				f_poBoard->enable ( true );
				f_poLogPad->enable ( true );
				f_poBoard->set_focus ( );
				l_poMove = & f_poMoves->add_tail ( );
				l_poMove->f_iSourceSystem = f_poBoard->f_iSourceSystem;
				l_poMove->f_iDestinationSystem = f_poBoard->f_iDestinationSystem;
				l_poMove->f_iFleet = l_iFleet;
				( * f_poSystems ) [ f_poBoard->f_iSourceSystem ].f_iFleet -= l_iFleet;
				set_state ( D_NORMAL );
				}
			else if ( l_iFleet > 0 )
				f_oStatusBar->message ( COLORS::D_FG_RED, "Not enough resources!" );
			else
				f_oStatusBar->message ( COLORS::D_FG_RED, "Run! Run! Emperor is mad!" );
			a_iCode = 0;
			}
		}
	else if ( f_reState == D_LOCKED )
		{
		f_oStatusBar->message ( COLORS::D_FG_RED, f_riRound >= 0 ? "Wait for new round!" : "Challange not started yet!" );
		a_iCode = 0;
		}
	return ( a_iCode );
	M_EPILOG
	}

int HGalaxyWindow::handler_esc( int, void const* )
	{
	M_PROLOG
	if ( f_reState == D_INPUT )
		{
		f_poFleet->enable ( false );
		f_poBoard->enable ( true );
		f_poLogPad->enable ( true );
		f_poBoard->set_focus ( );
		set_state( D_SELECT );
		}
	else if ( f_reState == D_SELECT )
		set_state( D_NORMAL );
	schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

int HGalaxyWindow::handler_space( int, void const* )
	{
	M_PROLOG
	if ( f_reState == D_NORMAL )
		f_poClient->end_round ( );
	return ( 0 );
	M_EPILOG
	}

void HGalaxyWindow::msg ( int a_iAttr, char const * const a_pcMsg )
	{
	M_PROLOG
	f_oStatusBar->message ( a_iAttr, a_pcMsg );
	M_EPILOG
	}

HClient::HClient ( char const * const a_pcProgramName )
	: f_iColor ( - 1 ), f_iRound ( - 1 ), f_eState ( D_LOCKED ), f_oSocket ( ),
	f_oWindow ( new HGalaxyWindow( a_pcProgramName, this, f_iRound, f_iColor, f_eState ) ),
	f_poBoard ( NULL ), f_oSystems ( 0 ), f_oHandlers ( 16 ), f_oEmperors ( 16 ),
	f_oMoves ( )
	{
	M_PROLOG
	f_oHandlers [ "setup" ] = &HClient::handler_setup;
	f_oHandlers [ "play" ] = &HClient::handler_play;
	f_oHandlers [ "msg" ] = &HClient::handler_msg;
	f_oHandlers [ "err" ] = &HClient::handler_error;
	f_oHandlers [ "kck" ] = &HClient::handler_error;
	f_oHandlers [ "player" ] = &HClient::handler_dummy;
	f_oHandlers [ "player_quit" ] = &HClient::handler_dummy;
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
	register_file_descriptor_handler ( f_oSocket.get_file_descriptor ( ), & HClient::handler_message );
	l_oMessage = "name:";
	l_oMessage += setup.f_oLogin + '\n';
	f_oSocket.write_until_eos ( l_oMessage );
	f_oWindow->set_data ( & f_oSystems, & f_oEmperors, & f_oMoves );
	HTUIProcess::init_tui ( "galaxy", f_oWindow );
	f_poBoard = f_oWindow->get_board();
	if ( ! setup.f_oGameType.is_empty() )
		l_oMessage.format( "create:glx:%s,%d,%d,%d\n",
				setup.f_oGame.raw(),
				setup.f_iEmperors, setup.f_iBoardSize, setup.f_iSystems );
	else
		l_oMessage.format( "join:%s\n", setup.f_oGame.raw() );
	f_oSocket.write_until_eos( l_oMessage );
	return;
	M_EPILOG
	}

int HClient::handler_message ( int )
	{
	M_PROLOG
	int l_iMsgLength = 0;
	HString l_oMessage;
	HString l_oCommand;
	if ( ( l_iMsgLength = f_oSocket.read_until( l_oMessage ).octets ) > 0 )
		{
		while ( ( l_oCommand = l_oMessage.split( ":", 0 ) ) == "glx" )
			l_oMessage = l_oMessage.mid( l_oCommand.get_length() + 1 );
		l_iMsgLength = static_cast<int>( l_oMessage.get_length() );
		if ( l_iMsgLength < 1 )
			{
			hcore::log << "got empty message from server" << endl;
			f_bLoop = false;
			}
		else
			process_command( l_oMessage );
		}
	else if ( l_iMsgLength < 0 )
		f_bLoop = false;
	refresh();
	return ( 0 );
	M_EPILOG
	}

void HClient::process_command( HString& a_roCommand )
	{
	M_PROLOG
	HString l_oMnemonic;
	HString l_oArgument;
	handler_t HANDLER;
	if ( setup.f_iVerbose > 2 )
		hcore::log << "msg: " << a_roCommand << endl;
	l_oMnemonic = a_roCommand.split ( ":", 0 );
	l_oArgument = a_roCommand.mid( l_oMnemonic.get_length() + 1 );
	if ( f_oHandlers.get( l_oMnemonic, HANDLER ) )
		( this->*HANDLER )( l_oArgument );
	else
		{
		hcore::log << "unknown mnemonic: " << l_oMnemonic << ", (the argument: " << l_oArgument << ")" << endl;
		f_bLoop = false;
		}
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
		f_oWindow->f_poBoard->f_iBoardSize = lexical_cast<int>( l_oValue );
	else if ( l_oVariable == "systems" )
		{
		if ( f_oSystems.get_size ( ) )
			f_bLoop = false;
		else
			f_oSystems = HArray < HSystem > ( setup.f_iSystems = lexical_cast<int>( l_oValue ) );
		}
	else if ( l_oVariable == "system_coordinates" )
		{
		l_iIndex = lexical_cast<int>( l_oValue.split ( ",", 0 ) );
		l_iCoordX = lexical_cast<int>( l_oValue.split ( ",", 1 ) );
		l_iCoordY = lexical_cast<int>( l_oValue.split ( ",", 2 ) );
		f_oSystems [ l_iIndex ].f_iCoordinateX = l_iCoordX;
		f_oSystems [ l_iIndex ].f_iCoordinateY = l_iCoordY;
		}
	else if ( l_oVariable == "emperor" )
		{
		l_iIndex = lexical_cast<int>( l_oValue.split ( ",", 0 ) );
		f_oEmperors [ l_iIndex ] = l_oValue.split ( ",", 1 );
		if ( f_oEmperors [ l_iIndex ] == setup.f_oLogin )
			f_iColor = l_iIndex;
		}
	else if ( l_oVariable == "ok" )
		{
		f_oWindow->schedule_refresh ( );
		f_oWindow->set_state ( D_NORMAL );
		f_iRound = 0;
		}
	return;
	M_EPILOG
	}

void HClient::handler_play ( HString& a_roCommand )
	{
	M_PROLOG
	char l_cEvent = 0;
	int l_iSysNo = - 1, l_iColor = 0, l_iProduction = - 1;
	HString l_oVariable;
	HString l_oValue;
	l_oVariable = a_roCommand.split ( "=", 0 );
	l_oValue = a_roCommand.split ( "=", 1 );
	if ( l_oVariable == "system_info" )
		{
		l_cEvent = l_oValue [ 0 ];
		l_iSysNo = lexical_cast<int>( l_oValue.split( ",", 1 ) );
		l_iProduction = lexical_cast<int>( l_oValue.split( ",", 2 ) );
		if ( l_iProduction >= 0 )
			f_oSystems [ l_iSysNo ].f_iProduction = l_iProduction;
		f_oSystems [ l_iSysNo ].f_iFleet = lexical_cast<int>( l_oValue.split( ",", 4 ) );
		l_iColor = lexical_cast<int>( l_oValue.split( ",", 3 ) );
		f_oEmperors.get( l_iColor, l_oValue );
		switch ( l_cEvent )
			{
			case ( 'c' ): /* conquered */
			case ( 'd' ): /* defeted */
				{
				f_oWindow->f_poLogPad->add( n_piColors [ l_iColor ] );
				f_oWindow->f_poLogPad->add( l_oValue );
				f_oWindow->f_poLogPad->add( COLORS::D_ATTR_NORMAL );
				f_oWindow->f_poLogPad->add( " conquered " );
				int l_iTemp = f_oSystems [ l_iSysNo ].f_iColor;
				f_oWindow->f_poLogPad->add( ( l_iTemp >= 0 ) ? n_piColors [ l_iTemp ] : COLORS::D_ATTR_NORMAL );
				f_oWindow->f_poLogPad->add( n_ppcSystemNames [ l_iSysNo ] );
				l_oValue.format ( "(%c)", n_pcSymbols [ l_iSysNo ] );
				f_oWindow->f_poLogPad->add( l_oValue );
				f_oWindow->f_poLogPad->add( COLORS::D_ATTR_NORMAL );
				f_oWindow->f_poLogPad->add( ".\n" );
				f_oSystems [ l_iSysNo ].f_iColor = l_iColor;
				break;
				}
			case ( 'r' ): /* reinforcements */
				{
				f_oWindow->f_poLogPad->add( "Reinforcements for " );
				f_oWindow->f_poLogPad->add( n_piColors[ l_iColor ] );
				f_oWindow->f_poLogPad->add( n_ppcSystemNames[ l_iSysNo ] );
				l_oValue.format( "(%c)", n_pcSymbols[ l_iSysNo ] );
				f_oWindow->f_poLogPad->add( l_oValue );
				f_oWindow->f_poLogPad->add( COLORS::D_ATTR_NORMAL );
				f_oWindow->f_poLogPad->add( " arrived.\n" );
				break;
				}
			case ( 'f' ): /* failed to conquer */
				{
				f_oSystems [ l_iSysNo ].f_iColor = l_iColor;
				l_iColor = f_iColor;
				f_oEmperors.get ( l_iColor, l_oValue );
				}
			case ( 's' ): /* resisted attack */
				{
				int l_iTemp = f_oSystems [ l_iSysNo ].f_iColor;
				f_oWindow->f_poLogPad->add ( ( l_iTemp >= 0 ) ? n_piColors [ l_iTemp ] : COLORS::D_ATTR_NORMAL );
				f_oWindow->f_poLogPad->add ( n_ppcSystemNames [ l_iSysNo ] );
				l_oVariable.format ( "(%c)", n_pcSymbols [ l_iSysNo ] );
				f_oWindow->f_poLogPad->add ( l_oVariable );
				f_oWindow->f_poLogPad->add ( COLORS::D_ATTR_NORMAL );
				f_oWindow->f_poLogPad->add ( " resisted attack from " );
				f_oWindow->f_poLogPad->add ( n_piColors [ l_iColor ] );
				f_oWindow->f_poLogPad->add ( l_oValue );
				f_oWindow->f_poLogPad->add ( COLORS::D_ATTR_NORMAL );
				f_oWindow->f_poLogPad->add ( ".\n" );
				break;
				}
			case ( 'i' ): /* info */
			default :
				{
				break;
				}
			}
		}
	else if ( l_oVariable == "round" )
		{
		f_oWindow->f_poLogPad->add( D_ATTR_CURSOR );
		f_oWindow->f_poLogPad->add( "----- " );
		f_oWindow->f_poLogPad->add( COLORS::D_ATTR_NORMAL );
		f_oWindow->f_poLogPad->add( " round: " );
		f_iRound = lexical_cast<int>( l_oValue );
		l_oValue.format ( "%d", f_iRound );
		f_oWindow->f_poLogPad->add( D_ATTR_CURSOR );
		f_oWindow->f_poLogPad->add( l_oValue );
		f_oWindow->f_poLogPad->add( " -----\n" );
		f_oWindow->f_poLogPad->add( COLORS::D_ATTR_NORMAL );
		f_oWindow->set_state( D_NORMAL );
		f_oWindow->schedule_refresh();
		}
	return;
	M_EPILOG
	}

void HClient::handler_error( HString& a_roMessage )
	{
	M_PROLOG
	hcore::log << "error: " << a_roMessage << endl;
	f_bLoop = false;
	return;
	M_EPILOG
	}

void HClient::handler_msg( HString& a_roMessage )
	{
	M_PROLOG
	int l_iIndex = 0, l_iOffset = 0;
	int l_iLength = static_cast<int>( a_roMessage.get_length() );
	HString l_oPart;
	while ( l_iIndex < l_iLength )
		{
		l_iOffset = static_cast<int>( a_roMessage.find( ';', l_iIndex ) );
		if ( l_iOffset < 0 )
			l_iOffset = l_iLength;
		l_oPart = a_roMessage.mid( l_iIndex, l_iOffset - l_iIndex );
		if ( l_oPart.is_empty ( ) )
			break;
		if ( l_oPart [ 0 ] == '$' ) /* color */
			f_oWindow->f_poLogPad->add( n_piColors[ lexical_cast<int> ( l_oPart.mid ( 1 ) ) ] );
		else /* text */
			f_oWindow->f_poLogPad->add( l_oPart );
		l_iIndex = l_iOffset + 1;
		}
	f_oWindow->f_poLogPad->add ( "\n" );
	f_oWindow->f_poLogPad->add ( COLORS::D_ATTR_NORMAL );
	return;
	M_EPILOG
	}

void HClient::end_round ( void )
	{
	M_PROLOG
	HString l_oMessage;
	f_oWindow->set_state ( D_LOCKED );
	if ( f_oMoves.size ( ) )
		{
		for ( moves_t::iterator it = f_oMoves.begin(); it != f_oMoves.end(); ++ it )
			{
			l_oMessage.format ( "cmd:glx:play:move=%d,%d,%d\n",
					it->f_iSourceSystem,
					it->f_iDestinationSystem,
					it->f_iFleet );
			f_oSocket.write_until_eos ( l_oMessage );
			}
		f_oMoves.clear();
		}
	f_oSocket.write_until_eos ( "cmd:glx:play:end_round\n" );
	return;
	M_EPILOG
	}

void HClient::send_message ( HString const & a_oMessage )
	{
	M_PROLOG
	f_oSocket.write_until_eos ( a_oMessage );
	return;
	M_EPILOG
	}

void HClient::handler_dummy( HString& )
	{
	}

}

int main_client ( void )
	{
	if ( HTime ( ).get_hour ( ) % 2 )
		n_ppcSystemNames = n_ppcSystemNamesNorse;
	else
		n_ppcSystemNames = n_ppcSystemNamesLatin;
	M_ENSURE( setup.f_oGameType.is_empty() || ( setup.f_oGameType == "glx" ) );
	HClient l_oClient( setup.f_oLogin.raw() );
	l_oClient.init_client( setup.f_oHost, setup.f_iPort );
	l_oClient.run ( );
	return ( 0 );
	}

}

