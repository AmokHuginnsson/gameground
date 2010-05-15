/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	boggled.cxx - this file is integral part of `gameground' project.

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

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "boggled.hxx"

#include "setup.hxx"
#include "clientinfo.hxx"
#include "logicfactory.hxx"
#include "spellchecker.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground
{

namespace boggle_data
{

static int const MAXIMUM_WORD_LENGTH = 16;

static char const _dices_[ 16 ][ 6 ] =
	{
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

struct BOGGLE
	{
	enum
		{
		UNINITIALIZED_SLOT = -1,
		SIDES = 6,
		DICE_COUNT = 16
		};
	};

}

namespace boggle
{

char const* const HBoggle::PROTOCOL::SEP = ":";
char const* const HBoggle::PROTOCOL::SEPP = ",";
char const* const HBoggle::PROTOCOL::NAME = "bgl";
char const* const HBoggle::PROTOCOL::PLAY = "play";
char const* const HBoggle::PROTOCOL::SAY = "say";
char const* const HBoggle::PROTOCOL::MSG = "msg";
char const* const HBoggle::PROTOCOL::PLAYER = "player";
char const* const HBoggle::PROTOCOL::DECK = "deck";
char const* const HBoggle::PROTOCOL::SETUP = "setup";
char const* const HBoggle::PROTOCOL::END_ROUND = "end_round";
char const* const HBoggle::PROTOCOL::ROUND = "round";
char const* const HBoggle::PROTOCOL::SCORED = "scored";
char const* const HBoggle::PROTOCOL::LONGEST = "longest";
HBoggle::SCORING::ORule HBoggle::RULES[] = { { 3, { 0, 0, 1, 1, 2, 3, 5, 11, 11, 11, 11, 11, 11, 11, 11, 11 } },
		{ 4, { 0, 0, 0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377 } },
		{ 5, { 0, 0, 0, 0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233 } },
		{ 5, { 0, 0, 0, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 } } };

HBoggle::HBoggle( HString const& name_, int players_, int roundTime_, int maxRounds_, int interRoundDelay_ )
	: HLogic( PROTOCOL::NAME, name_ ), _state( STATE::LOCKED ), _players( players_ ),
	_roundTime( roundTime_ ), _maxRounds( maxRounds_ ),
	_interRoundDelay( interRoundDelay_ ), _ruleSet( 0 ), _round( 0 ), _players(),
	_words(), _mutex()
	{
	M_PROLOG
	HRandomizer random;
	random.set( time ( NULL ) );
	_handlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HBoggle::handler_play );
	_handlers[ PROTOCOL::SAY ] = static_cast<handler_t>( &HBoggle::handler_message );
	return;
	M_EPILOG
	}

HBoggle::~HBoggle ( void )
	{
	M_PROLOG
	HAsyncCallerService::get_instance().flush( this );
	HScheduledAsyncCallerService::get_instance().flush( this );
	return;
	M_EPILOG
	}

void HBoggle::generate_game( void )
	{
	M_PROLOG
	HRandomizer rnd;
	randomizer_helper::init_randomizer_from_time( rnd );
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		_game[ i ][ 0 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		{
		int k = 0, slot = rnd.rnd( boggle_data::BOGGLE::DICE_COUNT - i );
		for ( int j = 0; j < slot; ++ j, ++ k )
			while ( _game[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
				++ k;
		while ( _game[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
			++ k;
		_game[ k ][ 0 ] = boggle_data::_dices_[ i ][ rnd.rnd( boggle_data::BOGGLE::SIDES ) ];
		}
	return;
	M_EPILOG
	}

HString HBoggle::make_deck( void )
	{
	M_PROLOG
	HString deck;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		deck += _game[ i ][ 0 ];
	out << "new deck: " << endl;
	cout << deck.left( 4 ) << endl;
	cout << deck.mid( 4, 4 ) << endl;
	cout << deck.mid( 8, 4 ) << endl;
	cout << deck.mid( 12, 4 ) << endl;
	return ( deck );
	M_EPILOG
	}

void HBoggle::handler_message( OClientInfo* clientInfo_, HString const& message_ )
	{
	M_PROLOG
	HLock l( _mutex );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< clientInfo_->_login << ": " << message_ << endl << _out );
	return;
	M_EPILOG
	}

void HBoggle::handler_play( OClientInfo* clientInfo_, HString const& word_ )
	{
	M_PROLOG
	HLock l( _mutex );
	int length = static_cast<int>( word_.get_length() );
	if ( ( _state == STATE::ACCEPTING )
			&& ( length >= RULES[ _ruleSet ]._minLength )
			&& ( length <= boggle_data::MAXIMUM_WORD_LENGTH ) )
		{
		words_t::iterator it = _words.find( word_ );
		if ( it == _words.end() )
			it = _words.insert( hcore::make_pair( word_, client_set_ptr_t( new client_set_t() ) ) ).first;
		it->second->insert( clientInfo_ );
		out << "word: " << word_ << "< inserted, proof: " << _words.size() << "," << it->second->size() << endl;
		}
	return;
	M_EPILOG
	}

HBoggle::OPlayerInfo* HBoggle::get_player_info( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	players_t::iterator it = _players.find( clientInfo_ );
	M_ASSERT( it != _players.end() );
	return ( &it->second );
	M_EPILOG
	}

bool HBoggle::do_accept( OClientInfo* clientInfo_ )
	{
	out << "new candidate " << clientInfo_->_login << endl;
	return ( false );
	}

void HBoggle::do_post_accept( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	HLock l( _mutex );
	out << "conditions: _players.size() = " << _players.size() << ", _players = " << _players << endl;
	OPlayerInfo info;
	_players[ clientInfo_ ] = info;
	/*
	 * Send proto info about new contestant to all players.
	 */
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< clientInfo_->_login << PROTOCOL::SEPP << 0 << PROTOCOL::SEPP << 0 << endl << _out );
	*clientInfo_->_socket << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "Welcome, this match settings are:" << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round time - " << _roundTime << " seconds" << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   number of rounds - " << _maxRounds << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round interval - " << _interRoundDelay << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "This match requires " << _players << " players to start the game." << endl;
	*clientInfo_->_socket << PROTOCOL::NAME << PROTOCOL::SEP
		<< PROTOCOL::SETUP << PROTOCOL::SEP << _roundTime
		<< PROTOCOL::SEPP << _interRoundDelay << endl;
	for ( players_t::iterator it = _players.begin(); it != _players.end(); ++ it )
		{
		if ( it->first != clientInfo_ )
			{
			*clientInfo_->_socket << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::PLAYER << PROTOCOL::SEP << it->first->_login
				<< PROTOCOL::SEPP << it->second._score << PROTOCOL::SEPP
				<< it->second._last << endl;
			*clientInfo_->_socket << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP << it->first->_login << " joined the mind contest." << endl;
			}
		}
	broadcast(
			_out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< clientInfo_->_login
			<< " joined the mind contest." << endl << _out );
	out << "player [" << clientInfo_->_login << "] accepted" << endl;
	if ( ! _round && ( _players.size() >= _players ) )
		{
		schedule_end_round();
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP
				<< "The match has begun, good luck!" << endl << _out );
		}
	else if ( _round > 0 )
		*clientInfo_->_socket << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::DECK << PROTOCOL::SEP << make_deck() << endl;
	return;
	M_EPILOG
	}

void HBoggle::do_kick( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	HLock l( _mutex );
	_players.erase( clientInfo_ );
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ++ it )
		{
		if ( it->second->size() > 1 )
			continue;
		if ( *(it->second->begin()) == clientInfo_ )
			it = _words.erase( it );
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << clientInfo_->_login << " left this match." << endl << _out );
	return;
	M_EPILOG
	}

yaal::hcore::HString HBoggle::get_info() const
	{
	HLock l( _mutex );
	return ( HString( "bgl," ) + _name + "," + _players.size() + "," + _players + "," + _roundTime + "," + _maxRounds + "," + _interRoundDelay );
	}

void HBoggle::schedule( EVENT::event_t event_ )
	{
	M_PROLOG
	HLock l( _mutex );
	if ( event_ == EVENT::BEGIN_ROUND )
		HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + _interRoundDelay, bound_call( &HBoggle::on_begin_round, this ) );
	else
		schedule_end_round();
	return;
	M_EPILOG
	}

void HBoggle::schedule_end_round( void )
	{
	M_PROLOG
	++ _round;
	HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + _roundTime, bound_call( &HBoggle::on_end_round, this ) );
	generate_game();
	_state = STATE::ACCEPTING;
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::DECK << PROTOCOL::SEP << make_deck() << endl << _out );
	return;
	M_EPILOG
	}

void HBoggle::on_begin_round( void )
	{
	M_PROLOG
	HLock l( _mutex );
	out << "<<begin>>" << endl;
	HAsyncCallerService::get_instance().register_call( 0, bound_call( &HBoggle::schedule, this, EVENT::END_ROUND ) );
	broadcast(
			_out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "New round started, you have got " << _roundTime
			<< " seconds, " << _maxRounds - _round
			<< " rounds left!" << endl << _out );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::ROUND << PROTOCOL::SEP << _round << endl << _out );
	return;
	M_EPILOG
	}

void HBoggle::on_end_round( void )
	{
	M_PROLOG
	HLock l( _mutex );
	out << "<<end>>" << endl;
	_state = STATE::LOCKED;
	if ( _round < _maxRounds )
		{
		HAsyncCallerService::get_instance().register_call( 0, bound_call( &HBoggle::schedule, this, EVENT::BEGIN_ROUND ) );
		_out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
			<< "This round has ended, next round in " << _interRoundDelay << " seconds!" << endl;
		}
	else
		_out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP << "Game Over!" << endl;
	broadcast( _out << _out );
	int* scores = RULES[ _ruleSet ]._score;
	typedef HList<words_t::iterator> longest_t;
	longest_t longest;
	int longestLength = 0;
	for ( words_t::iterator it = _words.begin(); it != _words.end(); ++ it )
		{
		int appearance = static_cast<int>( it->second->size() );
		if ( appearance > 1 )
			{
			out << appearance << " people found: " << it->first << endl;
			continue;
			}
		int length = static_cast<int>( it->first.get_length() );
		if ( word_is_good( it->first, length ) )
			{
			if ( length > longestLength )
				{
				longest.clear();
				longestLength = length;
				}
			if ( length == longestLength )
				longest.push_back( it );
			OClientInfo* clInfo = *it->second->begin();
			OPlayerInfo& info = *get_player_info( clInfo );
			*(clInfo->_socket) << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::SCORED << PROTOCOL::SEP << it->first << "[" << scores[ length - 1 ] << "]" << endl;
			info._last += scores[ length - 1 ];
			out << clInfo->_login << " scored: " << scores[ length - 1 ] << " for word: " << it->first << "." << endl;
			}
		}
	for ( players_t::iterator it = _players.begin(); it != _players.end(); ++ it )
		{
		it->second._score += it->second._last;
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::PLAYER << PROTOCOL::SEP
				<< it->first->_login << PROTOCOL::SEPP << it->second._score
				<< PROTOCOL::SEPP << it->second._last << endl << _out );
		it->second._last = 0;
		}
	for ( longest_t::iterator it = longest.begin(); it != longest.end(); ++ it )
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::LONGEST << PROTOCOL::SEP
				<< (*it)->first << " [" << (*(*it)->second->begin())->_login << "]" << endl << _out );
	_words.clear(); 
	if ( _round < _maxRounds )
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::END_ROUND << endl << _out );
	return;
	M_EPILOG
	}

bool HBoggle::is_good( int f, char const* ptr, int length )
	{
	bool good = false;
	static int const DIRECTIONS = 8;
	int versor[ DIRECTIONS ][ 2 ] = 
		{
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
	if ( ! _game[ f ][ 1 ] && ( *ptr == _game[ f ][ 0 ] ) )
		{
		if ( length == 1 )
			good = true;
		else
			{
			_game[ f ][ 1 ] = 1;
			int x = f % 4;
			int y = f / 4;
			for ( int i = 0; ! good && ( i < DIRECTIONS ); ++ i )
				{
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

bool HBoggle::word_is_good( HString const& word_, int length_ )
	{
	M_PROLOG
	bool good = false;
	char const* ptr = word_.raw();
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f )
		_game[ f ][ 1 ] = 0;
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f )
		{
		if ( is_good( f, ptr, length_ ) )
			{
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

namespace logic_factory
{

class HBoggleCreator : public HLogicCreatorInterface
	{
protected:
	virtual HLogic::ptr_t do_new_instance( HString const& );
	virtual void do_initialize_globals( void );
	} boggleCreator;

HLogic::ptr_t HBoggleCreator::do_new_instance( HString const& argv_ )
	{
	M_PROLOG
	out << "creating logic: " << argv_ << endl;
	HTokenizer t( argv_, "," );
	HString name = t[ 0 ];
	HString players = t[ 1 ];
	HString roundTime = t[ 2 ];
	HString maxRounds = t[ 3 ];
	HString interRoundDelay = t[ 4 ];
	int players = lexical_cast<int>( players );
	int roundTime = lexical_cast<int>( roundTime );
	int maxRounds = lexical_cast<int>( maxRounds );
	int interRoundDelay = lexical_cast<int>( interRoundDelay );
	return ( HLogic::ptr_t( new boggle::HBoggle( name,
					players,
					roundTime,
					maxRounds,
					interRoundDelay ) ) );
	M_EPILOG
	}

void HBoggleCreator::do_initialize_globals( void )
	{
	M_PROLOG
	static char const* const MAGIC_WORD = "mama";
	HSpellCheckerService::get_instance().spell_check( MAGIC_WORD );
	return;
	M_EPILOG
	}

namespace
{

bool registrar( void )
	{
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	HString setup;
	setup.format( "%s:%d,%d,%d,%d", "bgl", setup._players, setup._roundTime, setup._maxRounds, setup._interRoundDelay );
	factory.register_logic_creator( setup, &boggleCreator );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

}

