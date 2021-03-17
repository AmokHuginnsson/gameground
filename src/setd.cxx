/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hformat.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/util.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "setd.hxx"

#include "setup.hxx"
#include "client.hxx"
#include "logicfactory.hxx"
#include "spellchecker.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground {

namespace set_bang {

int const SET_MSG_MALFORMED = 0;
int const SET_MSG_NOT_A_SET = 1;
char const* const SET_MSG[] = {
	"malformed packet",
	"not a set"
};

char const* const HSetBang::PROTOCOL::DECK = "deck";
char const* const HSetBang::PROTOCOL::SET = "set";
int const HSetBang::SET_TABLE_CARD_COUNT;

HSetBang::HSetBang( HServer* server_, HLogic::id_t const& id_, HString const& comment_, int startupPlayers_,
		int deckCount_, int interRoundDelay_ )
	: HLogic( server_, id_, comment_ ), _startupPlayers( startupPlayers_ ),
	_cardsOnTable( 0 ), _cardsInDeck( 0 ), _deckNo( 0 ), _deckCount( deckCount_ ),
	_interRoundDelay( interRoundDelay_ ), _players(), _deck(), _varTmpBuffer() {
	M_PROLOG
	_handlers[ PROTOCOL::SET ] = static_cast<handler_t>( &HSetBang::handler_set );
	return;
	M_EPILOG
}

HSetBang::~HSetBang ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HSetBang::handler_set( HClient* client_, HString const& message_ ) {
	M_PROLOG
	HTokenizer t( message_, "," );
	int val[3];
	for ( int i( 0 ); i < 3; ++ i ) {
		try {
			val[ i ] = lexical_cast<int>( t[ i ] );
		} catch ( HLexicalCastException const& ) {
			throw HLogicException( SET_MSG[ SET_MSG_MALFORMED ] );
		}
	}
	for ( int i( 0 ); i < 3; ++ i ) {
		if ( ( val[ i ] < 0 ) || ( val[ i ] >= SET_TABLE_CARD_COUNT ) || ( _deck[ val[ i ] ] == -1 ) )
			throw HLogicException( SET_MSG[ SET_MSG_MALFORMED ] );
	}
	if ( ! makes_set( _deck[ val[ 0 ] ], _deck[ val[ 1 ] ], _deck[ val[ 2 ] ] ) )
		throw HLogicException( SET_MSG[ SET_MSG_NOT_A_SET ] );
	_deck[ val[ 0 ] ] = -1;
	_deck[ val[ 1 ] ] = -1;
	_deck[ val[ 2 ] ] = -1;
	OPlayerInfo& p( *get_player_info( client_ ) );
	++ p._score;
	++ p._last;
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< client_->login() << PROTOCOL::SEPP << p._score << PROTOCOL::SEPP << p._last << endl << _out );
	update_table();
	return;
	M_EPILOG
}

bool HSetBang::do_accept( HClient* client_ ) {
	OUT << "new candidate " << client_->login() << endl;
	return ( false );
}

void HSetBang::do_post_accept( HClient* client_ ) {
	M_PROLOG
	OUT << "conditions: _players.size() = " << _players.size() << ", _startupPlayers = " << _startupPlayers << endl;
	OPlayerInfo info;
	_players[ client_ ] = info;
	/*
	 * Send proto info about new contestant to all players.
	 */
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< client_->login() << PROTOCOL::SEPP << 0 << PROTOCOL::SEPP << 0 << endl << _out );
	for ( players_t::iterator it = _players.begin(); it != _players.end(); ++ it ) {
		if ( it->first != client_ ) {
			client_->send( _out << *this
				<< PROTOCOL::PLAYER << PROTOCOL::SEP << it->first->login()
				<< PROTOCOL::SEPP << it->second._score << PROTOCOL::SEPP
				<< it->second._last << endl << _out );
			client_->send( _out << *this
				<< PROTOCOL::MSG << PROTOCOL::SEP << it->first->login() << " joined the mind contest." << endl << _out );
		}
	}
	broadcast(
			_out << PROTOCOL::MSG << PROTOCOL::SEP
			<< client_->login()
			<< " joined the mind contest." << endl << _out );
	OUT << "player [" << client_->login() << "] accepted" << endl;
	if ( ! _deckNo && ( _players.size() >= _startupPlayers ) ) {
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< "The match has begun, good luck!" << endl << _out );
		generate_deck();
		broadcast( _out << PROTOCOL::DECK << PROTOCOL::SEP << serialize_deck() << endl << _out );
	} else if ( _deckNo > 0 ) {
		client_->send( _out << *this << PROTOCOL::DECK << PROTOCOL::SEP << serialize_deck() << endl << _out );
	}
	return;
	M_EPILOG
}

HSetBang::OPlayerInfo* HSetBang::get_player_info( HClient* client_ ) {
	M_PROLOG
	players_t::iterator it = _players.find( client_ );
	M_ASSERT( it != _players.end() );
	return ( &it->second );
	M_EPILOG
}

bool HSetBang::makes_set( int n1_, int n2_, int n3_ ) const {
	bool makesSet( true );
	int b1( 0 );
	int b2( 0 );
	int b3( 0 );
	b1 = n1_ % 3;
	b2 = n2_ % 3;
	b3 = n3_ % 3;
	if ( ( ( b1 == b2 ) && ( b1 != b3 ) ) || ( ( b1 == b3 ) && ( b1 != b2 ) ) || ( ( b2 == b3 ) && ( b2 != b1 ) ) )
		makesSet = false;
	if ( makesSet ) {
		b1 = ( n1_ / 3 ) % 3;
		b2 = ( n2_ / 3 ) % 3;
		b3 = ( n3_ / 3 ) % 3;
		if ( ( ( b1 == b2 ) && ( b1 != b3 ) ) || ( ( b1 == b3 ) && ( b1 != b2 ) ) || ( ( b2 == b3 ) && ( b2 != b1 ) ) )
			makesSet = false;
		if ( makesSet ) {
			b1 = ( n1_ / 9 ) % 3;
			b2 = ( n2_ / 9 ) % 3;
			b3 = ( n3_ / 9 ) % 3;
			if ( ( ( b1 == b2 ) && ( b1 != b3 ) ) || ( ( b1 == b3 ) && ( b1 != b2 ) ) || ( ( b2 == b3 ) && ( b2 != b1 ) ) )
				makesSet = false;
			if ( makesSet ) {
				b1 = ( n1_ / 27 ) % 3;
				b2 = ( n2_ / 27 ) % 3;
				b3 = ( n3_ / 27 ) % 3;
				if ( ( ( b1 == b2 ) && ( b1 != b3 ) ) || ( ( b1 == b3 ) && ( b1 != b2 ) ) || ( ( b2 == b3 ) && ( b2 != b1 ) ) )
					makesSet = false;
			}
		}
	}
	return makesSet;
}

bool HSetBang::contains_set( int limit_ ) const {
	M_ASSERT( ( limit_ >= 3 ) && ! ( limit_ % 3 ) && ( limit_ <= max( _cardsInDeck, SET_TABLE_CARD_COUNT ) ) );
	bool containsSet( makes_set( _deck[ 0 ], _deck[ 1 ], _deck[ 2 ] ) );
	int lastCard( 3 );
	while ( ! containsSet && ( lastCard < limit_ ) ) {
		for ( int i( 0 ); ! containsSet && ( i < lastCard ); ++ i ) {
			if ( _deck[ i ] == -1 )
				continue;
			for ( int j( i + 1 ); j < lastCard; ++ j ) {
				if ( _deck[ j ] == -1 )
					continue;
				if ( ( containsSet = makes_set( _deck[ i ], _deck[ j ], _deck[ lastCard ] ) ) )
					break;
			}
		}
		++ lastCard;
	}
	return containsSet;
}

void HSetBang::update_table( void ) {
	M_PROLOG
	if ( _cardsInDeck >= ( SET_TABLE_CARD_COUNT + 3 ) ) {
		for ( int i( 0 ); i < SET_TABLE_CARD_COUNT; ++ i ) {
			if ( _deck[ i ] == -1 )
				_deck[ i ] = _deck[ -- _cardsInDeck ];
		}
	} else {
		_cardsOnTable -= 3;
		_cardsInDeck -= 3;
	}
	if ( _cardsInDeck >= SET_CERTAIN_SET ) {
		while ( ! contains_set( _cardsOnTable ) ) {
			random_shuffle( _deck, _deck + _cardsInDeck );
			OUT << "shuffle needed" << endl;
		}
	} else {
		if ( ! contains_set( max( _cardsInDeck, SET_TABLE_CARD_COUNT ) ) ) {
			if ( _deckNo < _deckCount )
				generate_deck();
			else
				broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
						<< "No more decks! The game is over!" << endl << _out );
		} else {
			while ( ! contains_set( _cardsOnTable ) ) {
				random_shuffle( _deck, _deck + _cardsInDeck );
				OUT << "shuffle needed" << endl;
			}
		}
	}
	broadcast( _out << PROTOCOL::DECK << PROTOCOL::SEP << serialize_deck() << endl << _out );
	return;
	M_EPILOG
}

void HSetBang::generate_deck( void ) {
	M_PROLOG
	++ _deckNo;
	for ( players_t::iterator it( _players.begin() ), end( _players.end() ); it != end; ++ it )
		it->second._last = 0;
	_cardsInDeck = SET_DECK_CARD_COUNT;
	for ( int i( 0 ); i < _cardsInDeck; ++ i )
		_deck[ i ] = i;
	_cardsOnTable = SET_TABLE_CARD_COUNT;
	do {
		random_shuffle( _deck, _deck + _cardsInDeck );
	} while ( ! contains_set( _cardsOnTable ) );
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "New deck started, " << ( _deckCount - _deckNo ) << " decks left." << endl << _out );
	return;
	M_EPILOG
}

HString const& HSetBang::serialize_deck( void ) {
	_varTmpBuffer.clear();
	for ( int i( 0 ); i < SET_TABLE_CARD_COUNT; ++ i ) {
		if ( i )
			_varTmpBuffer += ",";
		_varTmpBuffer += _deck[i];
	}
	return ( _varTmpBuffer );
}

void HSetBang::do_kick( HClient* client_ ) {
	M_PROLOG
	HLock l( _mutex );
	_players.erase( client_ );
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << client_->login() << " left this match." << endl << _out );
	return;
	M_EPILOG
}

yaal::hcore::HString HSetBang::do_get_info() const {
	HLock l( _mutex );
	return ( HString( "set_bang," ) + get_comment() + "," + _players.size() );
}

}

namespace logic_factory {

class HSetBangCreator : public HLogicCreatorInterface {
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual HString do_get_info( void ) const;
} setBangCreator;

HLogic::ptr_t HSetBangCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	OUT << "creating logic: " << argv_ << endl;
	HTokenizer t( argv_, "," );
	HString name( t[ 0 ] );
	int players( 0 );
	int deckCount( 0 );
	int interRoundDelay( 0 );
	try {
		players = lexical_cast<int>( t[ 1 ] );
		deckCount = lexical_cast<int>( t[ 2 ] );
		interRoundDelay = lexical_cast<int>( t[ 3 ] );
	} catch ( HLexicalCastException const& ) {
		throw HLogicException( set_bang::SET_MSG[set_bang::SET_MSG_MALFORMED] );
	}
	return ( make_pointer<set_bang::HSetBang>( server_, id_, name, players, deckCount, interRoundDelay ) );
	M_EPILOG
}

HString HSetBangCreator::do_get_info( void ) const {
	M_PROLOG
	HString setupMsg;
	setupMsg = format( "set_bang:%d,%d,%d", setup._setStartupPlayers, setup._setDeckCount, setup._interRoundDelay );
	OUT << setupMsg << endl;
	return setupMsg;
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.register_logic_creator( HTokenizer( setBangCreator.get_info(), ":"  )[0], &setBangCreator );
	return failed;
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

