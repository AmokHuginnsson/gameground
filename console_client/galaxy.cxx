/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	gameground.cxx - this file is integral part of `galaxy' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cmath>
#include <iostream>

#include <yaal/hcore/harray.hxx>
#include <yaal/hcore/hhashmap.hxx>
#include <yaal/hcore/hsocket.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/hcore/hlog.hxx>
#include <yaal/tools/hiodispatcher.hxx>
#include <yaal/tools/hstringstream.hxx>
#include <yaal/tools/hash.hxx>
#include <yaal/tools/util.hxx>
#include <yaal/hconsole/console.hxx>
#include <yaal/hconsole/htuiprocess.hxx>
#include <yaal/hconsole/hwindow.hxx>
#include <yaal/hconsole/heditwidget.hxx>
#include <yaal/hconsole/hlogpad.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "galaxy.hxx"

#include "setup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

COLOR::color_t ATTR_BOARD( COLOR::combine( COLOR::FG_CYAN, COLOR::BG_BLACK ) );
COLOR::color_t ATTR_NEUTRAL_SYSTEM( COLOR::combine( COLOR::FG_LIGHTGRAY, COLOR::BG_BLACK ) );
COLOR::color_t ATTR_CURSOR( COLOR::combine( COLOR::FG_WHITE, COLOR::BG_BLACK ) );

namespace gameground {

namespace {

char const * const _systemNamesLatin_[ ] = {
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

char const * const _systemNamesNorse_[ ] = {
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

char const _symbols_[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

char const* const* _systemNames_ = nullptr;

COLOR::color_t _colors_[] = {
	COLOR::combine( COLOR::FG_BRIGHTBLUE, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_BRIGHTGREEN, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_BRIGHTRED, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_BRIGHTCYAN, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_BRIGHTMAGENTA, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_YELLOW, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_BLUE, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_GREEN, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_RED, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_CYAN, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_MAGENTA, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_BROWN, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_LIGHTGRAY, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_LIGHTGRAY, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_LIGHTGRAY, COLOR::BG_BLACK ),
	COLOR::combine( COLOR::FG_LIGHTGRAY, COLOR::BG_BLACK )
};

class HBoard;
class HClient;
class HGalaxyWindow;

class HMove {
protected:
	/*{*/
	int _sourceSystem;
	int _destinationSystem;
	int _fleet;
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

class HSystem {
protected:
	/*{*/
	int _coordinateX;
	int _coordinateY;
	int _color;
	int _production;
	int _fleet;
	/*}*/
public:
	/*{*/
	HSystem( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	friend class HBoard;
	friend class HClient;
	friend class HGalaxyWindow;
};

typedef HArray<HSystem> systems_t;
typedef HHashMap<int, HString> emperors_t;
typedef HList<HMove> moves_t;
typedef enum {
	NORMAL,
	SELECT,
	INPUT,
	LOCKED
} client_state_t;

class HEventListener {
protected:
	/*{*/
	int const& _round;
	int const& _color;
	client_state_t& _state;
	/*}*/
public:
	/*{*/
	HEventListener( int const&, int const&, client_state_t& );
	virtual ~HEventListener( void );
	virtual void on_show_system_info( int ) = 0;
	virtual void make_move( int, int ) = 0;
	int get_round( void ) const;
	int get_color( void ) const;
	client_state_t get_state( void ) const;
	void set_state( client_state_t );
	virtual void msg( int, char const* const ) = 0;
	/*}*/
};

class HBoard : public HWidget {
protected:
	/*{*/
	int _cursorY;
	int _cursorX;
	int _boardSize;
	int _sourceSystem;
	int _destinationSystem;
	systems_t* _systems;
	HEventListener& _listener;
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
	virtual void do_paint( void );
	virtual int do_process_input( int );
	virtual bool do_click( mouse::OMouse& );
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
class HGalaxyWindow : public HWindow, public HEventListener {
protected:
	/*{*/
	HString _varTmpBuffer;
	HBoard* _board;
	HEditWidget* _systemName;
	HEditWidget* _emperorName;
	HEditWidget* _production;
	HEditWidget* _fleet;
	HEditWidget* _messageInput;
	HLogPad* _logPad;
	HClient* _client;
	systems_t* _systems;
	emperors_t* _emperors;
	moves_t* _moves;
	/*}*/
public:
	/*{*/
	typedef HPointer<HGalaxyWindow> ptr_t;
	HGalaxyWindow( HString const&, HClient*, int&, int&, client_state_t& );
	virtual ~HGalaxyWindow( void );
	virtual void do_init( void ) override;
	HBoard * get_board( void );
	void set_data( systems_t *, emperors_t *, moves_t * );
	/*}*/
protected:
	/*{*/
	bool handler_enter( hconsole::HEvent const& );
	bool handler_esc( hconsole::HEvent const& );
	bool handler_space( hconsole::HEvent const& );
	virtual void on_show_system_info( int );
	virtual void make_move( int, int );
	virtual void msg( int, char const * const );
	/*}*/
private:
	/*{*/
	HGalaxyWindow( HGalaxyWindow const & );
	HGalaxyWindow & operator = ( HGalaxyWindow const & );
	/*}*/
	friend class HClient;
};

class HClient : public HTUIProcess {
	typedef void ( HClient::*handler_t ) ( HString& );
	typedef HHashMap<HString, handler_t> handlers_t;
protected:
	/*{*/
	HString _id;
	int _color;
	int _round;
	client_state_t _state;
	HSocket::ptr_t _socket;
	HGalaxyWindow::ptr_t _window;
	HBoard* _board;
	systems_t _systems;
	handlers_t _handlers;
	emperors_t _emperors;
	moves_t _moves;
	yaal::tools::HStringStream _out;
	static HString CLIENT_VERSION;
	/*}*/
public:
	/*{*/
	HClient( HString const& );
	virtual ~HClient( void );
	void init_client( HString&, int );
	void handler_message( yaal::tools::HIODispatcher::stream_t& );
	void end_round( void );
	void send_message( HString const& );
	HString const& id( void ) const;
	/*}*/
protected:
	/*{*/
	void process_command( HString& );
	void handler_setup( HString& );
	void handler_play( HString& );
	void handler_msg( HString& );
	void handler_error( HString& );
/* cppcheck-suppress functionConst */
	void handler_dummy( HString& );
	void handler_party( HString& );
	void handler_party_info( HString& );
	/*}*/
private:
	/*{*/
	HClient( HClient const& );
	HClient& operator = ( HClient const& );
	/*}*/
};

HString HClient::CLIENT_VERSION = "3";

HSystem::HSystem( void )
	: _coordinateX( -1 )
	, _coordinateY( -1 )
	, _color( -1 )
	, _production( -1 )
	, _fleet( -1 ) {
	M_PROLOG
	return;
	M_EPILOG
}

HEventListener::HEventListener( int const& round_, int const& color_,
		client_state_t & state_ )
	: _round( round_ ), _color( color_ ), _state( state_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HEventListener::~HEventListener( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HEventListener::get_round( void ) const {
	M_PROLOG
	return ( _round );
	M_EPILOG
}

int HEventListener::get_color( void ) const {
	M_PROLOG
	return ( _color );
	M_EPILOG
}

client_state_t HEventListener::get_state( void ) const {
	M_PROLOG
	return ( _state );
	M_EPILOG
}

void HEventListener::set_state( client_state_t state_ ) {
	M_PROLOG
	_state = state_;
	switch ( _state ) {
		case ( LOCKED ): {
			msg( COLOR::FG_WHITE, "A waiting for Galaxy events ..." );
			break;
		}
		case ( NORMAL ): {
			msg( COLOR::FG_WHITE, "Make Your moves ..." );
			break;
		}
		case ( SELECT ): {
			msg( COLOR::FG_WHITE, "Select destination for Your fleet ..." );
			break;
		}
		case ( INPUT ): {
			msg( COLOR::FG_WHITE, "How many destroyers You wish to send ?" );
			break;
		}
		default : {
			break;
		}
	}
	return;
	M_EPILOG
}

HBoard::HBoard( HWindow * parent_, HEventListener & listener_ )
	: HWidget( parent_, 0, 1, 0, 0, "&galaxy\n" )
	, _cursorY ( 0 )
	, _cursorX ( 0 )
	, _boardSize( -1 )
	, _sourceSystem( -1 )
	, _destinationSystem( -1 )
	, _systems( nullptr )
	, _listener( listener_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HBoard::~HBoard( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HBoard::do_paint( void ) {
	M_PROLOG
	int round( _listener.get_round() );
	client_state_t state( _listener.get_state() );
	HString pen;
	HBoard::draw_label();
	HConsole& cons = HConsole::get_instance();
	if ( _focused )
		cons.curs_set( CURSOR::INVISIBLE );
	if ( _boardSize >= 0 ) {
		_height = _boardSize + 1 /* for label */ + 2 /* for borders */;
		_width = _boardSize * 3 /* for System */ + 2 /* for borders */;
		if ( _boardSize < 12 ) {
			cons.cmvprintf( _rowRaw, _columnRaw + 35, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, "{" );
			cons.cmvprintf( _rowRaw + 1, _columnRaw + 20, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, "-+--+--+--+--+-'" );
		}
		pen = ',';
		for ( int ctr( 0 ); ctr < _boardSize; ctr ++ )
			pen += "-+-";
		pen += '.';
		cons.cmvprintf( _rowRaw, _columnRaw, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, pen );
		pen = '}';
		for ( int ctr( 0 ); ctr < _boardSize; ctr ++ )
			pen += " - ";
		pen += '{';
		for ( int ctr( 0 ); ctr < _boardSize; ctr ++ )
			cons.cmvprintf( _rowRaw + ctr + 1, _columnRaw, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, pen );
		pen = '`';
		for ( int ctr( 0 ); ctr < _boardSize; ctr ++ )
			pen += "-+-";
		pen += '\'';
		cons.cmvprintf( _rowRaw + _boardSize + 1, _columnRaw, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, pen );
		int systems( static_cast<int>( _systems->get_size() ) );
		if ( systems > 0 ) {
			int sysNo( -1 );
			for ( int ctr( 0 ); ctr < systems; ctr ++ ) {
				int coordX( ( * _systems )[ ctr ]._coordinateX );
				int coordY( ( * _systems )[ ctr ]._coordinateY );
				int color( ( * _systems )[ ctr ]._color );
				cons.cmvprintf(
					_rowRaw + 1 + coordY,
					_columnRaw + 1 + coordX * 3,
					( ( color >= 0 ) && _focused ) ? _colors_[ color ] : ATTR_NEUTRAL_SYSTEM, "(%c)",
					static_cast<char>( ctr + ( ctr < 26 ? 'A' : ( ctr < 35 ? '1' - 26 : '0' - 35 ) ) )
				);
				if ( ( coordX == _cursorX ) && ( coordY == _cursorY ) ) {
					sysNo = ctr;
				}
			}
			_listener.on_show_system_info( sysNo );
		}
		cons.cmvprintf( _rowRaw + 1 + _cursorY,
				_columnRaw + 1 + _cursorX * 3,
				_focused ? ATTR_CURSOR : COLOR::ATTR_NORMAL, "{" );
		cons.cmvprintf( _rowRaw + 1 + _cursorY,
				_columnRaw + 3 + _cursorX * 3,
				_focused ? ATTR_CURSOR : COLOR::ATTR_NORMAL, "}" );
		cons.cmvprintf( _rowRaw - 1, _columnRaw + 8, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, ",--{" );
		if ( round >= 0 )
			cons.cmvprintf( _rowRaw - 1, _columnRaw + 13, COLOR::ATTR_NORMAL, "%4d", round );
		cons.cmvprintf( _rowRaw - 1, _columnRaw + 17, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, "}--." );
		cons.cmvprintf( _rowRaw - 1, _columnRaw + 23, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, ",--{" );
		cons.cmvprintf( _rowRaw - 1, _columnRaw + 28, _focused ? COLOR::FG_WHITE : COLOR::ATTR_NORMAL, "    " );
		if ( ( state == SELECT ) || ( state == INPUT ) ) {
			int sysNo( get_sys_no( _cursorX, _cursorY ) );
			if ( sysNo >= 0 ) {
				cons.cmvprintf( _rowRaw - 1, _columnRaw + 28,
						_focused ? COLOR::FG_WHITE : COLOR::ATTR_NORMAL, "%4d",
						distance( _sourceSystem, sysNo ) + round );
			}
		}
		cons.cmvprintf( _rowRaw - 1, _columnRaw + 32, _focused ? ATTR_BOARD : COLOR::ATTR_NORMAL, "}--." );
	}
	M_EPILOG
}

int HBoard::do_process_input( int code_ ) {
	M_PROLOG
	int code = 0, sysNo = - 1;
	client_state_t state = _listener.get_state();
	code_ = HWidget::do_process_input( code_ );
	if ( state == LOCKED )
		return ( code_ == '\t' ? '\t' : '\r' );
	if ( _boardSize >= 0 ) {
		switch ( code_ ) {
			case ( KEY_CODE::DOWN ): {
				if ( _cursorY < ( _boardSize - 1 ) )
					_cursorY ++;
				else _cursorY = 0;
				break;
			}
			case ( KEY_CODE::UP ): {
				if ( _cursorY > 0 )
					_cursorY --;
				else _cursorY = _boardSize - 1;
				break;
			}
			case ( KEY_CODE::LEFT ): {
				if ( _cursorX > 0 )
					_cursorX --;
				else _cursorX = _boardSize - 1;
				break;
			}
			case ( KEY_CODE::RIGHT ): {
				if ( _cursorX < ( _boardSize - 1 ) )
					_cursorX ++;
				else _cursorX = 0;
				break;
			}
			case ( KEY_CODE::HOME ): {
				_cursorX = 0;
				break;
			}
			case ( KEY_CODE::END ): {
				_cursorX = _boardSize - 1;
				break;
			}
			case ( KEY_CODE::PAGE_UP ): {
				_cursorY = 0;
				break;
			}
			case ( KEY_CODE::PAGE_DOWN ): {
				_cursorY = _boardSize - 1;
				break;
			}
			case ( '\r' ): {
				sysNo = get_sys_no( _cursorX, _cursorY );
				if ( sysNo >= 0 ) {
					if ( state == NORMAL ) {
						if ( ( *_systems )[ sysNo ]._color == _listener.get_color() ) {
							_sourceSystem = sysNo;
							_listener.set_state( SELECT );
							break;
						}
					} else if ( state == SELECT ) {
						_destinationSystem = sysNo;
						_listener.make_move( _sourceSystem, sysNo );
						return ( 0 );
					}
				}
				break;
			}
			default : {
				code = code_;
				break;
			}
		}
		if ( ! code ) {
			sysNo = get_sys_no( _cursorX, _cursorY );
			_listener.on_show_system_info( sysNo );
			schedule_repaint();
		}
	}
	return ( code );
	M_EPILOG
}

bool HBoard::do_click( mouse::OMouse& mouse_ ) {
	bool handled( HWidget::do_click( mouse_ ) );
	if ( ! handled ) {
		if ( ( mouse_._row > 1 )
				&& ( mouse_._row < ( _boardSize + 2 ) )
				&& ( mouse_._column > 1 )
				&& ( mouse_._column < ( ( _boardSize * 3 ) + 2 ) ) ) {
			_cursorY = mouse_._row - 2;
			_cursorX = ( mouse_._column - 2 ) / 3;
			_listener.on_show_system_info( get_sys_no( _cursorX, _cursorY ) );
			schedule_repaint();
			handled = true;
		}
	}
	return ( handled );
}

void HBoard::set_systems( systems_t* systems_ ) {
	_systems = systems_;
}

int HBoard::get_sys_no( int coordX_, int coordY_ ) {
	M_PROLOG
	int ctr = 0, systems = static_cast<int>( _systems->get_size() );
	for ( ctr = 0; ctr < systems; ctr ++ )
		if ( ( ( *_systems )[ ctr ]._coordinateX == coordX_ )
				&& ( ( *_systems )[ ctr ]._coordinateY == coordY_ ) )
			break;
	return ( ctr < systems ? ctr : - 1 );
	M_EPILOG
}

int HBoard::distance( int source_, int destination_ ) {
	M_PROLOG
	int dX = 0, dY = 0, dist = 0;
	if ( source_ != destination_ ) {
		dX = ( *_systems )[ source_ ]._coordinateX - ( *_systems )[ destination_ ]._coordinateX;
		dY = ( *_systems )[ source_ ]._coordinateY - ( *_systems )[ destination_ ]._coordinateY;
		dX = ( dX >= 0 ) ? dX : - dX;
		dY = ( dY >= 0 ) ? dY : - dY;
		dX = ( ( _boardSize - dX ) < dX ) ? _boardSize - dX : dX;
		dY = ( ( _boardSize - dY ) < dY ) ? _boardSize - dY : dY;
		dist = static_cast<int>( sqrt( dX * dX + dY * dY ) + 0.5 );
	}
	return ( dist );
	M_EPILOG
}

HGalaxyWindow::HGalaxyWindow(
	HString const& windowTitle_,
	HClient* client_,
	int& round_, int& color_, client_state_t& state_
) : HWindow( windowTitle_ )
	, HEventListener( round_, color_, state_ )
	, _varTmpBuffer()
	, _board( nullptr )
	, _systemName( nullptr )
	, _emperorName( nullptr )
	, _production( nullptr )
	, _fleet( nullptr )
	, _messageInput( nullptr )
	, _logPad( nullptr )
	, _client( client_ )
	, _systems( nullptr )
	, _emperors( nullptr )
	, _moves( nullptr ) {
	M_PROLOG
	return;
	M_EPILOG
}

HGalaxyWindow::~HGalaxyWindow( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HGalaxyWindow::do_init( void ) {
	M_PROLOG
	HWindow::do_init();
	_board = create_widget<HBoard>( this, *this );
	_board->enable( true );
	_board->set_focus();
	_board->set_systems( _systems );
	HEditWidgetAttributes wa;
	wa.label_decoration( HWidget::LABEL::DECORATION::AUTO ).label_position( HWidget::LABEL::POSITION::STACKED );
	_systemName = create_widget<HEditWidget>( this, 1, 64, 1, 16, "System name", wa.mask( _maskExtended_ ).max_string_size( 64 ) );
	_emperorName = create_widget<HEditWidget>( this, 4, 64, 1, 16, "Emperor", wa.mask( _maskExtended_ ).max_string_size( 64 ) );
	_production = create_widget<HEditWidget>( this, 7, 64, 1, 7, "Product", wa.mask( _maskDigits_ ).max_string_size( 6 ) );
	_fleet = create_widget<HEditWidget>( this, 7, 72, 1, 7, "Fleet", wa.mask( _maskDigits_ ).max_string_size( 6 ) );
	_logPad = create_widget<HLogPad>( this, 10, 64, - 5, - 1, "Event &log", wa );
	_messageInput = create_widget<HEditWidget>( this, - 4, 64, 1, - 1, "&Message", wa.mask( _maskLoose_ ).max_string_size( 255 ) );
	_logPad->enable( true );
	_messageInput->enable( true );
	register_postprocess_handler( '\r', nullptr, call( &HGalaxyWindow::handler_enter, this, _1 ) );
	register_postprocess_handler( KEY_CODE::ESCAPE, nullptr, call( &HGalaxyWindow::handler_esc, this, _1 ) );
	register_postprocess_handler( ' ', nullptr, call( &HGalaxyWindow::handler_space, this, _1 ) );
	return;
	M_EPILOG
}

HBoard * HGalaxyWindow::get_board( void ) {
	return ( _board );
}

void HGalaxyWindow::set_data( systems_t * systems_,
		emperors_t * emperors_, moves_t * moves_ ) {
	_systems = systems_;
	_emperors = emperors_;
	_moves = moves_;
	return;
}

void HGalaxyWindow::on_show_system_info( int system_ ) {
	M_PROLOG
	int color = - 1;
	HString name;
	_systemName->set_text( "" );
	_emperorName->set_text( "" );
	_production->set_text( "" );
	_fleet->set_text( "" );
	if ( system_ >= 0 ) {
		_systemName->set_text( _systemNames_[ system_ ] );
		if ( ( *_systems )[ system_ ]._production >= 0 )
			_production->set_text( ( *_systems )[ system_ ]._production );
		if ( ( *_systems )[ system_ ]._fleet >= 0 )
			_fleet->set_text( ( *_systems )[ system_ ]._fleet );
		color = ( *_systems )[ system_ ]._color;
		emperors_t::iterator it( _emperors->find( color ) );
		if ( ( color >= 0 ) && ( it != _emperors->end() ) ) {
			_emperorName->set_text( it->second );
		}
	}
	return;
	M_EPILOG
}

void HGalaxyWindow::make_move( int sourceSystem_, int destinationSystem_ ) {
	M_PROLOG
	if ( sourceSystem_ != destinationSystem_ ) {
		set_state( INPUT );
		_board->enable( false );
		_logPad->enable( false );
		_fleet->enable( true );
		_fleet->set_focus();
	} else {
		handler_esc( HKeyPressEvent( 27 ) );
	}
	return;
	M_EPILOG
}

bool HGalaxyWindow::handler_enter( hconsole::HEvent const& ) {
	M_PROLOG
	bool consumed( false );
	HMove* move( nullptr );
	if ( (*_focusedChild) == _messageInput ) {
		if ( _messageInput->get_text().find_other_than( character_class( CHARACTER_CLASS::WHITESPACE ).data() ) >= 0 ) {
			_varTmpBuffer = "cmd:";
			_varTmpBuffer += _client->id();
			_varTmpBuffer += ":say:";
			_varTmpBuffer += _messageInput->get_text();
			_varTmpBuffer += "\n";
			_client->send_message( _varTmpBuffer );
			_messageInput->set_text( "" );
		}
		consumed = true;
	} else if ( _state == INPUT ) {
		if ( (*_focusedChild) == _fleet ) {
			int fleet( lexical_cast<int>( _fleet->get_text() ) );
			if ( ( fleet > 0 )
					&& ( fleet <= ( * _systems )[ _board->_sourceSystem ]._fleet ) ) {
				_fleet->enable( false );
				_board->enable( true );
				_logPad->enable( true );
				_board->set_focus();
				move = & _moves->add_tail();
				move->_sourceSystem = _board->_sourceSystem;
				move->_destinationSystem = _board->_destinationSystem;
				move->_fleet = fleet;
				( * _systems )[ _board->_sourceSystem ]._fleet -= fleet;
				set_state( NORMAL );
			} else if ( fleet > 0 )
				_statusBar->message( COLOR::FG_RED, "Not enough resources!" );
			else
				_statusBar->message( COLOR::FG_RED, "Run! Run! Emperor is mad!" );
			consumed = true;
		}
	} else if ( _state == LOCKED ) {
		_statusBar->message( COLOR::FG_RED, _round >= 0 ? "Wait for new round!" : "Challange not started yet!" );
		consumed = true;
	}
	return ( consumed );
	M_EPILOG
}

bool HGalaxyWindow::handler_esc( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _state == INPUT ) {
		_fleet->enable( false );
		_board->enable( true );
		_logPad->enable( true );
		_board->set_focus();
		set_state( SELECT );
	} else if ( _state == SELECT )
		set_state( NORMAL );
	schedule_repaint( false );
	return ( 0 );
	M_EPILOG
}

bool HGalaxyWindow::handler_space( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _state == NORMAL )
		_client->end_round();
	return ( 0 );
	M_EPILOG
}

void HGalaxyWindow::msg( int attr_, char const* const msg_ ) {
	M_PROLOG
	_statusBar->message( attr_, "%s", msg_ );
	M_EPILOG
}

HClient::HClient( HString const& programName_ )
	: _id()
	, _color( -1 )
	, _round( -1 )
	, _state( LOCKED )
	, _socket( make_pointer<HSocket>() )
	, _window( new HGalaxyWindow( programName_, this, _round, _color, _state ) )
	, _board( nullptr )
	, _systems( 0 )
	, _handlers( 16 )
	, _emperors( 16 )
	, _moves()
	, _out() {
	M_PROLOG
	_handlers[ "setup" ] = &HClient::handler_setup;
	_handlers[ "client_setup" ] = &HClient::handler_msg;
	_handlers[ "play" ] = &HClient::handler_play;
	_handlers[ "msg" ] = &HClient::handler_msg;
	_handlers[ "say" ] = &HClient::handler_msg;
	_handlers[ "err" ] = &HClient::handler_error;
	_handlers[ "kck" ] = &HClient::handler_error;
	_handlers[ "player" ] = &HClient::handler_dummy;
	_handlers[ "party" ] = &HClient::handler_party;
	_handlers[ "party_info" ] = &HClient::handler_party_info;
	_handlers[ "player_quit" ] = &HClient::handler_dummy;
	return;
	M_EPILOG
}

HClient::~HClient( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

HString const& HClient::id( void ) const {
	return ( _id );
}

void HClient::init_client( HString& host_, int port_ ) {
	M_PROLOG
	HString message;
	_socket->connect( host_, port_ );
	_dispatcher.register_file_descriptor_handler( _socket, call( &HClient::handler_message, this, _1 ) );
	if ( setup._password.is_empty() ) {
		*_socket << ( _out << "login:" << CLIENT_VERSION << ":" << setup._login << endl << _out );
	} else {
		*_socket << ( _out << "login:" << CLIENT_VERSION << ":" << setup._login << ":" << tools::hash::sha1( setup._password ) << endl << _out );
	}
	_window->set_data( &_systems, &_emperors, &_moves );
	HTUIProcess::init_tui( "galaxy", _window );
	_board = _window->get_board();
	if ( ! setup._gameType.is_empty() ) {
		message = format( "create:glx:%s,%d,%d,%d\n",
				setup._game,
				setup._emperors, setup._boardSize, setup._systems );
		*_socket << message;
	} else {
		*_socket << "get_partys\n";
	}
	return;
	M_EPILOG
}

void HClient::handler_message( yaal::tools::HIODispatcher::stream_t& ) {
	M_PROLOG
	int long msgLength = 0;
	HString message;
	HString command;
	if ( ( msgLength = _socket->read_until( message ) ) > 0 ) {
		if ( setup._debug ) {
			_window->_logPad->add( message );
			_window->_logPad->add( "\n" );
		}
		msgLength = message.get_length();
		if ( msgLength < 1 ) {
			hcore::log << "got empty message from server" << endl;
			_dispatcher.stop();
		} else
			process_command( message );
	} else if ( msgLength < 0 )
		_dispatcher.stop();
	repaint();
	return;
	M_EPILOG
}

void HClient::process_command( HString& command_ ) {
	M_PROLOG
	HString mnemonic;
	HString argument;
	mnemonic = get_token( command_, ":", 0 );
	argument = command_.mid( mnemonic.get_length() + 1 );
	handlers_t::iterator it( _handlers.find( mnemonic ) );
	if ( it != _handlers.end() )
		( this->*(it->second) )( argument );
	else {
		hcore::log << "unknown mnemonic: " << mnemonic << ", (the argument: " << argument << ")" << endl;
		_dispatcher.stop();
	}
	return;
	M_EPILOG
}

void HClient::handler_party( HString& command_ ) {
	M_PROLOG
	HString i( get_token( command_, ",", 0 ) );
	if ( i == _id ) {
		HString command( command_.mid( _id.get_length() + 1 ) );
		process_command( command );
	}
	return;
	M_EPILOG
}

void HClient::handler_party_info( HString& command_ ) {
	M_PROLOG
	if ( _id.is_empty() ) {
		HTokenizer t( command_, "," );
		HString newId( t[0] );
		HString type( t[1] );
		HString name( t[2] );
		if ( ( type == "glx" ) && ( name == setup._game ) ) {
			_id = newId;
			if ( setup._gameType.is_empty() ) {
				setup._gameType = "glx";
				*_socket << ( _out << "join:" << _id << endl << _out );
			}
		}
	}
	return;
	M_EPILOG
}

void HClient::handler_setup( HString& command_ ) {
	M_PROLOG
	int index( -1 );
	HString variable( get_token( command_, "=", 0 ) );
	HString value( get_token( command_, "=", 1 ) );
	if ( variable == "board_size" )
		_window->_board->_boardSize = lexical_cast<int>( value );
	else if ( variable == "systems" ) {
		if ( _systems.get_size() )
			_dispatcher.stop();
		else
			_systems = HArray<HSystem>( setup._systems = lexical_cast<int>( value ) );
	} else if ( variable == "system_coordinates" ) {
		index = lexical_cast<int>( get_token( value, ",", 0 ) );
		int coordX( lexical_cast<int>( get_token( value, ",", 1 ) ) );
		int coordY( lexical_cast<int>( get_token( value, ",", 2 ) ) );
		_systems[ index ]._coordinateX = coordX;
		_systems[ index ]._coordinateY = coordY;
	} else if ( variable == "emperor" ) {
		index = lexical_cast<int>( get_token( value, ",", 0 ) );
		_emperors[ index ] = get_token( value, ",", 1 );
		if ( _emperors[ index ] == setup._login )
			_color = index;
	} else if ( variable == "ok" ) {
		_window->schedule_repaint( false );
		_window->set_state( NORMAL );
		_round = 0;
	}
	return;
	M_EPILOG
}

void HClient::handler_play( HString& command_ ) {
	M_PROLOG
	HString variable( get_token( command_, "=", 0 ) );
	HString value( get_token( command_, "=", 1 ) );
	if ( variable == "system_info" ) {
		char event( static_cast<char>( value[ 0 ].get() ) );
		int sysNo( lexical_cast<int>( get_token( value, ",", 1 ) ) );
		int production( lexical_cast<int>( get_token( value, ",", 2 ) ) );
		if ( production >= 0 )
			_systems[ sysNo ]._production = production;
		_systems[ sysNo ]._fleet = lexical_cast<int>( get_token( value, ",", 4 ) );
		int color( lexical_cast<int>( get_token( value, ",", 3 ) ) );
		/* scope to limit `it' visibility */ {
			emperors_t::iterator it( _emperors.find( color ) );
			M_ASSERT( it != _emperors.end() );
			value = it->second;
		}
		switch ( event ) {
			case ( 'c' ): /* conquered */
			case ( 'd' ): /* defeted */ {
				_window->_logPad->add( _colors_[ color ] );
				_window->_logPad->add( value );
				_window->_logPad->add( COLOR::ATTR_NORMAL );
				_window->_logPad->add( " conquered " );
				int temp = _systems[ sysNo ]._color;
				_window->_logPad->add( ( temp >= 0 ) ? _colors_[ temp ] : COLOR::ATTR_NORMAL );
				_window->_logPad->add( _systemNames_[ sysNo ] );
				value = format( "(%c)", _symbols_[ sysNo ] );
				_window->_logPad->add( value );
				_window->_logPad->add( COLOR::ATTR_NORMAL );
				_window->_logPad->add( ".\n" );
				_systems[ sysNo ]._color = color;
				break;
			}
			case ( 'r' ): /* reinforcements */ {
				_window->_logPad->add( "Reinforcements for " );
				_window->_logPad->add( _colors_[ color ] );
				_window->_logPad->add( _systemNames_[ sysNo ] );
				value = format( "(%c)", _symbols_[ sysNo ] );
				_window->_logPad->add( value );
				_window->_logPad->add( COLOR::ATTR_NORMAL );
				_window->_logPad->add( " arrived.\n" );
				break;
			}
			case ( 'f' ): /* failed to conquer */ {
				_systems[ sysNo ]._color = color;
				color = _color;
				emperors_t::iterator it( _emperors.find( color ) );
				M_ASSERT( it != _emperors.end() );
				value = it->second;
			} /* fallthrough */
			case ( 's' ): /* resisted attack */ {
				int temp = _systems[ sysNo ]._color;
				_window->_logPad->add( ( temp >= 0 ) ? _colors_[ temp ] : COLOR::ATTR_NORMAL );
				_window->_logPad->add( _systemNames_[ sysNo ] );
				variable = format( "(%c)", _symbols_[ sysNo ] );
				_window->_logPad->add( variable );
				_window->_logPad->add( COLOR::ATTR_NORMAL );
				_window->_logPad->add( " resisted attack from " );
				_window->_logPad->add( _colors_[ color ] );
				_window->_logPad->add( value );
				_window->_logPad->add( COLOR::ATTR_NORMAL );
				_window->_logPad->add( ".\n" );
				break;
			}
			case ( 'i' ): /* info */
			default : {
				break;
			}
		}
	} else if ( variable == "round" ) {
		_window->_logPad->add( ATTR_CURSOR );
		_window->_logPad->add( "----- " );
		_window->_logPad->add( COLOR::ATTR_NORMAL );
		_window->_logPad->add( " round: " );
		_round = lexical_cast<int>( value );
		value = format( "%d", _round );
		_window->_logPad->add( ATTR_CURSOR );
		_window->_logPad->add( value );
		_window->_logPad->add( " -----\n" );
		_window->_logPad->add( COLOR::ATTR_NORMAL );
		_window->set_state( NORMAL );
		_window->schedule_repaint( false );
	}
	return;
	M_EPILOG
}

void HClient::handler_error( HString& message_ ) {
	M_PROLOG
	hcore::log << "error: " << message_ << endl;
	_dispatcher.stop();
	return;
	M_EPILOG
}

namespace {

int find_color( HString const& message_, int offset_ ) {
	int colorStartIndex = -1;
	int start = offset_;
	int length = static_cast<int>( message_.get_length() );
	while ( start < length ) {
		start = static_cast<int>( message_.find( '$'_ycp, start ) );
		if ( start < 0 )
			break;
		int color = start + 1;
		while ( ( color < length ) && is_digit( message_[color] ) ) { /* *FIXME* *TODO* Remove static_cast after UCS in HString is implemented. */
			++ color;
		}
		if ( ( color < length ) && ( message_[ color ] == ';' ) ) {
			colorStartIndex = start;
			break;
		}
		start = color;
	}
	return ( colorStartIndex );
}

}

void HClient::handler_msg( HString& message_ ) {
	M_PROLOG
	int index( 0 );
	int length( static_cast<int>( message_.get_length() ) );
	HString part;
	while ( index < length ) {
		int color( find_color( message_, index ) );
		if ( color > index ) {
			_window->_logPad->add( message_.substr( index, color - index ) );
		}
		if ( color >= 0 ) {
			int colorEnd( static_cast<int>( message_.find( ';'_ycp, color ) ) );
			_window->_logPad->add( _colors_[ lexical_cast<int>( message_.substr( color + 1, colorEnd - color ) ) ] );
			index = colorEnd + 1;
		} else {
			_window->_logPad->add( message_.substr( index ) );
			break;
		}
	}
	_window->_logPad->add( "\n" );
	_window->_logPad->add( COLOR::ATTR_NORMAL );
	return;
	M_EPILOG
}

void HClient::end_round( void ) {
	M_PROLOG
	HString message;
	_window->set_state( LOCKED );
	if ( _moves.size() ) {
		for ( moves_t::iterator it = _moves.begin(); it != _moves.end(); ++ it ) {
			message = format( "cmd:%s:glx:play:move=%d,%d,%d\n",
					_id,
					it->_sourceSystem,
					it->_destinationSystem,
					it->_fleet );
			*_socket << message;
		}
		_moves.clear();
	}
	*_socket << ( _out << "cmd:" << _id << ":glx:play:end_round\n" << _out );
	return;
	M_EPILOG
}

void HClient::send_message( HString const & message_ ) {
	M_PROLOG
	*_socket << message_;
	return;
	M_EPILOG
}

void HClient::handler_dummy( HString& ) {
}

}

int main_client( void ) {
	if ( now_utc().get_hour() % 2 ) {
		_systemNames_ = _systemNamesNorse_;
	} else {
		_systemNames_ = _systemNamesLatin_;
	}
	M_ENSURE( setup._gameType.is_empty() || ( setup._gameType == "glx" ) );
	HClient client( setup._login );
	client.init_client( setup._host, setup._port );
	client.run();
	return ( 0 );
}

}

