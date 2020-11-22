/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cctype>

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "gomokud.hxx"

#include "setup.hxx"
#include "client.hxx"
#include "logicfactory.hxx"
#include "spellchecker.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace sgf;

namespace gameground {

namespace gomoku {

HGomoku::STONE::stone_t const HGomoku::STONE::BLACK = 'b';
HGomoku::STONE::stone_t const HGomoku::STONE::WHITE = 'w';
HGomoku::STONE::stone_t const HGomoku::STONE::NONE = ' ';

char const* const HGomoku::PROTOCOL::SPECTATOR = "spectator";
char const* const HGomoku::PROTOCOL::PLAY = "play";
char const* const HGomoku::PROTOCOL::SGF = "sgf";
char const* const HGomoku::PROTOCOL::CONTESTANT = "contestant";
char const* const HGomoku::PROTOCOL::STONE = "stone";
char const* const HGomoku::PROTOCOL::TOMOVE = "to_move";
char const* const HGomoku::PROTOCOL::PUTSTONE = "put_stone";
char const* const HGomoku::PROTOCOL::SIT = "sit";
char const* const HGomoku::PROTOCOL::GETUP = "get_up";
char const* const HGomoku::PROTOCOL::FIVE_IN_A_ROW = "five_in_a_row";

int const GO_MSG_NOT_YOUR_TURN = 0;
int const GO_MSG_YOU_CANT_DO_IT_NOW = 1;
int const GO_MSG_MALFORMED = 2;
char const* const GO_MSG[] = {
	"not your turn",
	"you cannot do it now",
	"malformed packet"
};

HGomoku::HGomoku( HServer* server_, HLogic::id_t const& id_, HString const& comment_ )
	: HLogic( server_, id_, comment_ ),
	_state( STONE::NONE ),
	_move( 0 ), _start( 0 ),
	_game( GOBAN_SIZE * GOBAN_SIZE + sizeof ( '\0' ) ),
	_sgf( SGF::GAME_TYPE::GOMOKU, "gameground" ),
	_players(), _varTmpBuffer() {
	M_PROLOG
	_contestants[ 0 ] = _contestants[ 1 ] = nullptr;
	::memset( _game.raw(), STONE::NONE, GOBAN_SIZE * GOBAN_SIZE );
	_game.get<char>()[ GOBAN_SIZE * GOBAN_SIZE ] = 0;
	_handlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HGomoku::handler_play );
	return;
	M_EPILOG
}

HGomoku::~HGomoku ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HGomoku::broadcast_contestants( yaal::hcore::HString const& message_ ) {
	M_PROLOG
	M_ASSERT( _contestants[ 0 ] && _contestants[ 1 ] );
	_contestants[ 0 ]->send( message_ );
	_contestants[ 1 ]->send( message_ );
	return;
	M_EPILOG
}

void HGomoku::handler_sit( HClient* client_, HString const& message_ ) {
	M_PROLOG
	HString place = get_token( message_, ",", 1 );
	if ( place.get_length() < 1 ) {
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	} else {
		char stone = static_cast<char>( place[ 0 ].get() );
		if ( ( stone != STONE::BLACK ) && ( stone != STONE::WHITE ) ) {
			throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
		} else if ( ( contestant( STONE::BLACK ) == client_ )
				|| ( contestant( STONE::WHITE ) == client_ ) ) {
			throw HLogicException( "you were already sitting" );
		} else if ( contestant( stone ) != nullptr ) {
			client_->send( _out << *this
				<< PROTOCOL::MSG << PROTOCOL::SEP << "Some one was faster." << endl << _out );
		} else {
			contestant( stone ) = client_;
			HClient* black( contestant( STONE::BLACK ) );
			HClient* white( contestant( STONE::WHITE ) );
			if ( black && white ) {
				::memset( _game.raw(), STONE::NONE, GOBAN_SIZE * GOBAN_SIZE );
				_game.get<char>()[ GOBAN_SIZE * GOBAN_SIZE ] = 0;
				_start = 0;
				_move = 0;
				_state = STONE::BLACK;
				players_t::iterator blackIt( _players.insert( make_pair( black, 0 ) ).first );
				players_t::iterator whiteIt( _players.insert( make_pair( white, 0 ) ).first );
				_sgf.clear();
				_sgf.set_player( SGF::Player::BLACK, black->login() );
				_sgf.set_player( SGF::Player::WHITE, white->login() );
				_sgf.set_info( 15 );
				send_goban();
				broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << black->login() << PROTOCOL::SEPP << blackIt->second << endl << _out );
				broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << white->login() << PROTOCOL::SEPP << whiteIt->second << endl << _out );
				broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP << "The Gomoku match started." << endl << _out );
			}
		}
	}
	return;
	M_EPILOG
}

void HGomoku::handler_getup( HClient* client_, HString const& /*message_*/ ) {
	M_PROLOG
	contestant_gotup( client_ );
	_state = STONE::NONE;
	return;
	M_EPILOG
}

void HGomoku::handler_put_stone( HClient* client_, HString const& message_ ) {
	M_PROLOG
	if ( ( _state != STONE::BLACK ) && ( _state != STONE::WHITE ) )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	if ( contestant( _state ) != client_ )
		throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	int col = lexical_cast<int>( get_token( message_, ",", 1 ) );
	int row = lexical_cast<int>( get_token( message_, ",", 2 ) );
	make_move( col, row, _state );
	if ( _state != STONE::NONE )
		_state = opponent( _state );
	_sgf.move( SGF::Coord( col, row ) );
	send_goban();
	return;
	M_EPILOG
}

void HGomoku::handler_play( HClient* client_, HString const& message_ ) {
	M_PROLOG
	HLock l( _mutex );
	HString item = get_token( message_, ",", 0 );
	if ( ( item != PROTOCOL::SIT )
			&& ( contestant( STONE::BLACK ) != client_ )
			&& ( contestant( STONE::WHITE ) != client_ ) )
		throw HLogicException( "you are not playing" );
	if ( item == PROTOCOL::PUTSTONE )
		handler_put_stone( client_, message_ );
	else if ( item == PROTOCOL::SIT )
		handler_sit( client_, message_ );
	else if ( item == PROTOCOL::GETUP )
		handler_getup( client_, message_ );
	else
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	broadcast( _out << PROTOCOL::TOMOVE << PROTOCOL::SEP
			<< static_cast<char>( _state ) << endl << _out );
	send_contestants();
	return;
	M_EPILOG
}

bool HGomoku::do_accept( HClient* client_ ) {
	OUT << "new candidate " << client_->login() << endl;
	return ( false );
}

void HGomoku::do_post_accept( HClient* client_ ) {
	M_PROLOG
	HLock l( _mutex );
	broadcast( _out << PROTOCOL::SPECTATOR << PROTOCOL::SEP << client_->login() << endl << _out );
	for ( clients_t::HIterator it = _clients.begin(); it != _clients.end(); ++ it ) {
		if ( *it != client_ ) {
			client_->send( _out << *this
					<< PROTOCOL::SPECTATOR << PROTOCOL::SEP
					<< (*it)->login() << endl << _out );
			client_->send( _out << *this
					<< PROTOCOL::MSG << PROTOCOL::SEP
					<< "Spectator " << (*it)->login() << " approached this table." << endl << _out );
		}
	}
	for ( players_t::iterator it = _players.begin(); it != _players.end(); ++ it ) {
		if ( it->first != client_ ) {
			client_->send( _out << *this
					<< PROTOCOL::PLAYER << PROTOCOL::SEP
					<< it->first->login() << PROTOCOL::SEPP << it->second << endl << _out );
		}
	}
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Spectator " << client_->login() << " approached this table." << endl << _out );
	send_contestants();
	send_goban();
	return;
	M_EPILOG
}

void HGomoku::do_kick( HClient* client_ ) {
	M_PROLOG
	HLock l( _mutex );
	_players.erase( client_ );
	if ( ( contestant( STONE::BLACK ) == client_ )
			|| ( contestant( STONE::WHITE ) == client_ ) ) {
		STONE::stone_t stone = ( contestant( STONE::BLACK ) == client_ ? STONE::BLACK : STONE::WHITE );
		contestant_gotup( contestant( stone ) );
		send_contestants();
	}
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << client_->login() << " left this match." << endl << _out );
	return;
	M_EPILOG
}

yaal::hcore::HString HGomoku::do_get_info() const {
	HLock l( _mutex );
	return ( HString( "gomoku," ) + get_comment() + "," + _players.size() );
}

void HGomoku::send_goban( void ) {
	M_PROLOG
	_out << PROTOCOL::SGF << PROTOCOL::SEP;
	_sgf.save( _out, true );
	broadcast( _out << endl << _out );
	return;
	M_EPILOG
}

char& HGomoku::goban( int col_, int row_ ) {
	return ( _game.get<char>()[ row_ * GOBAN_SIZE + col_ ] );
}

char HGomoku::goban( int col_, int row_ ) const {
	return ( _game.get<char>()[ row_ * GOBAN_SIZE + col_ ] );
}

void HGomoku::clear_goban( bool removeDead ) {
	for ( int i = 0; i < GOBAN_SIZE; i++ ) {
		for ( int j = 0; j < GOBAN_SIZE; j++ ) {
			if ( goban( i, j ) != STONE::NONE ) {
				if ( removeDead && isupper( goban( i, j ) ) )
					goban( i, j ) = STONE::NONE;
				else
					goban( i, j ) = static_cast<char>( tolower( goban( i, j ) ) );
			}
		}
	}
}

HGomoku::STONE::stone_t HGomoku::opponent( STONE::stone_t stone ) {
	return ( stone == STONE::WHITE ? STONE::BLACK : STONE::WHITE );
}

void HGomoku::ensure_coordinates_validity( int x, int y ) {
	if ( ( x < 0 ) || ( x > ( GOBAN_SIZE - 1 ) )
			|| ( y < 0 ) || ( y > ( GOBAN_SIZE - 1 ) ) )
		throw HLogicException( "move outside goban" );
}

int HGomoku::is_winning_stone( int row_, int col_ ) const {
	M_PROLOG
	int line( -1 );
	do {
		char stone( goban( row_, col_ ) );
		int l( 0 );
		for ( int r( row_ ), c( col_ ); ( r < GOBAN_SIZE ) && ( c < GOBAN_SIZE ) && ( l <= FIVE_IN_A_ROW ) && ( goban( r, c ) == stone ); ++ c, ++ l )
			;
		if ( l == FIVE_IN_A_ROW ) {
			line = 0;
			break;
		}
		l = 0;
		for ( int r( row_ ), c( col_ ); ( r < GOBAN_SIZE ) && ( c < GOBAN_SIZE ) && ( l <= FIVE_IN_A_ROW ) && ( goban( r, c ) == stone ); ++ r, ++ l )
			;
		if ( l == FIVE_IN_A_ROW ) {
			line = 1;
			break;
		}
		l = 0;
		for ( int r( row_ ), c( col_ ); ( r < GOBAN_SIZE ) && ( c < GOBAN_SIZE ) && ( l <= FIVE_IN_A_ROW ) && ( goban( r, c ) == stone ); ++ r, ++ c, ++ l )
			;
		if ( l == FIVE_IN_A_ROW ) {
			line = 2;
			break;
		}
		l = 0;
		for ( int r( row_ ), c( col_ ); ( r < GOBAN_SIZE ) && ( c >= 0 ) && ( l <= FIVE_IN_A_ROW ) && ( goban( r, c ) == stone ); ++ r, -- c, ++ l )
			;
		if ( l == FIVE_IN_A_ROW ) {
			line = 3;
			break;
		}
	} while ( false );
	return ( line );
	M_EPILOG
}

void HGomoku::make_move( int x, int y, STONE::stone_t stone ) {
	M_PROLOG
	ensure_coordinates_validity( x, y );
	goban( x, y ) = stone;
	for ( int r( 0 ); r < GOBAN_SIZE; ++ r ) {
		for ( int c( 0 ); c < GOBAN_SIZE; ++ c ) {
			if ( goban( r, c ) == stone ) {
				int line = is_winning_stone( r, c );
				if ( line >= 0 ) {
					_state = STONE::NONE;
					broadcast( _out << PROTOCOL::FIVE_IN_A_ROW << PROTOCOL::SEP << stone << PROTOCOL::SEPP << r << PROTOCOL::SEPP << c << PROTOCOL::SEPP << line << endl << _out );
					HClient* winner( contestant( stone ) );
					int score( ++ _players[ winner ] );
					broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << winner->login() << PROTOCOL::SEPP << score << endl << _out );
					broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP << "The match has ended!" << endl << _out );
					broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP << "The contestant " << winner->login() << " won this match." << endl << _out );
					contestant( STONE::BLACK ) = nullptr;
					contestant( STONE::WHITE ) = nullptr;
				}
			}
		}
	}
	return;
	M_EPILOG
}

HClient*& HGomoku::contestant( STONE::stone_t stone ) {
	M_ASSERT( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE ) );
	return ( ( stone == STONE::BLACK ) ? _contestants[ 0 ] : _contestants[ 1 ] );
}

void HGomoku::contestant_gotup( HClient* client_ ) {
	STONE::stone_t stone = ( contestant( STONE::BLACK ) == client_ ? STONE::BLACK : STONE::WHITE );
	HClient* foe = nullptr;
	if ( ( _state != STONE::NONE ) && ( foe = contestant( opponent( stone ) ) ) ) {
		int score( ++ _players[ foe ] );
		broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP << foe->login() << PROTOCOL::SEPP << score << endl << _out );
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< client_->login() << " resigned - therefore " << foe->login() << " wins." << endl << _out );
	}
	contestant( stone ) = nullptr;
	_state = STONE::NONE;
	return;
}

void HGomoku::send_contestants( void ) {
	M_PROLOG
	send_contestant( STONE::BLACK );
	send_contestant( STONE::WHITE );
	return;
	M_EPILOG
}

void HGomoku::send_contestant( char stone ) {
	M_PROLOG
	HClient* cinfo = contestant( stone );
	broadcast( _out << PROTOCOL::CONTESTANT << PROTOCOL::SEP
			<< stone << PROTOCOL::SEPP
			<< ( cinfo ? cinfo->login() : "" )
			<< PROTOCOL::SEPP << endl << _out );
	return;
	M_EPILOG
}

}

namespace logic_factory {

class HGomokuCreator : public HLogicCreatorInterface {
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual HString do_get_info( void ) const;
} gomokuCreator;

HLogic::ptr_t HGomokuCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	OUT << "creating logic: " << argv_ << endl;
	HString name = get_token( argv_, ",", 0 );
	return ( make_pointer<gomoku::HGomoku>( server_, id_, name ) );
	M_EPILOG
}

HString HGomokuCreator::do_get_info( void ) const {
	M_PROLOG
	HString setup;
	setup.assign( "gomoku" );
	OUT << setup << endl;
	return ( setup );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.register_logic_creator( HTokenizer( gomokuCreator.get_info(), ":"  )[0], &gomokuCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

