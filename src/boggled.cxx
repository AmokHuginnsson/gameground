/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hstaticarray.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hformat.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/hscheduledasynccaller.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "boggled.hxx"
#include "spellchecker.hxx"
#include "bogglesolver.hxx"

#include "setup.hxx"
#include "client.hxx"
#include "logicfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace gameground {

namespace boggle_data {

static int const MAXIMUM_WORD_LENGTH = 16;

typedef yaal::hcore::HStaticArray<code_point_t, 6> dice_t;

static dice_t const _dicesPl_[ 16 ] = {
		{ U'a'_ycp, U'a'_ycp, U'd'_ycp, U'e'_ycp, U'ł'_ycp, U'y'_ycp },
		{ U'a'_ycp, U'a'_ycp, U'i'_ycp, U'k'_ycp, U'm'_ycp, U'y'_ycp },
		{ U'a'_ycp, U'c'_ycp, U'i'_ycp, U'n'_ycp, U'n'_ycp, U'p'_ycp },
		{ U'a'_ycp, U'e'_ycp, U'i'_ycp, U'o'_ycp, U'u'_ycp, U'y'_ycp },
		{ U'a'_ycp, U'e'_ycp, U'o'_ycp, U'y'_ycp, U'z'_ycp, U'z'_ycp },
		{ U'a'_ycp, U'g'_ycp, U'l'_ycp, U'o'_ycp, U'r'_ycp, U's'_ycp },
		{ U'ą'_ycp, U'c'_ycp, U'g'_ycp, U'm'_ycp, U'ń'_ycp, U't'_ycp },
		{ U'b'_ycp, U'e'_ycp, U'e'_ycp, U'p'_ycp, U'z'_ycp, U'ź'_ycp },
		{ U'b'_ycp, U'i'_ycp, U'i'_ycp, U'n'_ycp, U'r'_ycp, U'w'_ycp },
		{ U'c'_ycp, U'o'_ycp, U's'_ycp, U't'_ycp, U'w'_ycp, U'ż'_ycp },
		{ U'ć'_ycp, U'j'_ycp, U'm'_ycp, U't'_ycp, U'u'_ycp, U'z'_ycp },
		{ U'd'_ycp, U'e'_ycp, U'k'_ycp, U'n'_ycp, U'o'_ycp, U'r'_ycp },
		{ U'd'_ycp, U'h'_ycp, U'ó'_ycp, U'n'_ycp, U'w'_ycp, U'z'_ycp },
		{ U'e'_ycp, U'i'_ycp, U'l'_ycp, U's'_ycp, U'ś'_ycp, U'w'_ycp },
		{ U'ę'_ycp, U'f'_ycp, U'h'_ycp, U'j'_ycp, U'l'_ycp, U's'_ycp },
		{ U'i'_ycp, U'k'_ycp, U'ł'_ycp, U'o'_ycp, U'p'_ycp, U'r'_ycp }
};

static dice_t const _dicesEnNew_[ 16 ] = {
		{ U'a'_ycp, U'a'_ycp, U'e'_ycp, U'e'_ycp, U'g'_ycp, U'n'_ycp },
		{ U'e'_ycp, U'l'_ycp, U'r'_ycp, U't'_ycp, U't'_ycp, U'y'_ycp },
		{ U'a'_ycp, U'o'_ycp, U'o'_ycp, U't'_ycp, U't'_ycp, U'w'_ycp },
		{ U'a'_ycp, U'b'_ycp, U'b'_ycp, U'j'_ycp, U'o'_ycp, U'o'_ycp },
		{ U'e'_ycp, U'h'_ycp, U'r'_ycp, U't'_ycp, U'v'_ycp, U'w'_ycp },
		{ U'c'_ycp, U'i'_ycp, U'm'_ycp, U'o'_ycp, U't'_ycp, U'u'_ycp },
		{ U'd'_ycp, U'i'_ycp, U's'_ycp, U't'_ycp, U't'_ycp, U'y'_ycp },
		{ U'e'_ycp, U'i'_ycp, U'o'_ycp, U's'_ycp, U's'_ycp, U't'_ycp },
		{ U'd'_ycp, U'e'_ycp, U'l'_ycp, U'r'_ycp, U'v'_ycp, U'y'_ycp },
		{ U'a'_ycp, U'c'_ycp, U'h'_ycp, U'o'_ycp, U'p'_ycp, U's'_ycp },
		{ U'h'_ycp, U'i'_ycp, U'm'_ycp, U'n'_ycp, U'q'_ycp, U'u'_ycp },
		{ U'e'_ycp, U'e'_ycp, U'i'_ycp, U'n'_ycp, U's'_ycp, U'u'_ycp },
		{ U'e'_ycp, U'e'_ycp, U'g'_ycp, U'h'_ycp, U'n'_ycp, U'w'_ycp },
		{ U'a'_ycp, U'f'_ycp, U'f'_ycp, U'k'_ycp, U'p'_ycp, U's'_ycp },
		{ U'h'_ycp, U'l'_ycp, U'n'_ycp, U'n'_ycp, U'r'_ycp, U'z'_ycp },
		{ U'd'_ycp, U'e'_ycp, U'i'_ycp, U'l'_ycp, U'r'_ycp, U'x'_ycp }
};

static dice_t const _dicesEnOld_[ 16 ] = {
		{ U'a'_ycp, U'a'_ycp, U'c'_ycp, U'i'_ycp, U'o'_ycp, U't'_ycp },
		{ U'a'_ycp, U'h'_ycp, U'm'_ycp, U'o'_ycp, U'r'_ycp, U's'_ycp },
		{ U'e'_ycp, U'g'_ycp, U'k'_ycp, U'l'_ycp, U'u'_ycp, U'y'_ycp },
		{ U'a'_ycp, U'b'_ycp, U'i'_ycp, U'l'_ycp, U't'_ycp, U'y'_ycp },
		{ U'a'_ycp, U'c'_ycp, U'd'_ycp, U'e'_ycp, U'm'_ycp, U'p'_ycp },
		{ U'e'_ycp, U'g'_ycp, U'i'_ycp, U'n'_ycp, U't'_ycp, U'v'_ycp },
		{ U'g'_ycp, U'i'_ycp, U'l'_ycp, U'r'_ycp, U'u'_ycp, U'w'_ycp },
		{ U'e'_ycp, U'l'_ycp, U'p'_ycp, U's'_ycp, U't'_ycp, U'u'_ycp },
		{ U'd'_ycp, U'e'_ycp, U'n'_ycp, U'o'_ycp, U's'_ycp, U'w'_ycp },
		{ U'a'_ycp, U'c'_ycp, U'e'_ycp, U'l'_ycp, U'r'_ycp, U's'_ycp },
		{ U'a'_ycp, U'b'_ycp, U'j'_ycp, U'm'_ycp, U'o'_ycp, U'q'_ycp },
		{ U'e'_ycp, U'e'_ycp, U'f'_ycp, U'h'_ycp, U'i'_ycp, U'y'_ycp },
		{ U'e'_ycp, U'h'_ycp, U'i'_ycp, U'n'_ycp, U'p'_ycp, U's'_ycp },
		{ U'd'_ycp, U'k'_ycp, U'n'_ycp, U'o'_ycp, U't'_ycp, U'u'_ycp },
		{ U'a'_ycp, U'd'_ycp, U'e'_ycp, U'n'_ycp, U'v'_ycp, U'z'_ycp },
		{ U'b'_ycp, U'i'_ycp, U'f'_ycp, U'o'_ycp, U'r'_ycp, U'x'_ycp }
};

static dice_t const * const _dices_[] = {
	_dicesEnNew_,
	_dicesPl_
};

struct BOGGLE {
	static code_point_t const	UNINITIALIZED_SLOT;
	static int const SIDES = 6;
	static int const DICE_COUNT = 16;
};

code_point_t const BOGGLE::UNINITIALIZED_SLOT = code_point_t( static_cast<u32_t>( -1 ) );

typedef HArray<HSpellChecker> spell_checkers_t;
spell_checkers_t _spellCheckers_;
typedef HArray<HBoggleSolver> boggle_solvers_t;
boggle_solvers_t _boggleSolvers_;

}

namespace boggle {

char const* const HBoggle::PROTOCOL::NAME = "bgl";
char const* const HBoggle::PROTOCOL::PLAY = "play";
char const* const HBoggle::PROTOCOL::DECK = "deck";
char const* const HBoggle::PROTOCOL::SETUP = "setup";
char const* const HBoggle::PROTOCOL::END_ROUND = "end_round";
char const* const HBoggle::PROTOCOL::INIT = "init";
char const* const HBoggle::PROTOCOL::ROUND = "round";
char const* const HBoggle::PROTOCOL::PAUSE = "pause";
char const* const HBoggle::PROTOCOL::OVER = "over";
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

namespace {

char const* state_to_str( HBoggle::STATE state_ ) {
	char const* str( "" );
	switch ( state_ ) {
		case ( HBoggle::STATE::INIT ):  str = HBoggle::PROTOCOL::INIT;  break;
		case ( HBoggle::STATE::ROUND ): str = HBoggle::PROTOCOL::ROUND; break;
		case ( HBoggle::STATE::PAUSE ): str = HBoggle::PROTOCOL::PAUSE; break;
		case ( HBoggle::STATE::OVER ):  str = HBoggle::PROTOCOL::OVER;  break;
	}
	return str;
}

}

HBoggle::HBoggle(
	HServer* server_,
	id_t const& id_,
	HString const& comment_,
	LANGUAGE language_,
	SCORING scoring_,
	int players_,
	int roundTime_,
	int maxRounds_,
	int interRoundDelay_
) : HLogic( server_, id_, comment_ )
	, _state( STATE::INIT )
	, _language( language_ )
	, _scoring( scoring_ )
	, _startupPlayers( players_ )
	, _roundTime( roundTime_ )
	, _maxRounds( maxRounds_ )
	, _interRoundDelay( interRoundDelay_ )
	, _round( 0 )
	, _players()
	, _words()
	, _clock()
	, _varTmpBuffer() {
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
	random::HRandomNumberGenerator rng;
	for ( int i( 0 ); i < boggle_data::BOGGLE::DICE_COUNT; ++ i ) {
		_game[ i ][ 0 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
	}
	for ( int i( 0 ); i < boggle_data::BOGGLE::DICE_COUNT; ++ i ) {
		int k( 0 );
		int slot( static_cast<int>( rng() % static_cast<int unsigned>( boggle_data::BOGGLE::DICE_COUNT - i ) ) );
		for ( int j( 0 ); j < slot; ++ j, ++ k ) {
			while ( _game[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT ) {
				++ k;
			}
		}
		while ( _game[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT ) {
			++ k;
		}
		_game[ k ][ 0 ] = boggle_data::_dices_[ static_cast<int>( _language ) ][ i ][ static_cast<int>( rng() % boggle_data::BOGGLE::SIDES ) ];
	}
	return;
	M_EPILOG
}

HString const& HBoggle::serialize_deck( void ) {
	M_PROLOG
	_varTmpBuffer.clear();
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		_varTmpBuffer += _game[ i ][ 0 ];
	OUT << "new deck: " << endl;
	cout << _varTmpBuffer.left( 4 ) << endl;
	cout << _varTmpBuffer.mid( 4, 4 ) << endl;
	cout << _varTmpBuffer.mid( 8, 4 ) << endl;
	cout << _varTmpBuffer.mid( 12, 4 ) << endl;
	return ( _varTmpBuffer );
	M_EPILOG
}

void HBoggle::handler_play( HClient* client_, HString const& word_ ) {
	M_PROLOG
	HLock l( _mutex );
	static int const BOGGLE_REAL_MINIMUM_WORD_LENGTH = 3;
	int length = static_cast<int>( word_.get_length() );
	if ( ( _state == STATE::ROUND )
			&& ( length >= BOGGLE_REAL_MINIMUM_WORD_LENGTH )
			&& ( length <= boggle_data::MAXIMUM_WORD_LENGTH ) ) {
		words_t::iterator it = _words.find( word_ );
		if ( it == _words.end() )
			it = _words.insert( hcore::make_pair( word_, make_pointer<client_set_t>() ) ).first;
		it->second->insert( client_ );
		OUT << "word: " << word_ << "< inserted, proof: " << _words.size() << "," << it->second->size() << endl;
	}
	return;
	M_EPILOG
}

HBoggle::OPlayerInfo* HBoggle::get_player_info( HClient* client_ ) {
	M_PROLOG
	players_t::iterator it = _players.find( client_ );
	M_ASSERT( it != _players.end() );
	return ( &it->second );
	M_EPILOG
}

bool HBoggle::do_accept( HClient* client_ ) {
	OUT << "new candidate " << client_->login() << endl;
	return ( false );
}

inline char const* lang_id_to_name( LANGUAGE langId_ ) {
	char const* langStr( nullptr );
	switch ( langId_ ) {
		case ( LANGUAGE::ENGLISH ) : langStr = "English"; break;
		case ( LANGUAGE::POLISH ) : langStr = "Polish"; break;
	}
	return langStr;
}

void HBoggle::do_post_accept( HClient* client_ ) {
	M_PROLOG
	HLock l( _mutex );
	OUT << "conditions: _players.size() = " << _players.size() << ", _startupPlayers = " << _startupPlayers << endl;
	OPlayerInfo info;
	_players[ client_ ] = info;
	/*
	 * Send proto info about new contestant to all players.
	 */
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< client_->login() << PROTOCOL::SEPP << 0 << PROTOCOL::SEPP << 0 << endl << _out );
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
	client_->send( _out << *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "Welcome, this match settings are:" << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   language - " << lang_id_to_name( _language ) << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   scoring system - " << scoringStr << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round time - " << _roundTime << " seconds" << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   number of rounds - " << _maxRounds << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round interval - " << _interRoundDelay << endl
		<< *this << PROTOCOL::MSG << PROTOCOL::SEP
		<< "This match requires " << _startupPlayers << " players to start the game." << endl << _out );
	int elapsed( static_cast<int>( _clock.get_time_elapsed( time::UNIT::SECOND ) ) );
	int timeLeft( _state == STATE::ROUND ? _roundTime - elapsed : ( _state == STATE::PAUSE ? _interRoundDelay - elapsed : 0 ) );
	client_->send( _out << *this
		<< PROTOCOL::SETUP << PROTOCOL::SEP << state_to_str( _state )
		<< PROTOCOL::SEPP << timeLeft << endl << _out );
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
	if ( ! _round && ( _players.size() >= _startupPlayers ) ) {
		schedule_end_round();
		broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
				<< "The match has begun, good luck!" << endl << _out );
	} else if ( _round > 0 ) {
		client_->send( _out << *this << PROTOCOL::DECK << PROTOCOL::SEP << serialize_deck() << endl << _out );
		if ( _round >= _maxRounds ) {
			client_->send( _out << *this << PROTOCOL::MSG << PROTOCOL::SEP << "The game is over." << endl << _out );
		}
	}
	return;
	M_EPILOG
}

void HBoggle::do_kick( HClient* client_ ) {
	M_PROLOG
	HLock l( _mutex );
	_players.erase( client_ );
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ++ it ) {
		if ( it->second->size() > 1 ) {
			continue;
		}
		if ( *(it->second->begin()) == client_ ) {
			it = _words.erase( it );
		}
	}
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << client_->login() << " left this match." << endl << _out );
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
	_state = STATE::ROUND;
	_clock.reset();
	broadcast( _out << PROTOCOL::DECK << PROTOCOL::SEP << serialize_deck() << endl << _out );
	return;
	M_EPILOG
}

void HBoggle::on_begin_round( void ) {
	M_PROLOG
	HLock l( _mutex );
	OUT << "<<begin>>" << endl;
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
	OUT << "<<end>>" << endl;
	_clock.reset();
	if ( _round < _maxRounds ) {
		_state = STATE::PAUSE;
		HScheduledAsyncCaller::get_instance().call_in( duration( _interRoundDelay, time::UNIT::SECOND ), call( &HBoggle::on_begin_round, this ) );
		_out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "This round has ended, next round in " << _interRoundDelay << " seconds!" << endl;
	} else {
		_state = STATE::OVER;
		_out << PROTOCOL::MSG << PROTOCOL::SEP << "Game Over!" << endl;
	}
	broadcast( _out << _out );
	if ( !boggle_data::_boggleSolvers_[static_cast<int>( _language )] ) {
		boggle_data::_boggleSolvers_[static_cast<int>( _language )].solve( _varTmpBuffer );
	}
	int* scores = RULES[ static_cast<int>( _scoring ) ];
	typedef HList<words_t::iterator> longest_t;
	longest_t longest;
	int longestLength = 0;
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ) {
		words_t::iterator del( it );
		++ it;
		int appearance( static_cast<int>( del->second->size() ) );
		int length( static_cast<int>( del->first.get_length() ) );
		if ( _scoring == SCORING::LONGEST_WORDS ) {
			if ( ( scores[ length - 1 ] > 0 ) && word_is_good( del->first ) ) {
				if ( length > longestLength ) {
					longest.clear();
					longestLength = length;
				}
				if ( appearance > 1 ) {
					OUT << appearance << " people found: " << del->first << endl;
					_words.erase( del );
					continue;
				}
				if ( length == longestLength ) {
					longest.push_back( del );
				}
			} else {
				_words.erase( del );
			}
		} else {
			if ( appearance > 1 ) {
				OUT << appearance << " people found: " << del->first << endl;
				_words.erase( del );
				continue;
			}
			if ( ( scores[ length - 1 ] > 0 ) && word_is_good( del->first ) ) {
				if ( length > longestLength ) {
					longest.clear();
					longestLength = length;
				}
				if ( length == longestLength )
					longest.push_back( del );
			} else {
				_words.erase( del );
			}
		}
	}
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ++ it ) {
		int length = static_cast<int>( it->first.get_length() );
		if ( ( _scoring != SCORING::LONGEST_WORDS ) || ( length == longestLength ) ) {
			HClient* clInfo = *it->second->begin();
			OPlayerInfo& info = *get_player_info( clInfo );
			clInfo->send( _out << *this << PROTOCOL::SCORED << PROTOCOL::SEP << it->first << "[" << scores[ length - 1 ] << "]" << endl << _out );
			info._last += scores[ length - 1 ];
			OUT << clInfo->login() << " scored: " << scores[ length - 1 ] << " for word: " << it->first << "." << endl;
		}
	}
	for ( players_t::iterator it = _players.begin(); it != _players.end(); ++ it ) {
		it->second._score += it->second._last;
		broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
				<< it->first->login() << PROTOCOL::SEPP << it->second._score
				<< PROTOCOL::SEPP << it->second._last << endl << _out );
		it->second._last = 0;
	}
	for ( longest_t::iterator it = longest.begin(); it != longest.end(); ++ it ) {
		broadcast( _out << PROTOCOL::LONGEST << PROTOCOL::SEP
				<< (*it)->first << " [" << (*(*it)->second->begin())->login() << "]" << endl << _out );
	}
	_words.clear();
	if ( _round < _maxRounds ) {
		broadcast( _out << PROTOCOL::END_ROUND << endl << _out );
	}
	return;
	M_EPILOG
}

bool HBoggle::is_good( int f, yaal::hcore::HString::const_iterator it_, yaal::hcore::HString::const_iterator end_ ) {
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
	code_point_t saved = _game[ f ][ 1 ];
	if ( ! _game[ f ][ 1 ] && ( *it_ == _game[ f ][ 0 ] ) ) {
		++ it_;
		if ( it_ == end_ ) {
			good = true;
		} else {
			_game[ f ][ 1 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
			int x = f % 4;
			int y = f / 4;
			for ( int i = 0; ! good && ( i < DIRECTIONS ); ++ i ) {
				int nx = x + versor[ i ][ 0 ];
				int ny = y + versor[ i ][ 1 ];
				if ( ( nx >= 0 ) && ( nx < 4 ) && ( ny >= 0 ) && ( ny < 4 ) ) {
					good = is_good( ny * 4 + nx, it_, end_ );
				}
			}
		}
	}
	_game[ f ][ 1 ] = saved;
	return good;
}

bool HBoggle::word_is_good( HString const& word_ ) {
	M_PROLOG
	bool good = false;
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f ) {
		_game[ f ][ 1 ] = 0_ycp;
	}
	HString w( word_ );
	w.replace( "qu", "q" );
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f ) {
		if ( is_good( f, w.cbegin(), w.cend() ) ) {
			good = true;
			break;
		}
	}
	if ( good ) {
		good = ! boggle_data::_spellCheckers_[static_cast<int>( _language )].spell_check( word_ );
	}
	return good;
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
	OUT << "creating logic: " << argv_ << endl;
	HTokenizer t( argv_, "," );
	HString name( t[ 0 ] );
	HString languageStr( t[ 1 ] );
	HString scoringStr( t[ 2 ] );
	int players( 0 );
	int roundTime( 0 );
	int maxRounds( 0 );
	int interRoundDelay( 0 );
	try {
		players = lexical_cast<int>( t[ 3 ] );
		roundTime = lexical_cast<int>( t[ 4 ] );
		maxRounds = lexical_cast<int>( t[ 5 ] );
		interRoundDelay = lexical_cast<int>( t[ 6 ] );
	} catch ( HLexicalCastException const& ) {
		throw HLogicException( boggle::BOGGLE_MSG[boggle::BOGGLE_MSG_MALFORMED] );
	}
	LANGUAGE language( LANGUAGE::ENGLISH );
	if ( languageStr == "en" ) {
		language = LANGUAGE::ENGLISH;
	} else if ( languageStr == "pl" ) {
		language = LANGUAGE::POLISH;
	}
	boggle::HBoggle::SCORING scoring( boggle::HBoggle::SCORING::ORIGINAL );
	if ( scoringStr == "original" ) {
		scoring = boggle::HBoggle::SCORING::ORIGINAL;
	} else if ( scoringStr == "fibonacci" ) {
		scoring = boggle::HBoggle::SCORING::FIBONACCI;
	} else if ( scoringStr == "fibonacci4" ) {
		scoring = boggle::HBoggle::SCORING::FIBONACCI_4;
	} else if ( scoringStr == "geometric" ) {
		scoring = boggle::HBoggle::SCORING::GEOMETRIC;
	} else if ( scoringStr == "geometric4" ) {
		scoring = boggle::HBoggle::SCORING::GEOMETRIC_4;
	} else if ( scoringStr == "longestwords" ) {
		scoring = boggle::HBoggle::SCORING::LONGEST_WORDS;
	}
	return (
		make_pointer<boggle::HBoggle>(
			server_,
			id_,
			name,
			language,
			scoring,
			players,
			roundTime,
			maxRounds,
			interRoundDelay
		)
	);
	M_EPILOG
}

HString HBoggleCreator::do_get_info( void ) const {
	HString setupMsg;
	setupMsg = format(
		"%s:%s,%s,%d,%d,%d,%d",
		boggle::HBoggle::PROTOCOL::NAME,
		setup._boggleLanguage,
		setup._scoringSystem,
		setup._boggleStarupPlayers,
		setup._roundTime,
		setup._boggleRounds,
		setup._interRoundDelay
	);
	OUT << setupMsg << endl;
	return setupMsg;
}

void HBoggleCreator::do_initialize_globals( void ) {
	M_PROLOG
	boggle_data::_spellCheckers_.emplace_back( LANGUAGE::ENGLISH );
	boggle_data::_spellCheckers_.emplace_back( LANGUAGE::POLISH );
	boggle_data::_boggleSolvers_.emplace_back( "dict_en.txt" );
	boggle_data::_boggleSolvers_.emplace_back( "dict_pl.txt" );
	/* makejail.py helper */
	boggle_data::_spellCheckers_[ static_cast<int>( LANGUAGE::ENGLISH ) ].spell_check( "mom" );
	boggle_data::_spellCheckers_[ static_cast<int>( LANGUAGE::POLISH ) ].spell_check( "mama" );
	return;
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.register_logic_creator( HTokenizer( boggleCreator.get_info(), ":" )[0], &boggleCreator );
	return failed;
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

