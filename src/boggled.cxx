/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  boggled.cxx - this file is integral part of `gameground' project.

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

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/hscheduledasynccaller.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "boggled.hxx"

#include "setup.hxx"
#include "clientinfo.hxx"
#include "logicfactory.hxx"
#include "spellchecker.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace gameground {

namespace boggle_data {

static int const MAXIMUM_WORD_LENGTH = 16;

static char const _dices_[ 16 ][ 6 ] = {
		{ 'a', 'a', 'd', 'e', '³', 'y' },
		{ 'a', 'a', 'i', 'k', 'm', 'y' },
		{ 'a', 'c', 'i', 'n', 'n', 'p' },
		{ 'a', 'e', 'i', 'o', 'u', 'y' },
		{ 'a', 'e', 'o', 'y', 'z', 'z' },
		{ 'a', 'g', 'l', 'o', 'r', 's' },
		{ '±', 'c', 'g', 'm', 'ñ', 't' },
		{ 'b', 'e', 'e', 'p', 'z', '¼' },
		{ 'b', 'i', 'i', 'n', 'r', 'w' },
		{ 'c', 'o', 's', 't', 'w', '¿' },
		{ 'æ', 'j', 'm', 't', 'u', 'z' },
		{ 'd', 'e', 'k', 'n', 'o', 'r' },
		{ 'd', 'h', 'ó', 'n', 'w', 'z' },
		{ 'e', 'i', 'l', 's', '¶', 'w' },
		{ 'ê', 'f', 'h', 'j', 'l', 's' },
		{ 'i', 'k', '³', 'o', 'p', 'r' }
};

struct BOGGLE {
	enum {
		UNINITIALIZED_SLOT = -1,
		SIDES = 6,
		DICE_COUNT = 16
	};
};

}

namespace boggle {

char const* const HBoggle::PROTOCOL::NAME = "bgl";
char const* const HBoggle::PROTOCOL::PLAY = "play";
char const* const HBoggle::PROTOCOL::DECK = "deck";
char const* const HBoggle::PROTOCOL::SETUP = "setup";
char const* const HBoggle::PROTOCOL::END_ROUND = "end_round";
char const* const HBoggle::PROTOCOL::ROUND = "round";
char const* const HBoggle::PROTOCOL::SCORED = "scored";
char const* const HBoggle::PROTOCOL::LONGEST = "longest";
int HBoggle::RULES[6][16] = {
		{ 0, 0, 1, 1, 2, 3,  5, 11, 11,  11,  11,  11,   11,   11,   11,   11 },
		{ 0, 0, 1, 2, 3, 5,  8, 13, 21,  34,  55,  89,  144,  233,  377,  610 },
		{ 0, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 },
		{ 0, 0, 0, 1, 2, 3,  5,  8, 13,  21,  34,  55,   89,  144,  233,  377 },
		{ 0, 0, 0, 1, 2, 4,  8, 16, 32,  64, 128, 256,  512, 1024, 2048, 4096 },
		{ 0, 0, 1, 1, 1, 1,  1,  1,  1,   1,   1,   1,    1,    1,    1,    1 }
};

int const BOGGLE_MSG_MALFORMED = 0;
char const* const BOGGLE_MSG[] = {
	"malformed packet"
};

HBoggle::HBoggle( HServer* server_, id_t const& id_, HString const& comment_, SCORING::scoring_t scoring_, int players_, int roundTime_, int maxRounds_, int interRoundDelay_ )
	: HLogic( server_, id_, comment_ ), _state( STATE::LOCKED ),
	_scoring( scoring_ ), _startupPlayers( players_ ),
	_roundTime( roundTime_ ), _maxRounds( maxRounds_ ),
	_interRoundDelay( interRoundDelay_ ), _round( 0 ), _players(),
	_words(), _varTmpBuffer() {
	M_PROLOG
	_handlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HBoggle::handler_play );
	return;
	M_EPILOG
}

HBoggle::~HBoggle ( void ) {
	M_PROLOG
	HScheduledAsyncCaller::get_instance().flush( this );
	return;
	M_EPILOG
}

void HBoggle::generate_game( void ) {
	M_PROLOG
	HRandomizer rnd( randomizer_helper::make_randomizer() );
	for ( int i( 0 ); i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		_game[ i ][ 0 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
	for ( int i( 0 ); i < boggle_data::BOGGLE::DICE_COUNT; ++ i ) {
		int k( 0 );
		int slot( static_cast<int>( rnd( boggle_data::BOGGLE::DICE_COUNT - i ) ) );
		for ( int j( 0 ); j < slot; ++ j, ++ k )
			while ( _game[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
				++ k;
		while ( _game[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
			++ k;
		_game[ k ][ 0 ] = boggle_data::_dices_[ i ][ rnd( boggle_data::BOGGLE::SIDES ) ];
	}
	return;
	M_EPILOG
}

HString const& HBoggle::serialize_deck( void ) {
	M_PROLOG
	_varTmpBuffer.clear();
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		_varTmpBuffer += _game[ i ][ 0 ];
	out << "new deck: " << endl;
	cout << _varTmpBuffer.left( 4 ) << endl;
	cout << _varTmpBuffer.mid( 4, 4 ) << endl;
	cout << _varTmpBuffer.mid( 8, 4 ) << endl;
	cout << _varTmpBuffer.mid( 12, 4 ) << endl;
	return ( _varTmpBuffer );
	M_EPILOG
}

void HBoggle::handler_play( OClientInfo* clientInfo_, HString const& word_ ) {
	M_PROLOG
	HLock l( _mutex );
	static int const BOGGLE_REAL_MINIMUM_WORD_LENGTH = 3;
	int length = static_cast<int>( word_.get_length() );
	if ( ( _state == STATE::ACCEPTING )
			&& ( length >= BOGGLE_REAL_MINIMUM_WORD_LENGTH )
			&& ( length <= boggle_data::MAXIMUM_WORD_LENGTH ) ) {
		words_t::iterator it = _words.find( word_ );
		if ( it == _words.end() )
			it = _words.insert( hcore::make_pair( word_, make_pointer<client_set_t>() ) ).first;
		it->second->insert( clientInfo_ );
		out << "word: " << word_ << "< inserted, proof: " << _words.size() << "," << it->second->size() << endl;
	}
	return;
	M_EPILOG
}

HBoggle::OPlayerInfo* HBoggle::get_player_info( OClientInfo* clientInfo_ ) {
	M_PROLOG
	players_t::iterator it = _players.find( clientInfo_ );
	M_ASSERT( it != _players.end() );
	return ( &it->second );
	M_EPILOG
}

bool HBoggle::do_accept( OClientInfo* clientInfo_ ) {
	out << "new candidate " << clientInfo_->_login << endl;
	return ( false );
}

void HBoggle::do_post_accept( OClientInfo* clientInfo_ ) {
	M_PROLOG
	HLock l( _mutex );
	out << "conditions: _players.size() = " << _players.size() << ", _startupPlayers = " << _startupPlayers << endl;
	OPlayerInfo info;
	_players[ clientInfo_ ] = info;
	/*
	 * Send proto info about new contestant to all players.
	 */
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< clientInfo_->_login << PROTOCOL::SEPP << 0 << PROTOCOL::SEPP << 0 << endl << _out );
	char const* scoringStr = "";
	switch ( _scoring ) {
		case ( SCORING::ORIGINAL ): scoringStr = "Original Boggle"; break;
		case ( SCORING::FIBONACCI ): scoringStr = "Fibonacci"; break;
		case ( SCORING::GEOMETRIC ): scoringStr = "Geometric"; break;
		case ( SCORING::FIBONACCI_4 ): scoringStr = "Fibonacci 4-based"; break;
		case ( SCORING::GEOMETRIC_4 ): scoringStr = "Geometric 4-based"; break;
		case ( SCORING::LONGEST_WORDS ): scoringStr = "Longest Words"; break;
		default: break;
	}
	*clientInfo_->_socket << *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "Welcome, this match settings are:" << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   scoring system - " << scoringStr << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round time - " << _roundTime << " seconds" << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   number of rounds - " << _maxRounds << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round interval - " << _interRoundDelay << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "This match requires " << _startupPlayers << " players to start the game." << endl;
	*clientInfo_->_socket << *this
		<< PROTOCOL::SETUP << PROTOCOL::SEP << _roundTime
		<< PROTOCOL::SEPP << _interRoundDelay << endl;
	for ( players_t::iterator it = _players.begin(); it != _players.end(); ++ it ) {
		if ( it->first != clientInfo_ ) {
			*clientInfo_->_socket << *this
				<< PROTOCOL::PLAYER << PROTOCOL::SEP << it->first->_login
				<< PROTOCOL::SEPP << it->second._score << PROTOCOL::SEPP
				<< it->second._last << endl;
			*clientInfo_->_socket << *this
				<< PROTOCOL::MSG << PROTOCOL::SEP << it->first->_login << " joined the mind contest." << endl;
		}
	}
	broadcast(
			_out << PROTOCOL::MSG << PROTOCOL::SEP
			<< clientInfo_->_login
			<< " joined the mind contest." << endl << _out );
	out << "player [" << clientInfo_->_login << "] accepted" << endl;
	if ( ! _round && ( _players.size() >= _startupPlayers ) ) {
		schedule_end_round();
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< "The match has begun, good luck!" << endl << _out );
	} else if ( _round > 0 )
		*clientInfo_->_socket << *this << PROTOCOL::DECK << PROTOCOL::SEP << serialize_deck() << endl;
	return;
	M_EPILOG
}

void HBoggle::do_kick( OClientInfo* clientInfo_ ) {
	M_PROLOG
	HLock l( _mutex );
	_players.erase( clientInfo_ );
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ++ it ) {
		if ( it->second->size() > 1 )
			continue;
		if ( *(it->second->begin()) == clientInfo_ )
			it = _words.erase( it );
	}
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << clientInfo_->_login << " left this match." << endl << _out );
	return;
	M_EPILOG
}

yaal::hcore::HString HBoggle::do_get_info() const {
	HLock l( _mutex );
	return ( HString( "bgl," ) + get_comment() + "," + _startupPlayers + "," + _roundTime + "," + _maxRounds + "," + _interRoundDelay );
}

void HBoggle::schedule_end_round( void ) {
	M_PROLOG
	++ _round;
	HScheduledAsyncCaller::get_instance().call_in( time::duration( _roundTime, time::UNIT::SECOND ), call( &HBoggle::on_end_round, this ) );
	generate_game();
	_state = STATE::ACCEPTING;
	broadcast( _out << PROTOCOL::DECK << PROTOCOL::SEP << serialize_deck() << endl << _out );
	return;
	M_EPILOG
}

void HBoggle::on_begin_round( void ) {
	M_PROLOG
	HLock l( _mutex );
	out << "<<begin>>" << endl;
	schedule_end_round();
	broadcast(
			_out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "New round started, you have got " << _roundTime
			<< " seconds, " << _maxRounds - _round
			<< " rounds left!" << endl << _out );
	broadcast( _out << PROTOCOL::ROUND << PROTOCOL::SEP << _round << endl << _out );
	return;
	M_EPILOG
}

void HBoggle::on_end_round( void ) {
	M_PROLOG
	HLock l( _mutex );
	out << "<<end>>" << endl;
	_state = STATE::LOCKED;
	if ( _round < _maxRounds ) {
		HScheduledAsyncCaller::get_instance().call_in( duration( _interRoundDelay, time::UNIT::SECOND ), call( &HBoggle::on_begin_round, this ) );
		_out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "This round has ended, next round in " << _interRoundDelay << " seconds!" << endl;
	} else
		_out << PROTOCOL::MSG << PROTOCOL::SEP << "Game Over!" << endl;
	broadcast( _out << _out );
	int* scores = RULES[ _scoring ];
	typedef HList<words_t::iterator> longest_t;
	longest_t longest;
	int longestLength = 0;
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ) {
		words_t::iterator del( it );
		++ it;
		int appearance( static_cast<int>( del->second->size() ) );
		int length( static_cast<int>( del->first.get_length() ) );
		if ( _scoring == SCORING::LONGEST_WORDS ) {
			if ( ( scores[ length - 1 ] > 0 ) && word_is_good( del->first, length ) ) {
				if ( length > longestLength ) {
					longest.clear();
					longestLength = length;
				}
				if ( appearance > 1 ) {
					out << appearance << " people found: " << del->first << endl;
					_words.erase( del );
					continue;
				}
				if ( length == longestLength )
					longest.push_back( del );
			} else
				_words.erase( del );
		} else {
			if ( appearance > 1 ) {
				out << appearance << " people found: " << del->first << endl;
				_words.erase( del );
				continue;
			}
			if ( ( scores[ length - 1 ] > 0 ) && word_is_good( del->first, length ) ) {
				if ( length > longestLength ) {
					longest.clear();
					longestLength = length;
				}
				if ( length == longestLength )
					longest.push_back( del );
			} else
				_words.erase( del );
		}
	}
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ++ it ) {
		int length = static_cast<int>( it->first.get_length() );
		if ( ( _scoring != SCORING::LONGEST_WORDS ) || ( length == longestLength ) ) {
			OClientInfo* clInfo = *it->second->begin();
			OPlayerInfo& info = *get_player_info( clInfo );
			*(clInfo->_socket) << *this << PROTOCOL::SCORED << PROTOCOL::SEP << it->first << "[" << scores[ length - 1 ] << "]" << endl;
			info._last += scores[ length - 1 ];
			out << clInfo->_login << " scored: " << scores[ length - 1 ] << " for word: " << it->first << "." << endl;
		}
	}
	for ( players_t::iterator it = _players.begin(); it != _players.end(); ++ it ) {
		it->second._score += it->second._last;
		broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
				<< it->first->_login << PROTOCOL::SEPP << it->second._score
				<< PROTOCOL::SEPP << it->second._last << endl << _out );
		it->second._last = 0;
	}
	for ( longest_t::iterator it = longest.begin(); it != longest.end(); ++ it )
		broadcast( _out << PROTOCOL::LONGEST << PROTOCOL::SEP
				<< (*it)->first << " [" << (*(*it)->second->begin())->_login << "]" << endl << _out );
	_words.clear();
	if ( _round < _maxRounds )
		broadcast( _out << PROTOCOL::END_ROUND << endl << _out );
	return;
	M_EPILOG
}

bool HBoggle::is_good( int f, char const* ptr, int length ) {
	bool good = false;
	static int const DIRECTIONS = 8;
	int versor[ DIRECTIONS ][ 2 ] = {
			{ -1, -1 }, /* top left */
			{ 0, -1 }, /* top */
			{ 1, -1 }, /* top right */
			{ -1, 0 }, /* left */
			{ 1, 0 }, /* rigth */
			{ -1, 1 }, /* bottom down */
			{ 0, 1 }, /* bottom */
			{ 1, 1 } /* bottom right */
	};
	char saved = _game[ f ][ 1 ];
	if ( ! _game[ f ][ 1 ] && ( *ptr == _game[ f ][ 0 ] ) ) {
		if ( length == 1 )
			good = true;
		else {
			_game[ f ][ 1 ] = 1;
			int x = f % 4;
			int y = f / 4;
			for ( int i = 0; ! good && ( i < DIRECTIONS ); ++ i ) {
				int nx = x + versor[ i ][ 0 ];
				int ny = y + versor[ i ][ 1 ];
				if ( ( nx >= 0 ) && ( nx < 4 ) && ( ny >= 0 ) && ( ny < 4 ) )
					good = is_good( ny * 4 + nx, ptr + 1, length - 1 );
			}
		}
	}
	_game[ f ][ 1 ] = saved;
	return ( good );
}

bool HBoggle::word_is_good( HString const& word_, int length_ ) {
	M_PROLOG
	bool good = false;
	char const* ptr = word_.raw();
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f )
		_game[ f ][ 1 ] = 0;
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f ) {
		if ( is_good( f, ptr, length_ ) ) {
			good = true;
			break;
		}
	}
	if ( good )
		good = ! HSpellCheckerService::get_instance().spell_check( word_ );
	return ( good );
	M_EPILOG
}

}

namespace logic_factory {

class HBoggleCreator : public HLogicCreatorInterface {
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual void do_initialize_globals( void );
	virtual HString do_get_info( void ) const;
} boggleCreator;

HLogic::ptr_t HBoggleCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ ) {
	M_PROLOG
	out << "creating logic: " << argv_ << endl;
	HTokenizer t( argv_, "," );
	HString name( t[ 0 ] );
	HString scoringStr( t[ 1 ] );
	int players( 0 );
	int roundTime( 0 );
	int maxRounds( 0 );
	int interRoundDelay( 0 );
	try {
		players = lexical_cast<int>( t[ 2 ] );
		roundTime = lexical_cast<int>( t[ 3 ] );
		maxRounds = lexical_cast<int>( t[ 4 ] );
		interRoundDelay = lexical_cast<int>( t[ 5 ] );
	} catch ( HLexicalCastException const& ) {
		throw HLogicException( boggle::BOGGLE_MSG[boggle::BOGGLE_MSG_MALFORMED] );
	}
	boggle::HBoggle::SCORING::scoring_t scoring( boggle::HBoggle::SCORING::ORIGINAL );
	if ( scoringStr == "original" )
		scoring = boggle::HBoggle::SCORING::ORIGINAL;
	else if ( scoringStr == "fibonacci" )
		scoring = boggle::HBoggle::SCORING::FIBONACCI;
	else if ( scoringStr == "fibonacci4" )
		scoring = boggle::HBoggle::SCORING::FIBONACCI_4;
	else if ( scoringStr == "geometric" )
		scoring = boggle::HBoggle::SCORING::GEOMETRIC;
	else if ( scoringStr == "geometric4" )
		scoring = boggle::HBoggle::SCORING::GEOMETRIC_4;
	else if ( scoringStr == "longestwords" )
		scoring = boggle::HBoggle::SCORING::LONGEST_WORDS;
	return ( make_pointer<boggle::HBoggle>( server_, id_, name,
				scoring,
				players,
				roundTime,
				maxRounds,
				interRoundDelay ) );
	M_EPILOG
}

HString HBoggleCreator::do_get_info( void ) const {
	HString setupMsg;
	setupMsg.format( "%s:%s,%d,%d,%d,%d", boggle::HBoggle::PROTOCOL::NAME,
			setup._scoringSystem.raw(), setup._boggleStarupPlayers,
			setup._roundTime, setup._boggleRounds, setup._interRoundDelay );
	out << setupMsg << endl;
	return ( setupMsg );
}

void HBoggleCreator::do_initialize_globals( void ) {
	M_PROLOG
	static char const* const MAGIC_WORD = "mama";
	HSpellCheckerService::get_instance().spell_check( MAGIC_WORD );
	return;
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.register_logic_creator( HTokenizer( boggleCreator.get_info(), ":" )[0], &boggleCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

