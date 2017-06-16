/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  gamegroundd.cxx - this file is integral part of `galaxy' project.

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

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hformat.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/util.hxx>
#include <yaal/hconsole/console.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "galaxyd.hxx"

#include "setup.hxx"
#include "clientinfo.hxx"
#include "logicfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace yaal::hconsole;

namespace gameground {

namespace galaxy {

int const GALAXY_MSG_MALFORMED = 0;
char const* const GALAXY_MSG[] = {
	"malformed packet"
};

int _colors_[] = {
	( COLOR::FG_BRIGHTBLUE | COLOR::BG_BLACK ),
	( COLOR::FG_BRIGHTGREEN | COLOR::BG_BLACK ),
	( COLOR::FG_BRIGHTRED | COLOR::BG_BLACK ),
	( COLOR::FG_BRIGHTCYAN | COLOR::BG_BLACK ),
	( COLOR::FG_BRIGHTMAGENTA | COLOR::BG_BLACK ),
	( COLOR::FG_YELLOW | COLOR::BG_BLACK ),
	( COLOR::FG_BLUE | COLOR::BG_BLACK ),
	( COLOR::FG_GREEN | COLOR::BG_BLACK ),
	( COLOR::FG_RED | COLOR::BG_BLACK ),
	( COLOR::FG_CYAN | COLOR::BG_BLACK ),
	( COLOR::FG_MAGENTA | COLOR::BG_BLACK ),
	( COLOR::FG_BROWN | COLOR::BG_BLACK ),
	( COLOR::FG_LIGHTGRAY | COLOR::BG_BLACK ),
	( COLOR::FG_LIGHTGRAY | COLOR::BG_BLACK ),
	( COLOR::FG_LIGHTGRAY | COLOR::BG_BLACK ),
	( COLOR::FG_LIGHTGRAY | COLOR::BG_BLACK )
};

HSystem::HSystem ( HLogic const* logic_ )
	: _id( -1 ), _coordinateX( -1 ), _coordinateY( -1 ),
	_production( -1 ), _fleet( -1 ), _emperor( NULL ), _logic( logic_ ), _attackers() {
	M_PROLOG
	return;
	M_EPILOG
}

HSystem::HSystem( HSystem const& s )
	: _id( s._id ), _coordinateX( s._coordinateX ), _coordinateY( s._coordinateY ),
	_production( s._production ), _fleet( s._fleet ),
	_emperor( s._emperor ), _logic( s._logic ), _attackers( s._attackers ) {
}

HSystem& HSystem::operator = ( HSystem const& s ) {
	if ( &s != this ) {
		HSystem tmp( s );
		swap( tmp );
	}
	return ( *this );
}

void HSystem::swap( HSystem& other ) {
	if ( &other != this ) {
		using yaal::swap;
		swap( _id, other._id );
		swap( _coordinateX, other._coordinateX );
		swap( _coordinateY, other._coordinateY );
		swap( _production, other._production );
		swap( _fleet, other._fleet );
		swap( _emperor, other._emperor );
		swap( _logic, other._logic );
		swap( _attackers, other._attackers );
		M_ASSERT( ! "wrong execution path" );
	}
	return;
}

void HSystem::do_round( HGalaxy& galaxy_ ) {
	M_PROLOG
	HString message;
	if ( _emperor != NULL )
		_fleet += _production;
	else
		_fleet = _production;
	if ( _attackers.size() ) {
		int ec = 0;
		for ( attackers_t::iterator it = _attackers.begin(); it != _attackers.end(); ) {
			it->_arrivalTime --;
			if ( it->_arrivalTime <= 0 ) {
				int color( _emperor ? galaxy_.get_color( _emperor ) : 0 );
				if ( it->_emperor == _emperor ) /* reinforcements */ {
					_fleet += it->_size;
					message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
							_logic->id(), 'r', _id, _production, color, _fleet );
					*_emperor->_socket << message;
				} else if ( it->_size <= _fleet ) /* failed attack */ {
					_fleet -= it->_size;
					color = galaxy_.get_color( it->_emperor );
					message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
							_logic->id(), 's', _id, _production,
							color, _fleet );
					if ( _emperor ) {
						*_emperor->_socket << message; /* defender */
					}
					message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
							_logic->id(), 'f', _id, _production, _emperor ? galaxy_.get_color( _emperor ) : -1, -1 );
					*it->_emperor->_socket << message; /* attacker */
				} else if ( it->_size > _fleet ) {
					_fleet = it->_size - _fleet;
					color = galaxy_.get_color( it->_emperor );
					message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
							_logic->id(), 'd', _id, _production, color, - 1 );
					if ( _emperor ) {
						*_emperor->_socket << message; /* loser */
					}
					_emperor = it->_emperor;
					message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
							_logic->id(), 'c', _id, _production, color, _fleet );
					*it->_emperor->_socket << message; /* winer */
				}
				attackers_t::iterator done = it;
				++ it;
				M_ASSERT( _attackers.size() > 0 );
				_attackers.erase( done );
				++ ec;
			} else {
				galaxy_.mark_alive( it->_emperor );
				++ it;
			}
		}
	}
	if ( _emperor != NULL ) {
		message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
				_logic->id(), 'i', _id, - 1, galaxy_.get_color( _emperor ), _fleet );
		*_emperor->_socket << message;
	}
	if ( _emperor )
		galaxy_.mark_alive( _emperor );
	return;
	M_EPILOG
}

HGalaxy::HGalaxy( HServer* server_, id_t const& id_, HString const& comment_, int boardSize_, int systems_, int emperors_ )
	: HLogic( server_, id_, comment_ ), _boardSize( boardSize_ ), _neutralSystemCount( systems_ ),
	_startupPlayers( emperors_ ), _round( -1 ), _ready( 0 ),
	_systems( systems_ + emperors_, this ), _emperors() {
	M_PROLOG
	int ctr = 0, ctrLoc = 0;
	HRandomizer random( randomizer_helper::make_randomizer() );
	HSystem* system( NULL );
	for ( ctr = 0; ctr < ( emperors_ + systems_ ); ctr ++ ) {
		system = &_systems[ ctr ];
		system->_id = ctr;
		system->_coordinateX = static_cast<int>( random( static_cast<int unsigned>( _boardSize ) ) );
		system->_coordinateY = static_cast<int>( random( static_cast<int unsigned>( _boardSize ) ) );
		if ( ctr ) {
			for ( ctrLoc = 0; ctrLoc < ctr; ctrLoc ++ )
				if ( ( system->_coordinateX
							== _systems[ ctrLoc ]._coordinateX )
						&& ( system->_coordinateY
							== _systems[ ctrLoc ]._coordinateY ) )
					break;
			if ( ctrLoc < ctr ) {
				ctr --;
				continue;
			}
		}
	}
	for ( ctr = 0; ctr < ( emperors_ + systems_ ); ctr ++ )
		_systems[ ctr ]._production = _systems[ ctr ]._fleet = static_cast<int>( random( 16 ) );
	_handlers[ "play" ] = static_cast<handler_t>( &HGalaxy::handler_play );
	_handlers[ "say" ] = static_cast<handler_t>( &HGalaxy::handler_message );
	return;
	M_EPILOG
}

HGalaxy::~HGalaxy ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HGalaxy::do_accept( OClientInfo* ) {
	return ( false );
}

void HGalaxy::do_post_accept( OClientInfo* clientInfo_ ) {
	M_PROLOG
	int color = -1, sysNo = -1;
	HString message;
	/*
	 * Send basic setup info.
	 */
	message = format( "party:%s,setup:board_size=%d\n", _id, _boardSize );
	*clientInfo_->_socket << message; /* send setup info to new emperor */
	message = format( "party:%s,setup:systems=%d\n", _id, _startupPlayers + _neutralSystemCount );
	*clientInfo_->_socket << message;
	for ( int ctr = 0; ctr < ( _startupPlayers + _neutralSystemCount ); ++ ctr ) {
		message = format( "party:%s,setup:system_coordinates=%d,%d,%d\n",
				_id, ctr, _systems[ ctr ]._coordinateX,
				_systems[ ctr ]._coordinateY );
		*clientInfo_->_socket << message;
		if ( ( _round >= 0 ) && ( _systems[ ctr ]._emperor ) ) {
			message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
					_id, 'i', ctr, _systems[ ctr ]._production, get_color( _systems[ ctr ]._emperor ),
					_systems[ ctr ]._fleet );
			*clientInfo_->_socket << message;
		}
	}
	if ( ( _round < 0 ) && ( _ready < _startupPlayers ) ) {
		sysNo = assign_system( clientInfo_ ); /* assign mother system for new emperor */
		color = get_color( clientInfo_ );
		message = format( "setup:emperor=%d,%s\n",
				color, clientInfo_->_login );
		broadcast( message ); /* send setup information about new rival to all emperors */
		_systems[ sysNo ]._production = 10;
		_systems[ sysNo ]._fleet = 10;
		message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
				_id, 'c', sysNo, _systems[ sysNo ]._production, color,
				_systems[ sysNo ]._fleet );
		*clientInfo_->_socket << message;
		message = format( "msg:$12;Emperor $%d;", color );
		message += clientInfo_->_login;
		message += "$12; invaded the galaxy.\n";
		_ready ++;
	} else {
		_emperors[ clientInfo_ ] = OEmperorInfo();
		message = format( "party:%s,setup:emperor=%d,%s\n",
				_id, -1, clientInfo_->_login );
		*clientInfo_->_socket << message;
		message = "msg:$12;Spectator " + clientInfo_->_login + " is visiting this galaxy.\n";
	}
	broadcast( message ); /* inform every emperor about new rival */
	for ( emperors_t::iterator it = _emperors.begin(); it != _emperors.end(); ++ it ) {
		if ( it->first != clientInfo_ ) {
			int clr = it->second._color;
			if ( clr >= 0 ) {
				message = format( "party:%s,setup:emperor=%d,%s\n",
						_id, clr, it->first->_login );
				*clientInfo_->_socket << message;
				message = format( "party:%s,msg:$12;Emperor $%d;", _id, clr );
				message += it->first->_login;
				message += "$12; invaded the galaxy.\n";
			} else {
				message = "party:" + _id + ",msg:$12;Spectator " + it->first->_login + " is visiting this galaxy.\n";
			}
			*clientInfo_->_socket << message;
		}
	}
	if ( ( _round < 0 ) && ( _ready >= _startupPlayers ) ) {
		_round = 0;
		broadcast( "setup:ok\n" );
		_ready = 0;
	}
	return;
	M_EPILOG
}

int HGalaxy::assign_system( OClientInfo* clientInfo_ ) {
	M_PROLOG
	OEmperorInfo info;
	typedef HSet<int> integer_set_t;
	integer_set_t used;
	for ( emperors_t::iterator it = _emperors.begin(); it != _emperors.end(); ++ it )
		used.insert( it->second._color );
	int ctr = 0;
	for ( integer_set_t::HIterator it = used.begin(); it != used.end(); ++it, ++ ctr )
		if ( *it != ctr )
			break;
	info._color = ctr;
	info._systems = 1;
	int rivals = static_cast<int>( _emperors.size() );
	HRandomizer rnd( randomizer_helper::make_randomizer() );
	int motherSystem( static_cast<int>( rnd( static_cast<int unsigned>( _startupPlayers + _neutralSystemCount - rivals ) ) ) );
	_emperors[ clientInfo_ ] = info;
	ctr = 0;
	for ( int i = 0; i < motherSystem; ++ i, ++ ctr )
		while ( _systems[ ctr ]._emperor != NULL )
			++ ctr;
	while ( _systems[ ctr ]._emperor != NULL )
		++ ctr;
	M_ASSERT( ! _systems[ ctr ]._emperor );
	_systems[ ctr ]._emperor = clientInfo_;
	return ( ctr );
	M_EPILOG
}

void HGalaxy::handler_message( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	HString message;
	message = "say:$";
	int color = get_emperor_info( clientInfo_ )->_color;
	if ( color < 0 )
		color = 12;
	message += color;
	message += ';';
	message += clientInfo_->_login;
	message += "$12;: ";
	message += message_;
	message += '\n';
	broadcast( message );
	return;
	M_EPILOG
}

void HGalaxy::handler_play ( OClientInfo* clientInfo_, HString const& command_ ) {
	M_PROLOG
	if ( get_color( clientInfo_ ) == -1 ) {
		*clientInfo_->_socket << "err:Illegal message!\n" << endl;
		return;
	}
	HString variable;
	HString value;
	HFleet fleet;
	variable = get_token( command_, "=", 0 );
	value = get_token( command_, "=", 1 );
	if ( variable == "move" ) {
		fleet._emperor = clientInfo_;
		int source( lexical_cast<int>( get_token( value, ",", 0 ) ) );
		int destination( lexical_cast<int>( get_token( value, ",", 1 ) ) );
		fleet._size = lexical_cast<int>( get_token( value, ",", 2 ) );
		if ( ( source == destination )
				&& ( _systems[ source ]._emperor != clientInfo_ )
				&& ( _systems[ source ]._fleet < fleet._size ) )
			kick_client( clientInfo_ );
		else {
			_systems[ source ]._fleet -= fleet._size;
			int dX( _systems[ source ]._coordinateX - _systems[ destination ]._coordinateX );
			int dY( _systems[ source ]._coordinateY - _systems[ destination ]._coordinateY );
			dX = ( dX >= 0 ) ? dX : - dX;
			dY = ( dY >= 0 ) ? dY : - dY;
			dX = ( ( _boardSize - dX ) < dX ) ? _boardSize - dX : dX;
			dY = ( ( _boardSize - dY ) < dY ) ? _boardSize - dY : dY;
			fleet._arrivalTime = static_cast<int>( ::sqrt( dX * dX + dY * dY ) + 0.5 );
			_systems[ destination ]._attackers.push_front( fleet );
		}
	} else if ( variable == "end_round" ) {
		OEmperorInfo* info = get_emperor_info( clientInfo_ );
		M_ASSERT( info );
		if ( info->_systems >= 0 )
			_ready ++;
		OUT << "emperors: " << _emperors.size() << endl;
		OUT << "ready: " << _ready << endl;
		if ( _ready >= _emperors.size() )
			end_round();
	}
	return;
	M_EPILOG
}

void HGalaxy::end_round( void ) {
	M_PROLOG
	int ctr = 0, dead = 0;
	_ready = 0;
	for ( emperors_t::iterator it = _emperors.begin(); it != _emperors.end(); ++ it ) {
		if ( it->second._systems > 0 ) {
			it->second._systems = 0;
		}
	}
	for ( ctr = 0; ctr < ( _neutralSystemCount + _startupPlayers ); ctr ++ )
		_systems[ ctr ].do_round( *this );
	HString message;
	for ( emperors_t::iterator it = _emperors.begin(); it != _emperors.end(); ++ it ) {
		if ( ! it->second._systems ) {
			it->second._systems = -1;
			message = format( "msg:$12;Once mighty empire of $%d;%s$12; fall in ruins.\n",
					it->second._color, it->first->_login );
			broadcast( message );
		}
		if ( it->second._color >= 0 ) /* not spectator */ {
			if ( it->second._systems >= 0 )
				_ready ++;
			else
				dead ++;
		}
	}
	if ( _ready == 1 ) {
		for ( emperors_t::iterator it = _emperors.begin(); it != _emperors.end(); ++ it ) {
			if ( it->second._systems > 0 ) {
				message = format( "msg:$12;The invincible $%d;%s$12; crushed the galaxy.\n",
						it->second._color, it->first->_login );
				broadcast( message );
			}
		}
	}
	for ( ctr = 0; ctr < ( _startupPlayers + _neutralSystemCount ); ctr ++ ) {
		message = format( "party:%s,play:system_info=%c,%d,%d,%d,%d\n",
				_id, 'i', ctr, _systems[ ctr ]._production, get_color( _systems[ ctr ]._emperor ),
				_systems[ ctr ]._fleet );
		for ( emperors_t::iterator it = _emperors.begin(); it != _emperors.end(); ++ it ) {
			if ( it->second._color < 0 ) {
				*it->first->_socket << message;
			}
		}
	}
	_round ++;
	message = format( "play:round=%d\n", _round );
	broadcast( message );
	_ready = dead;
	return;
	M_EPILOG
}

int HGalaxy::get_color( OClientInfo* clientInfo_ ) {
	M_PROLOG
	OEmperorInfo* info = get_emperor_info( clientInfo_ );
	return ( info ? info->_color : -1 );
	M_EPILOG
}

void HGalaxy::mark_alive( OClientInfo* clientInfo_ ) {
	M_PROLOG
	get_emperor_info( clientInfo_ )->_systems ++;
	return;
	M_EPILOG
}

HGalaxy::OEmperorInfo* HGalaxy::get_emperor_info( OClientInfo* clientInfo_ ) {
	M_PROLOG
	OEmperorInfo* info = NULL;
	if ( clientInfo_ ) {
		emperors_t::iterator it = _emperors.find( clientInfo_ );
		M_ASSERT( it != _emperors.end() );
		info = &it->second;
	}
	return ( info );
	M_EPILOG
}

void HGalaxy::do_kick( OClientInfo* clientInfo_ ) {
	M_PROLOG
	for ( int ctr = 0; ctr < ( _neutralSystemCount + _startupPlayers ); ++ ctr ) {
		HSystem::attackers_t& attackers = _systems[ ctr ]._attackers;
		for ( HSystem::attackers_t::iterator it = attackers.begin();
				it != attackers.end(); ++ it ) {
			if ( it->_emperor == clientInfo_ )
				attackers.erase( it ++ ); /* First we increment iterator position and then we pass old iterator to erase() here. */
		}
		if ( _systems[ ctr ]._emperor == clientInfo_ )
			_systems[ ctr ]._emperor = NULL;
	}
	int color( _round >= 0 ? get_color( clientInfo_ ) : -1 );
	_emperors.erase( clientInfo_ );
	if ( ( color >= 0 ) && ( _round < 0 ) )
		-- _ready;
	if ( color >= 0 )
		broadcast( HString( "msg:$12;Emperor $" ) + color + ";" + clientInfo_->_login + "$12; fleed from the galaxy.\n" );
	else
		broadcast( HString( "msg:$12;Spectator " ) + clientInfo_->_login + " left this universum.\n" );
	OUT << "galaxy: dumping player: " << clientInfo_->_login << endl;
	if ( ( _round >= 0 ) && ( _ready >= _emperors.size() ) )
		end_round();
	OUT << "ready: " << _ready << ", emperors: " << _emperors.size() << endl;
	return;
	M_EPILOG
}

yaal::hcore::HString HGalaxy::do_get_info() const {
	return ( HString( "glx," ) + get_comment() + "," + _emperors.size() + "," + _startupPlayers + "," + _boardSize + "," + _neutralSystemCount );
}

}

namespace logic_factory {

class HGalaxyCreator : public HLogicCreatorInterface {
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual HString do_get_info( void ) const;
public:
} galaxyCreator;

HLogic::ptr_t HGalaxyCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	HString name = get_token( argv_, ",", 0 );
	int emperors( 0 );
	int boardSize( 0 );
	int systems( 0 );
	try {
		emperors = lexical_cast<int>( get_token( argv_, ",", 1 ) );
		boardSize = lexical_cast<int>( get_token( argv_, ",", 2 ) );
		systems = lexical_cast<int>( get_token( argv_, ",", 3 ) );
	} catch ( HLexicalCastException const& ) {
		throw HLogicException( galaxy::GALAXY_MSG[galaxy::GALAXY_MSG_MALFORMED] );
	}
	char* message = NULL;
	OUT << "new glx: ( " << name << " ) {" << endl;
	cout << "emperors = " << emperors << endl;
	cout << "board_size = " << boardSize << endl;
	cout << "systems = " << systems << endl;
	cout << "};" << endl;
	if ( OSetup::test_glx_emperors( emperors, message )
			|| OSetup::test_glx_emperors_systems( emperors, systems, message )
			|| OSetup::test_glx_systems( systems, message )
			|| OSetup::test_glx_board_size( boardSize, message ) )
		throw HLogicException( message );
	return ( make_pointer<galaxy::HGalaxy>( server_, id_, name,
					boardSize,
					systems,
					emperors ) );
	M_EPILOG
}

HString HGalaxyCreator::do_get_info( void ) const {
	HString setupMsg;
	setupMsg = format( "%s:%d,%d,%d", "glx", setup._emperors, setup._boardSize, setup._systems );
	OUT << setupMsg << endl;
	return ( setupMsg );
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.register_logic_creator( HTokenizer( galaxyCreator.get_info(), ":" )[0], &galaxyCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

