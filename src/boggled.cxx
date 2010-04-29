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

static char const n_ppcDices[ 16 ][ 6 ] =
	{
		{ 'a', 'a', 'd', 'e', '�', 'y' },
		{ 'a', 'a', 'i', 'k', 'm', 'y' },
		{ 'a', 'c', 'i', 'n', 'n', 'p' },
		{ 'a', 'e', 'i', 'o', 'u', 'y' },
		{ 'a', 'e', 'o', 'y', 'z', 'z' },
		{ 'a', 'g', 'l', 'o', 'r', 's' },
		{ '�', 'c', 'g', 'm', '�', 't' },
		{ 'b', 'e', 'e', 'p', 'z', '�' },
		{ 'b', 'i', 'i', 'n', 'r', 'w' },
		{ 'c', 'o', 's', 't', 'w', '�' },
		{ '�', 'j', 'm', 't', 'u', 'z' },
		{ 'd', 'e', 'k', 'n', 'o', 'r' },
		{ 'd', 'h', '�', 'n', 'w', 'z' },
		{ 'e', 'i', 'l', 's', '�', 'w' },
		{ '�', 'f', 'h', 'j', 'l', 's' },
		{ 'i', 'k', '�', 'o', 'p', 'r' }
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

HBoggle::HBoggle( HString const& a_oName, int a_iPlayers, int a_iRoundTime, int a_iMaxRounds, int a_iInterRoundDelay )
	: HLogic( PROTOCOL::NAME, a_oName ), f_eState( STATE::LOCKED ), f_iPlayers( a_iPlayers ),
	f_iRoundTime( a_iRoundTime ), f_iMaxRounds( a_iMaxRounds ),
	f_iInterRoundDelay( a_iInterRoundDelay ), f_iRuleSet( 0 ), f_iRound( 0 ), f_oPlayers(),
	f_oWords(), f_oMutex()
	{
	M_PROLOG
	HRandomizer l_oRandom;
	l_oRandom.set( time ( NULL ) );
	f_oHandlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HBoggle::handler_play );
	f_oHandlers[ PROTOCOL::SAY ] = static_cast<handler_t>( &HBoggle::handler_message );
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
		f_ppcGame[ i ][ 0 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		{
		int k = 0, l_iSlot = rnd.rnd( boggle_data::BOGGLE::DICE_COUNT - i );
		for ( int j = 0; j < l_iSlot; ++ j, ++ k )
			while ( f_ppcGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
				++ k;
		while ( f_ppcGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
			++ k;
		f_ppcGame[ k ][ 0 ] = boggle_data::n_ppcDices[ i ][ rnd.rnd( boggle_data::BOGGLE::SIDES ) ];
		}
	return;
	M_EPILOG
	}

HString HBoggle::make_deck( void )
	{
	M_PROLOG
	HString l_oDeck;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		l_oDeck += f_ppcGame[ i ][ 0 ];
	out << "new deck: " << endl;
	cout << l_oDeck.left( 4 ) << endl;
	cout << l_oDeck.mid( 4, 4 ) << endl;
	cout << l_oDeck.mid( 8, 4 ) << endl;
	cout << l_oDeck.mid( 12, 4 ) << endl;
	return ( l_oDeck );
	M_EPILOG
	}

void HBoggle::handler_message( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< a_poClientInfo->f_oLogin << ": " << a_roMessage << endl << _out );
	return;
	M_EPILOG
	}

void HBoggle::handler_play( OClientInfo* a_poClientInfo, HString const& a_oWord )
	{
	M_PROLOG
	HLock l( f_oMutex );
	int l_iLength = static_cast<int>( a_oWord.get_length() );
	if ( ( f_eState == STATE::ACCEPTING )
			&& ( l_iLength >= RULES[ f_iRuleSet ]._minLength )
			&& ( l_iLength <= boggle_data::MAXIMUM_WORD_LENGTH ) )
		{
		words_t::iterator it = f_oWords.find( a_oWord );
		if ( it == f_oWords.end() )
			it = f_oWords.insert( hcore::make_pair( a_oWord, client_set_ptr_t( new client_set_t() ) ) ).first;
		it->second->insert( a_poClientInfo );
		out << "word: " << a_oWord << "< inserted, proof: " << f_oWords.size() << "," << it->second->size() << endl;
		}
	return;
	M_EPILOG
	}

HBoggle::OPlayerInfo* HBoggle::get_player_info( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	players_t::iterator it = f_oPlayers.find( a_poClientInfo );
	M_ASSERT( it != f_oPlayers.end() );
	return ( &it->second );
	M_EPILOG
	}

bool HBoggle::do_accept( OClientInfo* a_poClientInfo )
	{
	out << "new candidate " << a_poClientInfo->f_oLogin << endl;
	return ( false );
	}

void HBoggle::do_post_accept( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	out << "conditions: f_oPlayers.size() = " << f_oPlayers.size() << ", f_iPlayers = " << f_iPlayers << endl;
	OPlayerInfo info;
	f_oPlayers[ a_poClientInfo ] = info;
	/*
	 * Send proto info about new contestant to all players.
	 */
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< a_poClientInfo->f_oLogin << PROTOCOL::SEPP << 0 << PROTOCOL::SEPP << 0 << endl << _out );
	*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "Welcome, this match settings are:" << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round time - " << f_iRoundTime << " seconds" << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   number of rounds - " << f_iMaxRounds << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "   round interval - " << f_iInterRoundDelay << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
		<< "This match requires " << f_iPlayers << " players to start the game." << endl;
	*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
		<< PROTOCOL::SETUP << PROTOCOL::SEP << f_iRoundTime
		<< PROTOCOL::SEPP << f_iInterRoundDelay << endl;
	for ( players_t::iterator it = f_oPlayers.begin(); it != f_oPlayers.end(); ++ it )
		{
		if ( it->first != a_poClientInfo )
			{
			*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::PLAYER << PROTOCOL::SEP << it->first->f_oLogin
				<< PROTOCOL::SEPP << it->second.f_iScore << PROTOCOL::SEPP
				<< it->second.f_iLast << endl;
			*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP << it->first->f_oLogin << " joined the mind contest." << endl;
			}
		}
	broadcast(
			_out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< a_poClientInfo->f_oLogin
			<< " joined the mind contest." << endl << _out );
	out << "player [" << a_poClientInfo->f_oLogin << "] accepted" << endl;
	if ( ! f_iRound && ( f_oPlayers.size() >= f_iPlayers ) )
		{
		schedule_end_round();
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP
				<< "The match has begun, good luck!" << endl << _out );
		}
	else if ( f_iRound > 0 )
		*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::DECK << PROTOCOL::SEP << make_deck() << endl;
	return;
	M_EPILOG
	}

void HBoggle::do_kick( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	f_oPlayers.erase( a_poClientInfo );
	for ( words_t::iterator it = f_oWords.begin(); it != f_oWords.end(); ++ it )
		{
		if ( it->second->size() > 1 )
			continue;
		if ( *(it->second->begin()) == a_poClientInfo )
			it = f_oWords.erase( it );
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << a_poClientInfo->f_oLogin << " left this match." << endl << _out );
	return;
	M_EPILOG
	}

yaal::hcore::HString HBoggle::get_info() const
	{
	HLock l( f_oMutex );
	return ( HString( "bgl," ) + f_oName + "," + f_oPlayers.size() + "," + f_iPlayers + "," + f_iRoundTime + "," + f_iMaxRounds + "," + f_iInterRoundDelay );
	}

void HBoggle::schedule( EVENT::event_t a_eEvent )
	{
	M_PROLOG
	HLock l( f_oMutex );
	if ( a_eEvent == EVENT::BEGIN_ROUND )
		HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + f_iInterRoundDelay, bound_call( &HBoggle::on_begin_round, this ) );
	else
		schedule_end_round();
	return;
	M_EPILOG
	}

void HBoggle::schedule_end_round( void )
	{
	M_PROLOG
	++ f_iRound;
	HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + f_iRoundTime, bound_call( &HBoggle::on_end_round, this ) );
	generate_game();
	f_eState = STATE::ACCEPTING;
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::DECK << PROTOCOL::SEP << make_deck() << endl << _out );
	return;
	M_EPILOG
	}

void HBoggle::on_begin_round( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	out << "<<begin>>" << endl;
	HAsyncCallerService::get_instance().register_call( 0, bound_call( &HBoggle::schedule, this, EVENT::END_ROUND ) );
	broadcast(
			_out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "New round started, you have got " << f_iRoundTime
			<< " seconds, " << f_iMaxRounds - f_iRound
			<< " rounds left!" << endl << _out );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::ROUND << PROTOCOL::SEP << f_iRound << endl << _out );
	return;
	M_EPILOG
	}

void HBoggle::on_end_round( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	out << "<<end>>" << endl;
	f_eState = STATE::LOCKED;
	if ( f_iRound < f_iMaxRounds )
		{
		HAsyncCallerService::get_instance().register_call( 0, bound_call( &HBoggle::schedule, this, EVENT::BEGIN_ROUND ) );
		_out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
			<< "This round has ended, next round in " << f_iInterRoundDelay << " seconds!" << endl;
		}
	else
		_out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP << "Game Over!" << endl;
	broadcast( _out << _out );
	int* scores = RULES[ f_iRuleSet ]._score;
	typedef HList<words_t::iterator> longest_t;
	longest_t longest;
	int l_iLongestLength = 0;
	for ( words_t::iterator it = f_oWords.begin(); it != f_oWords.end(); ++ it )
		{
		int appearance = static_cast<int>( it->second->size() );
		if ( appearance > 1 )
			{
			out << appearance << " people found: " << it->first << endl;
			continue;
			}
		int l_iLength = static_cast<int>( it->first.get_length() );
		if ( word_is_good( it->first, l_iLength ) )
			{
			if ( l_iLength > l_iLongestLength )
				{
				longest.clear();
				l_iLongestLength = l_iLength;
				}
			if ( l_iLength == l_iLongestLength )
				longest.push_back( it );
			OClientInfo* clInfo = *it->second->begin();
			OPlayerInfo& info = *get_player_info( clInfo );
			*(clInfo->f_oSocket) << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::SCORED << PROTOCOL::SEP << it->first << "[" << scores[ l_iLength - 1 ] << "]" << endl;
			info.f_iLast += scores[ l_iLength - 1 ];
			out << clInfo->f_oLogin << " scored: " << scores[ l_iLength - 1 ] << " for word: " << it->first << "." << endl;
			}
		}
	for ( players_t::iterator it = f_oPlayers.begin(); it != f_oPlayers.end(); ++ it )
		{
		it->second.f_iScore += it->second.f_iLast;
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::PLAYER << PROTOCOL::SEP
				<< it->first->f_oLogin << PROTOCOL::SEPP << it->second.f_iScore
				<< PROTOCOL::SEPP << it->second.f_iLast << endl << _out );
		it->second.f_iLast = 0;
		}
	for ( longest_t::iterator it = longest.begin(); it != longest.end(); ++ it )
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::LONGEST << PROTOCOL::SEP
				<< (*it)->first << " [" << (*(*it)->second->begin())->f_oLogin << "]" << endl << _out );
	f_oWords.clear(); 
	if ( f_iRound < f_iMaxRounds )
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
	char saved = f_ppcGame[ f ][ 1 ];
	if ( ! f_ppcGame[ f ][ 1 ] && ( *ptr == f_ppcGame[ f ][ 0 ] ) )
		{
		if ( length == 1 )
			good = true;
		else
			{
			f_ppcGame[ f ][ 1 ] = 1;
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
	f_ppcGame[ f ][ 1 ] = saved;
	return ( good );
	}

bool HBoggle::word_is_good( HString const& a_oWord, int a_iLength )
	{
	M_PROLOG
	bool good = false;
	char const* ptr = a_oWord.raw();
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f )
		f_ppcGame[ f ][ 1 ] = 0;
	for ( int f = 0; f < boggle_data::MAXIMUM_WORD_LENGTH; ++ f )
		{
		if ( is_good( f, ptr, a_iLength ) )
			{
			good = true;
			break;
			}
		}
	if ( good )
		good = ! HSpellCheckerService::get_instance().spell_check( a_oWord );
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

HLogic::ptr_t HBoggleCreator::do_new_instance( HString const& a_oArgv )
	{
	M_PROLOG
	out << "creating logic: " << a_oArgv << endl;
	HTokenizer t( a_oArgv, "," );
	HString l_oName = t[ 0 ];
	HString l_oPlayers = t[ 1 ];
	HString l_oRoundTime = t[ 2 ];
	HString l_oMaxRounds = t[ 3 ];
	HString l_oInterRoundDelay = t[ 4 ];
	int l_iPlayers = lexical_cast<int>( l_oPlayers );
	int l_iRoundTime = lexical_cast<int>( l_oRoundTime );
	int l_iMaxRounds = lexical_cast<int>( l_oMaxRounds );
	int l_iInterRoundDelay = lexical_cast<int>( l_oInterRoundDelay );
	return ( HLogic::ptr_t( new boggle::HBoggle( l_oName,
					l_iPlayers,
					l_iRoundTime,
					l_iMaxRounds,
					l_iInterRoundDelay ) ) );
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
	HString l_oSetup;
	l_oSetup.format( "%s:%d,%d,%d,%d", "bgl", setup.f_iPlayers, setup.f_iRoundTime, setup.f_iMaxRounds, setup.f_iInterRoundDelay );
	factory.register_logic_creator( l_oSetup, &boggleCreator );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

}

