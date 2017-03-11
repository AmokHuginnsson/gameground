/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  god.cxx - this file is integral part of `gameground' project.

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

#include <cstring>

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hopenssl.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/util.hxx>
#include <yaal/tools/hscheduledasynccaller.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "god.hxx"

#include "setup.hxx"
#include "clientinfo.hxx"
#include "logicfactory.hxx"
#include "spellchecker.hxx"
#include "security.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace sgf;

namespace gameground {

namespace go {

HGo::STONE::stone_t const HGo::STONE::BLACK = 'b';
HGo::STONE::stone_t const HGo::STONE::WHITE = 'w';
HGo::STONE::stone_t const HGo::STONE::NONE = ' ';
HGo::STONE::stone_t const HGo::STONE::MARK = 'm';
HGo::STONE::stone_t const HGo::STONE::DEAD_BLACK = 's';
HGo::STONE::stone_t const HGo::STONE::DEAD_WHITE = 't';
HGo::STONE::stone_t const HGo::STONE::TERITORY_BLACK = 'p';
HGo::STONE::stone_t const HGo::STONE::TERITORY_WHITE = 'q';
HGo::STONE::stone_t const HGo::STONE::TERITORY_NONE = 'x';

char const* const HGo::PROTOCOL::SETUP = "setup";
char const* const HGo::PROTOCOL::ADMIN = "admin";
char const* const HGo::PROTOCOL::PLAY = "play";
char const* const HGo::PROTOCOL::NEWGAME = "newgame";
char const* const HGo::PROTOCOL::UNDO = "undo";
char const* const HGo::PROTOCOL::CONTESTANT = "contestant";
char const* const HGo::PROTOCOL::GOBAN = "goban";
char const* const HGo::PROTOCOL::KOMI = "komi";
char const* const HGo::PROTOCOL::HANDICAPS = "handicaps";
char const* const HGo::PROTOCOL::MAINTIME = "maintime";
char const* const HGo::PROTOCOL::BYOYOMIPERIODS = "byoyomiperiods";
char const* const HGo::PROTOCOL::BYOYOMITIME = "byoyomitime";
char const* const HGo::PROTOCOL::MOVE = "move";
char const* const HGo::PROTOCOL::TOMOVE = "to_move";
char const* const HGo::PROTOCOL::PUTSTONE = "put_stone";
char const* const HGo::PROTOCOL::SELECT = "select";
char const* const HGo::PROTOCOL::PASS = "pass";
char const* const HGo::PROTOCOL::SGF = "sgf";
char const* const HGo::PROTOCOL::SIT = "sit";
char const* const HGo::PROTOCOL::GETUP = "get_up";
char const* const HGo::PROTOCOL::DEAD = "dead";
char const* const HGo::PROTOCOL::ACCEPT = "accept";
char const* const HGo::PROTOCOL::REQUEST = "request";
char const* const HGo::PROTOCOL::REJECT = "reject";
char const* const HGo::PROTOCOL::IGNORE = "ignore";
static int const ACCEPTED = -7;

int const GO_MSG_MALFORMED = 0;
int const GO_MSG_INSUFFICIENT_PRIVILEGES = 1;
int const GO_MSG_YOU_CANNOT_DO_IT_NOW = 2;
int const GO_MSG_YOU_ARE_NOT_PLAYING = 3;
int const GO_MSG_NOT_YOUR_TURN = 4;
int const GO_MSG_BAD_SGF = 5;
char const* const GO_MSG[] = {
	"malformed packet",
	"insifficient privileges",
	"you cannot do it now",
	"you are not playing",
	"not your turn",
	"provided SGF has unexpected content"
};

HGo::HGo( HServer* server_, HLogic::id_t const& id_, HString const& comment_ )
	: HLogic( server_, id_, comment_ )
	, _toMove( setup._handicaps <= 1 ? STONE::BLACK : STONE::WHITE )
	, _marking( false )
	, _gobanSize( setup._gobanSize )
	, _komi100( setup._komi100 )
	, _handicaps( setup._handicaps )
	, _mainTime( setup._mainTime )
	, _byoYomiPeriods( setup._byoYomiPeriods )
	, _byoYomiTime( setup._byoYomiTime )
	, _move( 0 )
	, _pass( 0 )
	, _start( 0 )
	, _game( GOBAN_SIZE::NORMAL * GOBAN_SIZE::NORMAL + sizeof ( '\0' ) )
	, _koGame( GOBAN_SIZE::NORMAL * GOBAN_SIZE::NORMAL + sizeof ( '\0' ) )
	, _oldGame( GOBAN_SIZE::NORMAL * GOBAN_SIZE::NORMAL + sizeof ( '\0' ) )
	, _sgf( SGF::GAME_TYPE::GO, "gameground" )
	, _adminQueue()
	, _path()
	, _branch()
	, _varTmpBuffer()
	, _pendingUndoRequest( NULL ) {
	M_PROLOG
	_handlers[ PROTOCOL::SETUP ] = static_cast<handler_t>( &HGo::handler_setup );
	_handlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HGo::handler_play );
	_handlers[ PROTOCOL::SGF ] = static_cast<handler_t>( &HGo::handler_sgf );
	_handlers[ PROTOCOL::SIT ] = static_cast<handler_t>( &HGo::handler_sit );
	_handlers[ PROTOCOL::GETUP ] = static_cast<handler_t>( &HGo::handler_getup );
	_handlers[ PROTOCOL::SELECT ] = static_cast<handler_t>( &HGo::handler_select );
	_handlers[ PROTOCOL::NEWGAME ] = static_cast<handler_t>( &HGo::handler_newgame );
	reset_goban( true );
	return;
	M_EPILOG
}

HGo::~HGo ( void ) {
	M_PROLOG
	revoke_scheduled_tasks();
	return;
	M_EPILOG
}

void HGo::broadcast_contestants( yaal::hcore::HString const& message_ ) {
	M_PROLOG
	M_ASSERT( _contestants[ 0 ]._client && _contestants[ 1 ]._client );
	*_contestants[ 0 ]._client->_socket << message_;
	*_contestants[ 1 ]._client->_socket << message_;
	return;
	M_EPILOG
}

void HGo::handler_setup( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( ! can_setup( clientInfo_ ) )
		throw HLogicException( GO_MSG[ GO_MSG_INSUFFICIENT_PRIVILEGES ] );
	HString item = get_token( message_, ",", 0 );
	int value( 0 );
	try {
		value = lexical_cast<int>( get_token( message_, ",", 1 ) );
	} catch ( HLexicalCastException const& ) {
		throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
	}
	bool regenGoban( false );
	try {
		if ( item == PROTOCOL::GOBAN ) {
			_sgf.set_board_size( _gobanSize = value );
			regenGoban = true;
		} else if ( item == PROTOCOL::KOMI ) {
			_sgf.set_komi100( _komi100 = value );
		} else if ( item == PROTOCOL::HANDICAPS ) {
			set_handicaps( value );
			regenGoban = true;
		} else if ( item == PROTOCOL::MAINTIME ) {
			_sgf.set_time( _mainTime = value );
		} else if ( item == PROTOCOL::BYOYOMIPERIODS ) {
			SGF::byoyomi_t byoyomi( _sgf.get_byoyomi() );
			_sgf.set_overtime( _byoYomiPeriods = value, byoyomi.second );
		} else if ( item == PROTOCOL::BYOYOMITIME ) {
			SGF::byoyomi_t byoyomi( _sgf.get_byoyomi() );
			_sgf.set_overtime( byoyomi.first, _byoYomiTime = value );
		} else
			throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	} catch ( SGFException const& ) {
		throw HLogicException( GO_MSG[ GO_MSG_BAD_SGF ] );
	}
	broadcast( _out << PROTOCOL::SETUP << PROTOCOL::SEP << message_ << endl << _out );
	if ( regenGoban )
		reset_goban( true );
	send_goban();
	return;
	M_EPILOG
}

namespace {
void dump( char const* stones_, int size_ ) {
	for ( int r( 0 ); r < size_; ++ r ) {
		for ( int c( 0 ); c < size_; ++ c ) {
			cout << stones_[r * size_ + c];
		}
		cout << endl;
	}
}
}

void HGo::handler_sgf( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	if ( ! can_setup( clientInfo_ ) )
		throw HLogicException( GO_MSG[ GO_MSG_INSUFFICIENT_PRIVILEGES ] );
	try {
		SGF sgf( SGF::GAME_TYPE::GO, "gameground" );
		OUT << message_ << endl;
		sgf.load( message_ );
		_sgf.swap( sgf );
		_gobanSize = _sgf.get_board_size();
		_komi100 = _sgf.get_komi100();
		_handicaps = _sgf.get_handicap();
		_mainTime = _sgf.get_time();
		SGF::byoyomi_t byoyomi( _sgf.get_byoyomi() );
		_byoYomiPeriods = byoyomi.first;
		_byoYomiTime = byoyomi.second;
		reset_goban( false );
		SGF::game_tree_t::const_node_t currentMove( _sgf.game_tree().get_root() );
		_branch.clear();
		if ( currentMove ) {
			while ( currentMove->has_children() ) {
				currentMove = currentMove->get_child_at( 0 );
				_branch.push_back( currentMove );
				apply_move( currentMove );
			}
			_sgf.set_current_move( currentMove );
		}
		dump( _game.get<char>(), _gobanSize );
		broadcast( _out << PROTOCOL::SGF << PROTOCOL::SEP << message_ << endl << _out );
		broadcast( _out << PROTOCOL::SETUP << PROTOCOL::SEP
			<< PROTOCOL::GOBAN << PROTOCOL::SEPP << _gobanSize << endl
			<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
			<< PROTOCOL::KOMI << PROTOCOL::SEPP << _komi100 << endl
			<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
			<< PROTOCOL::HANDICAPS << PROTOCOL::SEPP << _handicaps << endl
			<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
			<< PROTOCOL::MAINTIME << PROTOCOL::SEPP << _mainTime << endl
			<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
			<< PROTOCOL::BYOYOMIPERIODS << PROTOCOL::SEPP << _byoYomiPeriods << endl
			<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
			<< PROTOCOL::BYOYOMITIME << PROTOCOL::SEPP << _byoYomiTime << endl << _out );
	} catch ( SGFException const& ) {
		throw HLogicException( GO_MSG[ GO_MSG_BAD_SGF ] );
	}
	return;
	M_EPILOG
}

void HGo::handler_sit( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	if ( message_.get_length() < 1 )
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	else {
		char stone = message_[ 0 ];
		if ( ( stone != STONE::BLACK ) && ( stone != STONE::WHITE ) )
			throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
		else if ( ( contestant( STONE::BLACK )._client == clientInfo_ )
				|| ( contestant( STONE::WHITE )._client == clientInfo_ ) )
			throw HLogicException( "you were already sitting" );
		else if ( contestant( stone )._client != NULL )
			*clientInfo_->_socket << *this
				<< PROTOCOL::MSG << PROTOCOL::SEP << "Some one was faster." << endl;
		else {
			OPlayerInfo& info = contestant( stone );
			info._client = clientInfo_;
			info._timeLeft = _mainTime;
			info._byoYomiPeriods = _byoYomiPeriods;
			info._stonesCaptured = 0;
			info._score = ( stone == STONE::WHITE ? _komi100 : 0 );
			OPlayerInfo& black( contestant( STONE::BLACK ) );
			OPlayerInfo& white( contestant( STONE::WHITE ) );
			if ( black._client && white._client ) {
				OPlayerInfo& foe = contestant( opponent( stone ) );
				foe._timeLeft = info._timeLeft;
				foe._byoYomiPeriods = info._byoYomiPeriods;
				_pass = 0;
				_sgf.set_player( SGF::Player::BLACK, black._client->_login );
				_sgf.set_player( SGF::Player::WHITE, white._client->_login );
				clear_markers();
				send_goban();
				broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP << "The Go match started." << endl << _out );
			}
			after_move();
		}
	}
	return;
	M_EPILOG
}

void HGo::handler_getup( OClientInfo* clientInfo_, HString const& /*message_*/ ) {
	M_PROLOG
	if ( ( contestant( STONE::BLACK )._client != clientInfo_ ) && ( contestant( STONE::WHITE )._client != clientInfo_ ) )
		throw HLogicException( "you were not sitting" );
	contestant_gotup( clientInfo_ );
	after_move();
	return;
	M_EPILOG
}

void HGo::handler_select( OClientInfo* clientInfo_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	if ( ! can_setup( clientInfo_ ) )
		throw HLogicException( GO_MSG[ is_admin( clientInfo_ ) ? GO_MSG_YOU_CANNOT_DO_IT_NOW : GO_MSG_INSUFFICIENT_PRIVILEGES ] );
	HTokenizer t( message_, "," );
	HTokenizer::HIterator it( t.begin() );
	HTokenizer::HIterator end( t.end() );
	if ( it == end )
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	int moveNo( 0 );
	try {
		moveNo = lexical_cast<int>( *it );
	} catch ( HLexicalCastException const& ) {
		throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
	}
	++ it;
	SGF::game_tree_t::const_node_t currentMove( _sgf.game_tree().get_root() );
	_branch.clear();
	if ( currentMove ) {
		for ( int i( 0 ); i < moveNo; ++ i ) {
			int childCount( static_cast<int>( currentMove->child_count() ) );
			if ( childCount == 0 )
				throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
			int child( 0 );
			if ( childCount > 1 ) {
				if ( it == end )
					throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
				try {
					child = lexical_cast<int>( *it );
				} catch ( HLexicalCastException const& ) {
					throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
				}
				++ it;
			}
			currentMove = currentMove->get_child_at( child );
			_branch.push_back( currentMove );
		}
		_sgf.set_current_move( currentMove );
		reset_goban( false );
		for ( branch_t::const_iterator i( _branch.begin() ), e( _branch.end() ); i != e; ++ i )
			apply_move( *i );
	} else if ( moveNo > 0 )
		throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
	broadcast( _out << PROTOCOL::SELECT << PROTOCOL::SEP << message_ << endl << _out );
	return;
	M_EPILOG
}

void HGo::handler_put_stone( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	OPlayerInfo& player( contestant( _toMove ) );
	if ( ! can_setup( clientInfo_ ) ) {
		if ( _marking )
			throw HLogicException( GO_MSG[ GO_MSG_YOU_CANNOT_DO_IT_NOW ] );
		if ( player._client != clientInfo_ )
			throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	}
	_pass = 0;
	int col( 0 );
	int row( 0 );
	try {
		col = lexical_cast<int>( get_token( message_, ",", 0 ) );
		row = lexical_cast<int>( get_token( message_, ",", 1 ) );
	} catch ( HLexicalCastException const& ) {
		throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
	}
	make_move( col, row );
	_sgf.move( SGF::Coord( col, row ), player._timeLeft - static_cast<int>( ::time( nullptr ) - _start ) );
	_out << PROTOCOL::MOVE << PROTOCOL::SEP << message_ << endl;
	broadcast( _out.consume() );
	return;
	M_EPILOG
}

void HGo::apply_teritory_marker( void ) {
	M_PROLOG
	_sgf.clear_markers( _sgf.get_current_move() );
	for ( int i = 0; i < ( _gobanSize * _gobanSize ); ++ i ) {
		int x = i / _gobanSize;
		int y = i % _gobanSize;
		switch ( goban( x, y ) ) {
			case ( STONE::DEAD_WHITE ):
			case ( STONE::TERITORY_BLACK ): {
				_sgf.add_position( SGF::Position::BLACK_TERITORY, SGF::Coord( x, y ) );
			} break;
			case ( STONE::DEAD_BLACK ):
			case ( STONE::TERITORY_WHITE ): {
				_sgf.add_position( SGF::Position::WHITE_TERITORY, SGF::Coord( x, y ) );
			} break;
			default: {
			} break;
		}
	}
	return;
	M_EPILOG
}

void HGo::clear_markers( void ) {
	M_PROLOG
	if ( _sgf.get_current_move() ) {
		_sgf.clear_markers( _sgf.get_current_move() );
		replace_stones( STONE::DEAD_BLACK, STONE::BLACK );
		replace_stones( STONE::DEAD_WHITE, STONE::WHITE );
		replace_stones( STONE::TERITORY_BLACK, STONE::NONE );
		replace_stones( STONE::TERITORY_WHITE, STONE::NONE );
		replace_stones( STONE::TERITORY_NONE, STONE::NONE );
		commit();
	}
	M_EPILOG
}

void HGo::handler_pass( OClientInfo* clientInfo_ ) {
	M_PROLOG
	if ( _marking )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANNOT_DO_IT_NOW ] );
	if ( contestant( _toMove )._client != clientInfo_ )
		throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	++ _pass;
	_sgf.move( SGF::PASS );
	if ( _pass == 3 ) {
		_marking = true;
		revoke_scheduled_tasks();
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP << "The match has ended." << endl << _out );
		broadcast_contestants( _out << *this << PROTOCOL::MSG << PROTOCOL::SEP << "Select your dead stones." << endl << _out );
		mark_teritory();
		apply_teritory_marker();
	} else
		pass();
	send_goban();
	return;
	M_EPILOG
}

void HGo::pass( void ) {
	M_PROLOG
	_toMove = opponent( _toMove );
	return;
	M_EPILOG
}

void HGo::handler_dead( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	if ( ! _marking )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANNOT_DO_IT_NOW ] );
	HString str;
	for ( int i( 0 ); ! ( str = get_token( message_, ",", i ) ).is_empty() ; i += 2 ) {
		int col( 0 );
		int row( 0 );
		try {
			col = lexical_cast<int>( str );
			row = lexical_cast<int>( get_token( message_, ",", i + 1 ) );
		} catch ( HLexicalCastException const& ) {
			throw HLogicException( GO_MSG[GO_MSG_MALFORMED] );
		}
		OUT << "dead: " << col << "," << row << endl;
		ensure_coordinates_validity( col, row );
		STONE::stone_t stone = goban( col, row );
		if( ! ( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE )
					|| ( stone == STONE::DEAD_BLACK ) || ( stone == STONE::DEAD_WHITE ) ) )
			throw HLogicException( "no stone here" );
		if ( contestant( goban( col, row ) )._client != clientInfo_ )
			throw HLogicException( "not your stone" );
		mark_stone_dead( col, row );
	}
	mark_teritory();
	apply_teritory_marker();
	send_goban();
	return;
	M_EPILOG
}

void HGo::handler_accept( OClientInfo* clientInfo_ ) {
	M_PROLOG
	if ( ! _marking )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANNOT_DO_IT_NOW ] );
	OPlayerInfo& info( *contestant( clientInfo_ ) );
	if ( info._byoYomiPeriods == ACCEPTED )
		throw HLogicException( "you already accepted stones" );
	info._byoYomiPeriods = ACCEPTED;
	if ( ( _contestants[ 0 ]._byoYomiPeriods == ACCEPTED )
			&& ( _contestants[ 1 ]._byoYomiPeriods == ACCEPTED ) ) {
		count_score();
		end_match();
		send_contestants();
	}
	return;
	M_EPILOG
}

void HGo::handler_newgame( OClientInfo* clientInfo_, HString const& ) {
	M_PROLOG
	if ( ! can_setup( clientInfo_ ) )
		throw HLogicException( GO_MSG[ GO_MSG_INSUFFICIENT_PRIVILEGES ] );
	_start = 0;
	_move = 0;
	reset_goban( true );
	send_goban();
	return;
	M_EPILOG
}

void HGo::handler_undo( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	int const opponentIdx( _contestants[0]._client == clientInfo_ ? 1 : 0 );
	if ( message_ == PROTOCOL::REQUEST ) {
		cout << "undo request" << endl;
		if ( ! _contestants[opponentIdx]._ignoreUndo ) {
			if ( _pendingUndoRequest ) {
				if ( _pendingUndoRequest == _contestants[opponentIdx]._client ) {
					_out << PROTOCOL::MSG << PROTOCOL::SEP << "Your opponent requested undo before you did." << endl;
					*clientInfo_->_socket << *this << _out.consume();
				}
			} else {
				int skip( _toMove == ( opponentIdx == 0 ? STONE::BLACK : STONE::WHITE ) ? 1 : 2 );
				OUT << "skip = " << skip << endl;
				SGF::game_tree_t::const_node_t currentMove( _sgf.get_current_move() );
				while ( ( skip > 0 ) && currentMove && currentMove->get_parent() ) {
					currentMove = currentMove->get_parent();
					-- skip;
				}
				if ( ! skip ) {
					_pendingUndoRequest = clientInfo_;
					OClientInfo* opp( _contestants[opponentIdx]._client );
					_out << PROTOCOL::UNDO << endl;
					*opp->_socket << *this << _out.consume();
					_toMove = opponent( _toMove );
				} else {
					_out << PROTOCOL::MSG << PROTOCOL::SEP << "You cannot undo at this stage." << endl;
					*clientInfo_->_socket << *this << _out.consume();
				}
			}
		} else {
			_out << PROTOCOL::MSG << PROTOCOL::SEP << "Your opponent ignores all your undo requests." << endl;
			*clientInfo_->_socket << *this << _out.consume();
		}
	} else {
		if ( ! _pendingUndoRequest || ( _pendingUndoRequest != _contestants[opponentIdx]._client ) )
			throw HLogicException( GO_MSG[ GO_MSG_YOU_CANNOT_DO_IT_NOW ] );
		if ( message_ == PROTOCOL::ACCEPT ) {
			SGF::game_tree_t::const_node_t currentMove( _sgf.get_current_move() );
			if ( currentMove ) {
				int skip( _toMove == ( opponentIdx == 0 ? STONE::BLACK : STONE::WHITE ) ? 1 : 2 );
				OUT << "skip = " << skip << endl;
				for ( int i( 0 ); currentMove->get_parent() && ( i < skip ); ++ i )
					currentMove = currentMove->get_parent();
				_sgf.set_current_move( currentMove );
				reset_goban( false );
				branch_t::const_iterator m( _branch.begin() );
				for ( int i( 0 ), SIZE( static_cast<int>( _branch.get_size() - skip ) ); i < SIZE; ++ i, ++ m )
					apply_move( *m );
				send_path();
				if ( skip == 1 )
					_toMove = opponent( _toMove );
			}
		} else if ( message_ == PROTOCOL::REJECT ) {
			_out << PROTOCOL::MSG << PROTOCOL::SEP << "Your opponent rejected your undo request." << endl;
			*_pendingUndoRequest->_socket << *this << _out.consume();
		} else if ( message_ == PROTOCOL::IGNORE ) {
			_contestants[1 - opponentIdx]._ignoreUndo = true;
			_out << PROTOCOL::MSG << PROTOCOL::SEP << "Your opponent rejected your undo request and will ignore all future undo requests." << endl;
			*_pendingUndoRequest->_socket << *this << _out.consume();
		} else {
			throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
		}
		_toMove = opponent( _toMove );
		_pendingUndoRequest = NULL;
	}
	return;
	M_EPILOG
}

HGo::STONE::stone_t HGo::mark_teritory( int x, int y ) {
	STONE::stone_t teritory = static_cast<char>( toupper( STONE::TERITORY_NONE ) );
	STONE::stone_t stone = goban( x, y );
	if ( ( stone == STONE::NONE ) || ( stone == STONE::DEAD_BLACK ) || ( stone == STONE::DEAD_WHITE ) ) {
		if ( stone == STONE::NONE )
			goban( x, y ) = teritory;
		else
			goban( x, y ) = static_cast<char>( toupper( stone ) );
		int blackNeighbour = 0;
		int whiteNeighbour = 0;
		int bothNeighbour = 0;
		int directs[][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
		for ( int i = 0; i < 4; ++ i ) {
			int nx = x + directs[ i ][ 0 ];
			int ny = y + directs[ i ][ 1 ];
			if ( ( nx >= 0 ) && ( nx < _gobanSize )
				&& ( ny >= 0 ) && ( ny < _gobanSize ) ) {
				STONE::stone_t neighbour = goban( nx, ny );
				if ( ( neighbour != teritory )
						&& ( neighbour != toupper( STONE::DEAD_BLACK ) )
						&& ( neighbour != toupper( STONE::DEAD_WHITE ) ) ) {
					STONE::stone_t newTeritory = mark_teritory( nx, ny );
					if ( newTeritory == STONE::BLACK )
						++ blackNeighbour;
					else  if ( newTeritory == STONE::WHITE )
						++ whiteNeighbour;
					else if ( ( newTeritory == STONE::TERITORY_NONE ) || ( newTeritory == toupper( STONE::TERITORY_NONE ) ) )
						++ bothNeighbour;
					else {
						M_ASSERT( newTeritory == STONE::NONE );
					}
				}
			}
		}
		if ( blackNeighbour || whiteNeighbour || bothNeighbour ) {
			if ( blackNeighbour && ! ( whiteNeighbour || bothNeighbour ) )
				teritory = STONE::BLACK;
			else if ( whiteNeighbour && ! ( blackNeighbour || bothNeighbour ) )
				teritory = STONE::WHITE;
			else
				teritory = STONE::TERITORY_NONE;
		} else {
			if ( stone == STONE::DEAD_BLACK )
				teritory = STONE::WHITE;
			else if ( stone == STONE::DEAD_WHITE )
				teritory = STONE::BLACK;
			else
				teritory = STONE::NONE;
		}
	} else if ( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE ) || ( stone == toupper( STONE::TERITORY_NONE ) ) )
		teritory = stone;
	else {
		M_ASSERT( ! "bad code path" );
	}
	return ( teritory );
}

void HGo::mark_teritory( void ) {
	M_PROLOG
	replace_stones( STONE::TERITORY_BLACK, STONE::NONE );
	replace_stones( STONE::TERITORY_WHITE, STONE::NONE );
	replace_stones( STONE::TERITORY_NONE, STONE::NONE );
	for ( int i = 0; i < ( _gobanSize * _gobanSize ); ++ i ) {
		int x = i / _gobanSize;
		int y = i % _gobanSize;
		if ( goban( x, y ) == STONE::NONE ) {
			STONE::stone_t teritory = mark_teritory( x, y );
			STONE::stone_t mark = STONE::TERITORY_NONE;
			switch ( teritory ) {
				case ( STONE::BLACK ): mark = STONE::TERITORY_BLACK; break;
				case ( STONE::WHITE ): mark = STONE::TERITORY_WHITE; break;
				case ( STONE::TERITORY_NONE ): mark = STONE::TERITORY_NONE; break;
				default:
					OUT << "teritory: '" << teritory << "', at: " << x << "," << y << endl;
					M_ASSERT( ! "bug in mark_teritory switch" );
			}
			replace_stones( static_cast<char>( toupper( STONE::TERITORY_NONE ) ), mark );
		}
	}
	replace_stones( static_cast<char>( toupper( STONE::DEAD_BLACK ) ), STONE::DEAD_BLACK );
	replace_stones( static_cast<char>( toupper( STONE::DEAD_WHITE ) ), STONE::DEAD_WHITE );
	return;
	M_EPILOG
}

void HGo::count_score( void ) {
	M_PROLOG
	mark_teritory();
	commit();
	int blackTeritory = count_stones( STONE::TERITORY_BLACK );
	int whiteTeritory = count_stones( STONE::TERITORY_WHITE );
	int blackCaptures = count_stones( STONE::DEAD_BLACK );
	int whiteCaptures = count_stones( STONE::DEAD_WHITE );
	blackTeritory += whiteCaptures;
	whiteTeritory += blackCaptures;
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "The game results are: " << endl << _out );
	OPlayerInfo& b( _contestants[ 0 ] );
	OPlayerInfo& w( _contestants[ 1 ] );
	b._stonesCaptured += whiteCaptures;
	w._stonesCaptured += blackCaptures;
	b._score = b._stonesCaptured + blackTeritory;
	w._score += ( w._stonesCaptured + whiteTeritory );
	HString komiStr( _komi100 / 100 );
	if ( _komi100 % 100 ) {
		komiStr.append( "." );
		if ( _komi100 % 10 ) {
			komiStr.append( _komi100 % 100 );
		} else {
			komiStr.append( ( _komi100 % 100 ) / 10 );
		}
	}
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Black's (" << b._client->_login << ") teritory: " << blackTeritory
			<< ", captutes: " << b._stonesCaptured << "." << endl << _out );
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "White's (" << w._client->_login << ") teritory: " << whiteTeritory
			<< ", captutes: " << w._stonesCaptured
			<< ", and " << _komi100 << " of komi." << endl << _out );
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< ( b._score > w._score ? b._client->_login : w._client->_login )
			<< ( b._score > w._score ? " (black)" : " (white)" )
			<< " wins by " << ( b._score > w._score ? b._score - w._score : w._score - b._score ) + .5
			<< endl << _out );
	send_goban();
	return;
	M_EPILOG
}

void HGo::handler_play( OClientInfo* clientInfo_, HString const& message_ ) {
	M_PROLOG
	HLock l( _mutex );
	HString item( get_token( message_, ",", 0 ) );
	if ( ! can_play( clientInfo_ ) ) {
		if ( can_setup( clientInfo_ ) ) {
			if ( item != PROTOCOL::PUTSTONE )
				throw HLogicException( GO_MSG[GO_MSG_YOU_CANNOT_DO_IT_NOW] );
		} else
			throw HLogicException( GO_MSG[GO_MSG_YOU_ARE_NOT_PLAYING] );
	}
	HString param( message_.mid( item.get_length() + 1 ) );
	bool callAfterMove( false );
	if ( item == PROTOCOL::PUTSTONE ) {
		handler_put_stone( clientInfo_, param );
		callAfterMove = true;
	} else if ( item == PROTOCOL::PASS ) {
		handler_pass( clientInfo_ );
		callAfterMove = true;
	} else if ( item == PROTOCOL::DEAD )
		handler_dead( clientInfo_, param );
	else if ( item == PROTOCOL::ACCEPT )
		handler_accept( clientInfo_ );
	else if ( item == PROTOCOL::UNDO ) {
		handler_undo( clientInfo_, param );
		callAfterMove = true;
	} else
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	if ( callAfterMove )
		after_move();
	return;
	M_EPILOG
}

void HGo::after_move( void ) {
	M_PROLOG
	if ( ongoing_match() && ! _marking )
		update_clocks();
	send_contestants();
	broadcast( _out << PROTOCOL::TOMOVE << PROTOCOL::SEP
			<< static_cast<char>( _marking ? STONE::MARK : _toMove ) << endl << _out );
	return;
	M_EPILOG
}

void HGo::end_match( void ) {
	M_PROLOG
	_contestants[0]._client = _contestants[1]._client = NULL;
	_marking = false;
	_pendingUndoRequest = NULL;
	return;
	M_EPILOG
}

HGo::OPlayerInfo* HGo::contestant( OClientInfo const* clientInfo_ ) {
	M_PROLOG
	OPlayerInfo* pi( NULL );
	if ( _contestants[0]._client == clientInfo_ )
		pi = &_contestants[0];
	else if ( _contestants[1]._client == clientInfo_ )
		pi = &_contestants[1];
	return ( pi );
	M_EPILOG
}

HGo::OPlayerInfo const* HGo::contestant( OClientInfo const* clientInfo_ ) const {
	M_PROLOG
	OPlayerInfo const* pi( NULL );
	if ( _contestants[0]._client == clientInfo_ )
		pi = &_contestants[0];
	else if ( _contestants[1]._client == clientInfo_ )
		pi = &_contestants[1];
	return ( pi );
	M_EPILOG
}

bool HGo::do_accept( OClientInfo* clientInfo_ ) {
	OUT << "new candidate " << clientInfo_->_login << endl;
	return ( false );
}

void HGo::do_post_accept( OClientInfo* clientInfo_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _adminQueue.is_empty() )
		*clientInfo_->_socket << *this
			<< PROTOCOL::SETUP << PROTOCOL::SEP << PROTOCOL::ADMIN << endl;
	*clientInfo_->_socket
		<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::GOBAN << PROTOCOL::SEPP << _gobanSize << endl
		<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::KOMI << PROTOCOL::SEPP << _komi100 << endl
		<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::HANDICAPS << PROTOCOL::SEPP << _handicaps << endl
		<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::MAINTIME << PROTOCOL::SEPP << _mainTime << endl
		<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::BYOYOMIPERIODS << PROTOCOL::SEPP << _byoYomiPeriods << endl
		<< *this << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::BYOYOMITIME << PROTOCOL::SEPP << _byoYomiTime << endl;
	for ( clients_t::HIterator it = _clients.begin(); it != _clients.end(); ++ it ) {
		if ( *it != clientInfo_ ) {
			*clientInfo_->_socket << *this
					<< PROTOCOL::PLAYER << PROTOCOL::SEP
					<< (*it)->_login << endl;
			*clientInfo_->_socket << *this
					<< PROTOCOL::MSG << PROTOCOL::SEP
					<< "Player " << (*it)->_login << " approached this table." << endl;
		}
	}
	_adminQueue.push_back( clientInfo_ );
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< clientInfo_->_login << endl << _out );
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << clientInfo_->_login << " approached this table." << endl << _out );
	send_contestants( clientInfo_ );
	send_goban( clientInfo_ );
	return;
	M_EPILOG
}

void HGo::do_kick( OClientInfo* clientInfo_ ) {
	M_PROLOG
	HLock l( _mutex );
	bool newadmin( false );
	admin_queue_t::iterator it( find( _adminQueue.begin(), _adminQueue.end(), clientInfo_ ) );
	if ( it == _adminQueue.begin() )
		newadmin = true;
	_adminQueue.erase( it );
	if ( newadmin ) {
		it = _adminQueue.begin();
		while ( ( it != _adminQueue.end() ) && ! (*it)->_valid )
			++ it;
		if ( it != _adminQueue.end() ) {
			try {
				*((*it)->_socket) << *this
					<< PROTOCOL::SETUP << PROTOCOL::SEP << PROTOCOL::ADMIN << endl;
			} catch ( HOpenSSLException const& ) {
				drop_client( *it );
			}
		}
	}
	if ( ( contestant( STONE::BLACK )._client == clientInfo_ )
			|| ( contestant( STONE::WHITE )._client == clientInfo_ ) ) {
		STONE::stone_t stone = ( contestant( STONE::BLACK )._client == clientInfo_ ? STONE::BLACK : STONE::WHITE );
		contestant_gotup( contestant( stone )._client );
		send_contestants();
	}
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << clientInfo_->_login << " left this match." << endl << _out );
	return;
	M_EPILOG
}

yaal::hcore::HString HGo::do_get_info( void ) const {
	HLock l( _mutex );
	return ( HString( "go," ) + get_comment() + "," + _gobanSize + "," + _komi100 + "," + _handicaps + "," + _mainTime + "," + _byoYomiPeriods + "," + _byoYomiTime );
}

void HGo::schedule_timeout( void ) {
	M_PROLOG
	++ _move;
	OPlayerInfo& p( contestant( _toMove ) );
	if ( p._byoYomiPeriods < _byoYomiPeriods )
		p._timeLeft = _byoYomiTime;
	HScheduledAsyncCaller::get_instance().call_in( duration( p._timeLeft, time::UNIT::SECOND ), call( &HGo::on_timeout, this ) );
	return;
	M_EPILOG
}

void HGo::on_timeout( void ) {
	M_PROLOG
	HLock l( _mutex );
	OPlayerInfo& p( contestant( _toMove ) );
	-- p._byoYomiPeriods;
	if ( p._byoYomiPeriods < 0 ) {
		OPlayerInfo& lost( _contestants[_toMove == STONE::BLACK ? 0 : 1 ] );
		OPlayerInfo& won( _contestants[_toMove == STONE::BLACK ? 1 : 0 ] );
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< lost._client->_login << ( _toMove == STONE::BLACK ? " (black)" : " (white)" )
				<< " has run out of time - therefore " << won._client->_login << ( _toMove == STONE::BLACK ? " (white)" : " (black)" )
				<< " wins." << endl << _out );
		end_match();
		after_move();
	} else {
		p._timeLeft = _byoYomiTime;
		schedule_timeout();
		send_contestants();
	}
	return;
	M_EPILOG
}

void HGo::reset_goban( bool sgf_ ) {
	M_PROLOG
	if ( sgf_ ) {
		_sgf.clear();
		_sgf.set_info( _gobanSize, _handicaps, _komi100, _mainTime, _byoYomiPeriods, _byoYomiTime );
	}
	_contestants[0].reset( _mainTime, 0, _byoYomiPeriods );
	_contestants[1].reset( _mainTime, _komi100, _byoYomiPeriods );
	::memset( _game.raw(), STONE::NONE, static_cast<int unsigned>( _gobanSize * _gobanSize ) );
	_game.get<char>()[ _gobanSize * _gobanSize ] = 0;
	::memset( _koGame.raw(), STONE::NONE, static_cast<int unsigned>( _gobanSize * _gobanSize ) );
	_koGame.get<char>()[ _gobanSize * _gobanSize ] = 0;
	::memset( _oldGame.raw(), STONE::NONE, static_cast<int unsigned>( _gobanSize * _gobanSize ) );
	_oldGame.get<char>()[ _gobanSize * _gobanSize ] = 0;
	if ( _handicaps > 1 ) {
		put_handicap_stones( _handicaps, sgf_ );
	}
	_toMove = _handicaps <= 1 ? STONE::BLACK : STONE::WHITE;
	_marking = false;
	_pendingUndoRequest = NULL;
	return;
	M_EPILOG
}

void HGo::apply_move( sgf::SGF::game_tree_t::const_node_t node_ ) {
	M_PROLOG
	if ( (*node_)->type() == SGF::Move::TYPE::SETUP ) {
		SGF::Setup const* s( (*node_)->setup() );
		SGF::Setup::setup_t::const_iterator remove( s->_data.find( SGF::Position::REMOVE ) );
		if ( remove != s->_data.end() ) {
			for ( SGF::Setup::coords_t::const_iterator it( remove->second.begin() ), end( remove->second.end() ); it != end; ++ it )
				put_stone( it->col(), it->row(), STONE::NONE );
		}
		SGF::Setup::setup_t::const_iterator black( s->_data.find( SGF::Position::BLACK ) );
		if ( black != s->_data.end() ) {
			for ( SGF::Setup::coords_t::const_iterator it( black->second.begin() ), end( black->second.end() ); it != end; ++ it )
				put_stone( it->col(), it->row(), STONE::BLACK );
		}
		SGF::Setup::setup_t::const_iterator white( s->_data.find( SGF::Position::WHITE ) );
		if ( white != s->_data.end() ) {
			for ( SGF::Setup::coords_t::const_iterator it( white->second.begin() ), end( white->second.end() ); it != end; ++ it )
				put_stone( it->col(), it->row(), STONE::BLACK );
		}
	} else {
		int time( (*node_)->time() );
		if ( time ) {
			if ( time > 0 )
				contestant( _toMove )._timeLeft = time;
			else
				contestant( _toMove )._byoYomiPeriods = -time;
		}
		if ( (*node_)->coord() != SGF::PASS )
			make_move( (*node_)->col(), (*node_)->row() );
		else
			pass();
	}
	return;
	M_EPILOG
}

void HGo::set_handicaps( int handicaps_ ) {
	M_PROLOG
	if ( ( handicaps_ > 9 ) || ( handicaps_ < 0 ) )
		throw HLogicException( _out << "bad handicap value: " << handicaps_ << _out );
	if ( handicaps_ != _handicaps ) {
		if ( handicaps_ > 0 )
			_komi100 = 0;
		else
			_komi100 = setup._komi100;
		_handicaps = handicaps_;
		broadcast( _out << PROTOCOL::SETUP << PROTOCOL::SEP
				<< PROTOCOL::KOMI << PROTOCOL::SEPP << _komi100 << endl << _out );
	}
	return;
	M_EPILOG
}

void HGo::put_handicap_stones( int handi_, bool sgf_ ) {
	M_PROLOG
	int hoshi( 3 - ( _gobanSize == GOBAN_SIZE::TINY ? 1 : 0 ) );
	int col( 0 );
	int row( 0 );
	switch ( handi_ ) {
		case ( 9 ):
			put_stone( col = _gobanSize / 2, row = _gobanSize / 2, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		case ( 8 ):
			put_handicap_stones( 6, sgf_ );
			put_stone( col = _gobanSize / 2, row = hoshi, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
			put_stone( col = _gobanSize / 2, row = ( _gobanSize - hoshi ) - 1, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		break;
		case ( 7 ):
			put_stone( col = _gobanSize / 2, row = _gobanSize / 2, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		case ( 6 ):
			put_handicap_stones( 4, sgf_ );
			put_stone( col = hoshi, row = _gobanSize / 2, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
			put_stone( col = ( _gobanSize - hoshi ) - 1, row = _gobanSize / 2, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		break;
		case ( 5 ):
			put_stone( col = _gobanSize / 2, row = _gobanSize / 2, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		case ( 4 ):
			put_stone( col = ( _gobanSize - hoshi ) - 1, row = ( _gobanSize - hoshi ) - 1, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		case ( 3 ):
			put_stone( col = hoshi, row = hoshi, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		case ( 2 ):
			put_stone( col = hoshi, row = ( _gobanSize - hoshi ) - 1, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
			put_stone( col = ( _gobanSize - hoshi ) - 1, row = hoshi, STONE::BLACK );
			if ( sgf_ )
				_sgf.add_position( SGF::Position::BLACK, SGF::Coord( col, row ) );
		break;
		default:
			M_ASSERT( ! "unhandled case" );
	}
	return;
	M_EPILOG
}

void HGo::put_stone( int col_, int row_, STONE::stone_t stone_ ) {
	M_PROLOG
	_game.get<char>()[ row_ * _gobanSize + col_ ] = stone_;
	return;
	M_EPILOG
}

void HGo::send_path( OClientInfo* clientInfo_ ) {
	M_PROLOG
	_path.clear();
	SGF::game_tree_t::const_node_t n( _sgf.get_current_move() );
	int moveNo( 0 );
	while ( n ) {
		SGF::game_tree_t::const_node_t p( n->get_parent() );
		if ( p ) {
			if ( p->child_count() > 1 ) {
				int idx( 0 );
				for ( SGF::game_tree_t::HNode::const_iterator it( p->begin() ); &*it != n; ++ it, ++ idx )
					;
				_path.push_back( idx );
			}
			++ moveNo;
		}
		n = p;
	}
	_out << PROTOCOL::SELECT << PROTOCOL::SEP << moveNo;
	for ( int i( static_cast<int>( _path.get_size() - 1 ) ); i >= 0; -- i )
		_out << ',' << _path[i];
	_out << endl;
	if ( clientInfo_ )
		*clientInfo_->_socket << *this << _out.consume();
	else
		broadcast( _out.consume() );
	return;
	M_EPILOG
}

void HGo::send_goban( OClientInfo* clientInfo_ ) {
	M_PROLOG
	_out << PROTOCOL::SGF << PROTOCOL::SEP;
	_sgf.save( _out, true );
	_out << endl;
	if ( clientInfo_ )
		*clientInfo_->_socket << *this << _out.consume();
	else
		broadcast( _out.consume() );
	send_path( clientInfo_ );
	return;
	M_EPILOG
}

char& HGo::goban( int col_, int row_ ) {
	return ( _koGame.get<char>()[ row_ * _gobanSize + col_ ] );
}

bool HGo::have_liberties( int col_, int row_, STONE::stone_t stone ) {
	if ( ( col_ < 0 ) || ( col_ > ( _gobanSize - 1 ) )
			|| ( row_ < 0 ) || ( row_ > ( _gobanSize - 1 ) ) )
		return ( false );
	if ( goban( col_, row_ ) == STONE::NONE )
		return ( true );
	if ( goban( col_, row_ ) == stone ) {
		goban( col_, row_ ) = static_cast<char>( toupper( stone ) );
		return ( have_liberties( col_, row_ - 1, stone )
				|| have_liberties( col_, row_ + 1, stone )
				|| have_liberties( col_ - 1, row_, stone )
				|| have_liberties( col_ + 1, row_, stone ) );
	}
	return ( false );
}

void HGo::clear_goban( bool removeDead ) {
	for ( int i = 0; i < _gobanSize; i++ ) {
		for ( int j = 0; j < _gobanSize; j++ ) {
			if ( goban( i, j ) != STONE::NONE ) {
				if ( removeDead && isupper( goban( i, j ) ) )
					goban( i, j ) = STONE::NONE;
				else
					goban( i, j ) = static_cast<char>( tolower( goban( i, j ) ) );
			}
		}
	}
}

HGo::STONE::stone_t HGo::opponent( STONE::stone_t stone ) {
	return ( stone == STONE::WHITE ? STONE::BLACK : STONE::WHITE );
}

bool HGo::have_killed( int x, int y, STONE::stone_t stone ) {
	bool killed = false;
	STONE::stone_t foeStone = opponent( stone );
	goban( x, y ) = stone;
	if ( ( x > 0 ) && ( goban( x - 1, y ) == foeStone ) && ( ! have_liberties( x - 1, y, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	if ( ( x < ( _gobanSize - 1 ) ) && ( goban( x + 1, y ) == foeStone ) && ( ! have_liberties( x + 1, y, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	if ( ( y > 0 ) && ( goban( x, y - 1 ) == foeStone ) && ( ! have_liberties( x, y - 1, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	if ( ( y < ( _gobanSize - 1 ) ) && ( goban( x, y + 1 ) == foeStone ) && ( ! have_liberties( x, y + 1, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	goban( x, y ) = STONE::NONE;
	return ( killed );
}

bool HGo::is_ko( void ) {
	return ( ::memcmp( _koGame.raw(), _oldGame.raw(), static_cast<int unsigned>( _gobanSize * _gobanSize ) ) == 0 );
}

bool HGo::is_suicide( int x, int y, STONE::stone_t stone ) {
	bool suicide = false;
	goban( x, y ) = stone;
	if ( ! have_liberties( x, y, stone ) ) {
		suicide = true;
	}
	clear_goban( false );
	goban( x, y ) = STONE::NONE;
	return ( suicide );
}

void HGo::ensure_coordinates_validity( int x, int y ) {
	if ( ( x < 0 ) || ( x > ( _gobanSize - 1 ) )
			|| ( y < 0 ) || ( y > ( _gobanSize - 1 ) ) ) {
		throw HLogicException( "move outside goban" );
	}
}

void HGo::make_move( int x, int y ) {
	M_PROLOG
	int before = count_stones( opponent( _toMove ) );
	ensure_coordinates_validity( x, y );
	::memcpy( _koGame.raw(), _game.raw(), static_cast<int unsigned>( _gobanSize * _gobanSize ) );
	if ( goban( x, y ) != STONE::NONE )
		throw HLogicException( "position already occupied" );
	if ( ! have_killed( x, y, _toMove ) ) {
		if ( is_suicide( x, y, _toMove ) ) {
			clear_goban( false );
			throw HLogicException( "suicides forbidden" );
		}
	}
	goban( x, y ) = _toMove;
	if ( is_ko() ) {
		throw HLogicException( "forbidden by ko rule" );
	}
	::memcpy( _oldGame.raw(), _game.raw(), static_cast<int unsigned>( _gobanSize * _gobanSize ) );
	commit();
	int after = count_stones( opponent( _toMove ) );
	contestant( _toMove )._stonesCaptured += ( before - after );
	_toMove = opponent( _toMove );
	return;
	M_EPILOG
}

void HGo::commit( void ) {
	M_PROLOG
	::memcpy( _game.raw(), _koGame.raw(), static_cast<int unsigned>( _gobanSize * _gobanSize ) );
	return;
	M_EPILOG
}

void HGo::mark_stone_dead( int col, int row ) {
	M_PROLOG
	STONE::stone_t stone = goban( col, row );
	switch ( stone ) {
		case ( STONE::BLACK ) : stone = STONE::DEAD_BLACK; break;
		case ( STONE::WHITE ) : stone = STONE::DEAD_WHITE; break;
		case ( STONE::DEAD_BLACK ) : stone = STONE::BLACK; break;
		case ( STONE::DEAD_WHITE ) : stone = STONE::WHITE; break;
		default:
			M_ASSERT( ! "predicate error for switch( stone )" );
	}
	goban( col, row ) = stone;
	commit();
	return;
	M_EPILOG
}

HGo::OPlayerInfo& HGo::contestant( STONE::stone_t stone ) {
	M_ASSERT( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE )
			|| ( stone == STONE::DEAD_BLACK ) || ( stone == STONE::DEAD_WHITE ) );
	return ( ( stone == STONE::BLACK ) || ( stone == STONE::DEAD_BLACK ) ? _contestants[ 0 ] : _contestants[ 1 ] );
}

HGo::OPlayerInfo const& HGo::contestant( STONE::stone_t stone ) const {
	M_ASSERT( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE )
			|| ( stone == STONE::DEAD_BLACK ) || ( stone == STONE::DEAD_WHITE ) );
	return ( ( stone == STONE::BLACK ) || ( stone == STONE::DEAD_BLACK ) ? _contestants[ 0 ] : _contestants[ 1 ] );
}

void HGo::contestant_gotup( OClientInfo* clientInfo_ ) {
	STONE::stone_t stone = ( contestant( STONE::BLACK )._client == clientInfo_ ? STONE::BLACK : STONE::WHITE );
	if ( ongoing_match() ) {
		OPlayerInfo& foe( contestant( opponent( stone ) ) );
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< clientInfo_->_login << ( stone == STONE::BLACK ? " (black)" : " (white)" )
				<< " resigned - therefore " << foe._client->_login << ( stone == STONE::BLACK ? " (white)" : " (black)" )
				<< " wins." << endl << _out );
	}
	contestant( stone )._client = NULL;
	_marking = false;
	_pendingUndoRequest = NULL;
	return;
}

void HGo::send_contestants( OClientInfo* clientInfo_ ) {
	M_PROLOG
	send_contestant( STONE::BLACK, clientInfo_ );
	send_contestant( STONE::WHITE, clientInfo_ );
	return;
	M_EPILOG
}

void HGo::send_contestant( char stone, OClientInfo* clientInfo_ ) {
	M_PROLOG
	OPlayerInfo& info( contestant( stone ) );
	char const* name = "";
	int captured( 0 );
	int time( 0 );
	int byoyomi( 0 );
	int score( 0 );
	if ( info._client ) {
		name = info._client->_login.raw();
		captured = info._stonesCaptured;
		time = static_cast<int>( info._timeLeft );
		byoyomi = info._byoYomiPeriods;
		score = info._score;
	}
	_out << PROTOCOL::CONTESTANT << PROTOCOL::SEP
			<< stone << PROTOCOL::SEPP
			<< name << PROTOCOL::SEPP
			<< captured << PROTOCOL::SEPP
			<< score << PROTOCOL::SEPP
			<< time << PROTOCOL::SEPP
			<< byoyomi << endl;
	if ( clientInfo_ )
		*clientInfo_->_socket << *this << _out.consume();
	else
		broadcast( _out.consume() );
	return;
	M_EPILOG
}

int HGo::count_stones( STONE::stone_t stone ) {
	M_PROLOG
	return ( static_cast<int>( count( _game.get<char>(), _game.get<char>() + _gobanSize * _gobanSize, stone ) ) );
	M_EPILOG
}

void HGo::replace_stones( STONE::stone_t which, STONE::stone_t with ) {
	M_PROLOG
	replace( _koGame.get<char>(), _koGame.get<char>() + _gobanSize * _gobanSize, which, with );
	return;
	M_EPILOG
}

bool HGo::is_admin( OClientInfo* clientInfo_ ) const {
	M_PROLOG
	return ( ! _adminQueue.is_empty() && ( _adminQueue.head() == clientInfo_ ) );
	M_EPILOG
}

bool HGo::can_play( OClientInfo* clientInfo_ ) const {
	M_PROLOG
	M_ASSERT( ( _toMove == STONE::BLACK ) || ( _toMove == STONE::WHITE ) );
	return ( ongoing_match()
			&& ( ( contestant( STONE::BLACK )._client == clientInfo_ )
				|| ( contestant( STONE::WHITE )._client == clientInfo_ ) ) );
	M_EPILOG
}

bool HGo::can_setup( OClientInfo* clientInfo_ ) const {
	M_PROLOG
	M_ASSERT( ( _toMove == STONE::BLACK ) || ( _toMove == STONE::WHITE ) );
	return ( ! ongoing_match() && is_admin( clientInfo_ ) );
	M_EPILOG
}

bool HGo::ongoing_match( void ) const {
	return ( _contestants[0]._client && _contestants[1]._client );
}

void HGo::update_clocks( void ) {
	M_PROLOG
	revoke_scheduled_tasks();
	int long now( ::time( NULL ) );
	OPlayerInfo& p( contestant( opponent( _toMove ) ) );
	if ( _start ) {
		p._timeLeft -= static_cast<int>( now - _start );
	}
	schedule_timeout();
	_start = now;
	return;
	M_EPILOG
}

void HGo::revoke_scheduled_tasks( void ) {
	M_PROLOG
	HScheduledAsyncCaller::get_instance().flush( this );
	return;
	M_EPILOG
}

}

namespace logic_factory {

class HGoCreator : public HLogicCreatorInterface {
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual HString do_get_info( void ) const;
} goCreator;

HLogic::ptr_t HGoCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	OUT << "creating logic: " << argv_ << endl;
	HString name = get_token( argv_, ",", 0 );
	return ( make_pointer<go::HGo>( server_, id_, name ) );
	M_EPILOG
}

HString HGoCreator::do_get_info( void ) const {
	M_PROLOG
	HString setupMsg;
	setupMsg.format( "go:%d,%d,%d,%d,%d,%d", setup._gobanSize, setup._komi100, setup._handicaps,
			setup._mainTime, setup._byoYomiPeriods, setup._byoYomiTime );
	OUT << setupMsg << endl;
	return ( setupMsg );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.register_logic_creator( HTokenizer( goCreator.get_info(), ":" )[0], &goCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

