/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	god.cxx - this file is integral part of `gameground' project.

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
#include "god.hxx"

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

namespace go
{

HGo::STONE::stone_t const HGo::STONE::BLACK = 'b';
HGo::STONE::stone_t const HGo::STONE::WHITE = 'w';
HGo::STONE::stone_t const HGo::STONE::NONE = ' ';
HGo::STONE::stone_t const HGo::STONE::MARK = 'm';
HGo::STONE::stone_t const HGo::STONE::DEAD_BLACK = 's';
HGo::STONE::stone_t const HGo::STONE::DEAD_WHITE = 't';
HGo::STONE::stone_t const HGo::STONE::TERITORY_BLACK = 'p';
HGo::STONE::stone_t const HGo::STONE::TERITORY_WHITE = 'q';
HGo::STONE::stone_t const HGo::STONE::TERITORY_NONE = 'x';

char const* const HGo::PROTOCOL::NAME = "go";
char const* const HGo::PROTOCOL::SETUP = "setup";
char const* const HGo::PROTOCOL::ADMIN = "admin";
char const* const HGo::PROTOCOL::PLAY = "play";
char const* const HGo::PROTOCOL::CONTESTANT = "contestant";
char const* const HGo::PROTOCOL::GOBAN = "goban";
char const* const HGo::PROTOCOL::KOMI = "komi";
char const* const HGo::PROTOCOL::HANDICAPS = "handicaps";
char const* const HGo::PROTOCOL::MAINTIME = "maintime";
char const* const HGo::PROTOCOL::BYOYOMIPERIODS = "byoyomiperiods";
char const* const HGo::PROTOCOL::BYOYOMITIME = "byoyomitime";
char const* const HGo::PROTOCOL::STONES = "stones";
char const* const HGo::PROTOCOL::STONE = "stone";
char const* const HGo::PROTOCOL::TOMOVE = "to_move";
char const* const HGo::PROTOCOL::PUTSTONE = "put_stone";
char const* const HGo::PROTOCOL::PASS = "pass";
char const* const HGo::PROTOCOL::SIT = "sit";
char const* const HGo::PROTOCOL::GETUP = "get_up";
char const* const HGo::PROTOCOL::DEAD = "dead";
char const* const HGo::PROTOCOL::ACCEPT = "accept";
static int const SECONDS_IN_MINUTE = 60;
static int const ACCEPTED = -7;

int const GO_MSG_NOT_YOUR_TURN = 0;
int const GO_MSG_YOU_CANT_DO_IT_NOW = 1;
int const GO_MSG_MALFORMED = 2;
char const* const GO_MSG[] =
	{
	_( "not your turn" ),
	_( "you cannot do it now" ),
	_( "malformed packet" )
	};

HGo::HGo( HString const& a_oName )
	: HLogic( "go", a_oName ),
	f_eState( STONE::NONE ), f_iGobanSize( setup.f_iGobanSize ),
	f_iKomi( setup.f_iKomi ), f_iHandicaps( setup.f_iHandicaps ), f_iMainTime( setup.f_iMainTime ),
	f_iByoYomiPeriods( setup.f_iByoYomiPeriods ), f_iByoYomiTime( setup.f_iByoYomiTime ),
	f_iMove( 0 ), f_iPass( 0 ), f_iStart( 0 ),
	f_oGame( GOBAN_SIZE::NORMAL * GOBAN_SIZE::NORMAL + sizeof ( '\0' ) ),
	f_oKoGame( GOBAN_SIZE::NORMAL * GOBAN_SIZE::NORMAL + sizeof ( '\0' ) ),
	f_oOldGame( GOBAN_SIZE::NORMAL * GOBAN_SIZE::NORMAL + sizeof ( '\0' ) ),
	f_oPlayers(), f_oVarTmpBuffer(), f_oMutex()
	{
	M_PROLOG
	f_ppoContestants[ 0 ] = f_ppoContestants[ 1 ] = NULL;
	HRandomizer l_oRandom;
	l_oRandom.set( time ( NULL ) );
	f_oHandlers[ PROTOCOL::SETUP ] = static_cast<handler_t>( &HGo::handler_setup );
	f_oHandlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HGo::handler_play );
	f_oHandlers[ PROTOCOL::SAY ] = static_cast<handler_t>( &HGo::handler_message );
	set_handicaps( f_iHandicaps );
	return;
	M_EPILOG
	}

HGo::~HGo ( void )
	{
	M_PROLOG
	revoke_scheduled_tasks();
	return;
	M_EPILOG
	}

void HGo::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP << a_poClientInfo->f_oLogin << ": " << a_roMessage << endl << _out );
	return;
	M_EPILOG
	}

void HGo::broadcast_contestants( yaal::hcore::HString const& a_oMessage )
	{
	M_PROLOG
	M_ASSERT( f_ppoContestants[ 0 ] && f_ppoContestants[ 1 ] );
	f_ppoContestants[ 0 ]->f_oSocket->write_until_eos( a_oMessage );
	f_ppoContestants[ 1 ]->f_oSocket->write_until_eos( a_oMessage );
	return;
	M_EPILOG
	}

void HGo::handler_setup( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	if ( *f_oClients.begin() != a_poClientInfo )
		throw HLogicException( "you are not admin" );
	if ( f_eState != STONE::NONE )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	HString item = get_token( a_roMessage, ",", 0 );
	int value = lexical_cast<int>( get_token( a_roMessage, ",", 1 ) );
	if ( item == PROTOCOL::GOBAN )
		{
		f_iGobanSize = value;
		set_handicaps( f_iHandicaps );
		}
	else if ( item == PROTOCOL::KOMI )
		f_iKomi = value;
	else if ( item == PROTOCOL::HANDICAPS )
		set_handicaps( value );
	else if ( item == PROTOCOL::MAINTIME )
		f_iMainTime = value;
	else if ( item == PROTOCOL::BYOYOMIPERIODS )
		f_iByoYomiPeriods = value;
	else if ( item == PROTOCOL::BYOYOMITIME )
		f_iByoYomiTime = value;
	else
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::SETUP << PROTOCOL::SEP << a_roMessage << endl << _out );
	return;
	M_EPILOG
	}

void HGo::handler_sit( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HString place = get_token( a_roMessage, ",", 1 );
	if ( place.get_length() < 1 )
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	else
		{
		char stone = place[ 0 ];
		if ( ( stone != STONE::BLACK ) && ( stone != STONE::WHITE ) )
			throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
		else if ( ( contestant( STONE::BLACK ) == a_poClientInfo )
				|| ( contestant( STONE::WHITE ) == a_poClientInfo ) )
			throw HLogicException( "you were already sitting" );
		else if ( contestant( stone ) != NULL )
			*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP << "Some one was faster." << endl;
		else
			{
			contestant( stone ) = a_poClientInfo;
			OPlayerInfo& info = *get_player_info( a_poClientInfo );
			info.f_iTimeLeft = f_iMainTime * SECONDS_IN_MINUTE;
			info.f_iByoYomiPeriods = f_iByoYomiPeriods;
			info.f_iStonesCaptured = 0;
			info.f_iScore = ( stone == STONE::WHITE ? f_iKomi : 0 );
			if ( ! ( contestant( STONE::BLACK ) && contestant( STONE::WHITE ) ) )
				{
				f_iStart = 0;
				f_iMove = 0;
				set_handicaps( f_iHandicaps );
				}
			else
				{
				OPlayerInfo& foe = *get_player_info( contestant( oponent( stone ) ) );
				foe.f_iTimeLeft = info.f_iTimeLeft;
				foe.f_iByoYomiPeriods = info.f_iByoYomiPeriods;
				f_eState = ( f_iHandicaps > 1 ? STONE::WHITE : STONE::BLACK );
				f_iPass = 0;
				broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
						<< PROTOCOL::MSG << PROTOCOL::SEP << "The Go match started." << endl << _out );
				}
			}
		}
	return;
	M_EPILOG
	}

void HGo::handler_getup( OClientInfo* a_poClientInfo, HString const& /*a_roMessage*/ )
	{
	M_PROLOG
	contestant_gotup( a_poClientInfo );
	f_eState = STONE::NONE;
	return;
	M_EPILOG
	}

void HGo::handler_put_stone( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	if ( ( f_eState != STONE::BLACK ) && ( f_eState != STONE::WHITE ) )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	if ( contestant( f_eState ) != a_poClientInfo )
		throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	f_iPass = 0;
	int col = lexical_cast<int>( get_token( a_roMessage, ",", 1 ) );
	int row = lexical_cast<int>( get_token( a_roMessage, ",", 2 ) );
	int before = count_stones( oponent( f_eState ) );
	make_move( col, row, f_eState );
	int after = count_stones( oponent( f_eState ) );
	get_player_info( contestant( f_eState ) )->f_iStonesCaptured += ( before - after );
	f_eState = oponent( f_eState );
	send_goban();
	return;
	M_EPILOG
	}

void HGo::handler_pass( OClientInfo* a_poClientInfo, HString const& /*a_roMessage*/ )
	{
	M_PROLOG
	if ( ( f_eState != STONE::BLACK ) && ( f_eState != STONE::WHITE ) )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	if ( contestant( f_eState ) != a_poClientInfo )
		throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	f_eState = oponent( f_eState );
	++ f_iPass;
	if ( f_iPass == 3 )
		{
		f_eState = STONE::MARK;
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP << "The match has ended." << endl << _out );
		broadcast_contestants( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP << "Select your dead stones." << endl << _out );
		}
	return;
	M_EPILOG
	}

void HGo::handler_dead( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	if ( f_eState != STONE::MARK )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	HString str;
	for ( int i = 1; ! ( str = get_token( a_roMessage, ",", i ) ).is_empty() ; i += 2 )
		{
		int col = lexical_cast<int>( str );
		int row = lexical_cast<int>( get_token( a_roMessage, ",", i + 1 ) );
		out << "dead: " << col << "," << row << endl;
		ensure_coordinates_validity( col, row );
		STONE::stone_t stone = goban( col, row );
		if( ! ( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE )
					|| ( stone == STONE::DEAD_BLACK ) || ( stone == STONE::DEAD_WHITE ) ) )
			throw HLogicException( "no stone here" );
		if ( contestant( goban( col, row ) ) != a_poClientInfo )
			throw HLogicException( "not your stone" );
		mark_stone_dead( col, row );
		}
	send_goban();
	return;
	M_EPILOG
	}

void HGo::handler_accept( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	if ( f_eState != STONE::MARK )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	OPlayerInfo& info = *get_player_info( a_poClientInfo );
	if ( info.f_iByoYomiPeriods == ACCEPTED )
		throw HLogicException( "you already accepted stones" );
	info.f_iByoYomiPeriods = ACCEPTED;
	if ( ( get_player_info( f_ppoContestants[ 0 ] )->f_iByoYomiPeriods == ACCEPTED )
			&& ( get_player_info( f_ppoContestants[ 1 ] )->f_iByoYomiPeriods == ACCEPTED ) )
		{
		count_score();
		f_eState = STONE::NONE;
		}
	return;
	M_EPILOG
	}

void HGo::count_score( void )
	{
	M_PROLOG
	for ( int i = 0; i < ( f_iGobanSize * f_iGobanSize ); ++ i )
		{
		int x = i / f_iGobanSize;
		int y = i % f_iGobanSize;
		if ( goban( x, y ) == STONE::NONE )
			{
			STONE::stone_t teritory = mark_teritory( x, y );
			STONE::stone_t mark = STONE::TERITORY_NONE;
			switch ( teritory )
				{
				case ( STONE::BLACK ): mark = STONE::TERITORY_BLACK; break;
				case ( STONE::WHITE ): mark = STONE::TERITORY_WHITE; break;
				case ( STONE::TERITORY_NONE ): mark = STONE::TERITORY_NONE; break;
				default:
					out << "teritory: '" << teritory << "'" << endl;
					M_ASSERT( ! "bug in count_score switch" );
				}
			replace_stones( static_cast<char>( toupper( STONE::TERITORY_NONE ) ), mark );
			}
		}
	replace_stones( static_cast<char>( toupper( STONE::DEAD_BLACK ) ), STONE::DEAD_BLACK );
	replace_stones( static_cast<char>( toupper( STONE::DEAD_WHITE ) ), STONE::DEAD_WHITE );
	commit();
	int l_iBlackTeritory = count_stones( STONE::TERITORY_BLACK );
	int l_iWhiteTeritory = count_stones( STONE::TERITORY_WHITE );
	int l_iBlackCaptures = count_stones( STONE::DEAD_BLACK );
	int l_iWhiteCaptures = count_stones( STONE::DEAD_WHITE );
	l_iBlackTeritory += l_iWhiteCaptures;
	l_iWhiteTeritory += l_iBlackCaptures;
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
			<< "The game results are: " << endl << _out );
	OPlayerInfo& b = *get_player_info( f_ppoContestants[ 0 ] );
	OPlayerInfo& w = *get_player_info( f_ppoContestants[ 1 ] );
	b.f_iStonesCaptured += l_iWhiteCaptures;
	w.f_iStonesCaptured += l_iBlackCaptures;
	b.f_iScore = b.f_iStonesCaptured + l_iBlackTeritory;
	w.f_iScore += ( w.f_iStonesCaptured + l_iWhiteTeritory );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Black teritory: " << l_iBlackTeritory
			<< ", captutes: " << b.f_iStonesCaptured << "." << endl << _out );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
			<< "White teritory: " << l_iWhiteTeritory
			<< ", captutes: " << w.f_iStonesCaptured
			<< ", and " << f_iKomi << " of komi." << endl << _out );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP
			<< ( b.f_iScore > w.f_iScore ? "Black" : "White" )
			<< " wins by " << ( b.f_iScore > w.f_iScore ? b.f_iScore - w.f_iScore : w.f_iScore - b.f_iScore ) + .5
			<< endl << _out );
	send_goban();
	return;
	M_EPILOG
	}

HGo::STONE::stone_t HGo::mark_teritory( int x, int y )
	{
	STONE::stone_t teritory = static_cast<char>( toupper( STONE::TERITORY_NONE ) );
	STONE::stone_t stone = goban( x, y );
	if ( ( stone == STONE::NONE ) || ( stone == STONE::DEAD_BLACK ) || ( stone == STONE::DEAD_WHITE ) )
		{
		if ( stone == STONE::NONE )
			goban( x, y ) = teritory;
		else
			goban( x, y ) = static_cast<char>( toupper( stone ) );
		int l_iBlackNeighbour = 0;
		int l_iWhiteNeighbour = 0;
		int l_iBothNeighbour = 0;
		int directs[][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
		for ( int i = 0; i < 4; ++ i )
			{
			int nx = x + directs[ i ][ 0 ];
			int ny = y + directs[ i ][ 1 ];
			if ( ( nx >= 0 ) && ( nx < f_iGobanSize )
				&& ( ny >= 0 ) && ( ny < f_iGobanSize ) )
				{
				STONE::stone_t neighbour = goban( nx, ny );
				if ( ( neighbour != teritory )
						&& ( neighbour != toupper( STONE::DEAD_BLACK ) )
						&& ( neighbour != toupper( STONE::DEAD_WHITE ) ) )
					{
					STONE::stone_t NewTeritory = mark_teritory( nx, ny );
					if ( NewTeritory == STONE::BLACK )
						++ l_iBlackNeighbour;
					else  if ( NewTeritory == STONE::WHITE )
						++ l_iWhiteNeighbour;
					else
						++ l_iBothNeighbour;
					}
				}
			}
		if ( l_iBothNeighbour || ( l_iBlackNeighbour && l_iWhiteNeighbour ) )
			teritory = STONE::TERITORY_NONE;
		else if ( l_iBlackNeighbour )
			teritory = STONE::BLACK;
		else
			teritory = STONE::WHITE;
		}
	else if ( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE ) || ( stone == toupper ( STONE::TERITORY_NONE ) ) )
		teritory = stone;
	else
		M_ASSERT( ( stone == toupper( STONE::DEAD_BLACK ) ) || ( stone == toupper( STONE::DEAD_WHITE ) ) );
	return ( teritory );
	}

void HGo::handler_play( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	HString item = get_token( a_roMessage, ",", 0 );
	if ( ( item != PROTOCOL::SIT )
			&& ( contestant( STONE::BLACK ) != a_poClientInfo )
			&& ( contestant( STONE::WHITE ) != a_poClientInfo ) )
		throw HLogicException( "you are not playing" );
	if ( item == PROTOCOL::PUTSTONE )
		handler_put_stone( a_poClientInfo, a_roMessage );
	else if ( item == PROTOCOL::PASS )
		handler_pass( a_poClientInfo, a_roMessage );
	else if ( item == PROTOCOL::SIT )
		handler_sit( a_poClientInfo, a_roMessage );
	else if ( item == PROTOCOL::GETUP )
		handler_getup( a_poClientInfo, a_roMessage );
	else if ( item == PROTOCOL::DEAD )
		handler_dead( a_poClientInfo, a_roMessage );
	else if ( item == PROTOCOL::ACCEPT )
		handler_accept( a_poClientInfo );
	else
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::TOMOVE << PROTOCOL::SEP
			<< static_cast<char>( f_eState ) << endl << _out );
	if ( ( f_eState == STONE::BLACK ) || ( f_eState == STONE::WHITE ) )
		update_clocks();
	send_contestants();
	return;
	M_EPILOG
	}

HGo::players_t::iterator HGo::find_player( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	players_t::iterator it;
	for ( it = f_oPlayers.begin(); it != f_oPlayers.end(); ++ it )
		if ( it->first == a_poClientInfo )
			break;
	return ( it );
	M_EPILOG
	}

HGo::OPlayerInfo* HGo::get_player_info( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	players_t::iterator it = find_player( a_poClientInfo );
	M_ASSERT( it != f_oPlayers.end() );
	return ( &it->second );
	M_EPILOG
	}

bool HGo::do_accept( OClientInfo* a_poClientInfo )
	{
	out << "new candidate " << a_poClientInfo->f_oLogin << endl;
	return ( false );
	}

void HGo::do_post_accept( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	if ( f_oPlayers.size() == 0 )
		*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::SETUP << PROTOCOL::SEP << PROTOCOL::ADMIN << endl;
	*a_poClientInfo->f_oSocket
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::GOBAN << PROTOCOL::SEPP << f_iGobanSize << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::KOMI << PROTOCOL::SEPP << f_iKomi << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::HANDICAPS << PROTOCOL::SEPP << f_iHandicaps << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::MAINTIME << PROTOCOL::SEPP << f_iMainTime << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::BYOYOMIPERIODS << PROTOCOL::SEPP << f_iByoYomiPeriods << endl
		<< PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::SETUP << PROTOCOL::SEP
		<< PROTOCOL::BYOYOMITIME << PROTOCOL::SEPP << f_iByoYomiTime << endl;
	player_t info;
	info.first = a_poClientInfo;
	f_oPlayers.push_back( info );
	for ( clients_t::HIterator it = f_oClients.begin(); it != f_oClients.end(); ++ it )
		{
		if ( *it != a_poClientInfo )
			{
			*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
					<< PROTOCOL::PLAYER << PROTOCOL::SEP
					<< (*it)->f_oLogin << endl;
			*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
					<< PROTOCOL::MSG << PROTOCOL::SEP
					<< "Player " << (*it)->f_oLogin << " approched this table." << endl;
			}
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::PLAYER << PROTOCOL::SEP
			<< a_poClientInfo->f_oLogin << endl << _out );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << a_poClientInfo->f_oLogin << " approched this table." << endl << _out );
	send_contestants();
	send_goban();
	return;
	M_EPILOG
	}

void HGo::do_kick( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	bool newadmin = false;
	players_t::iterator it = find_player( a_poClientInfo );
	if ( it == f_oPlayers.begin() )
		newadmin = true;
	f_oPlayers.erase( it );
	if ( newadmin )
		{
		it = f_oPlayers.begin();
		if ( it != f_oPlayers.end() )
			*it->first->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::SETUP << PROTOCOL::SEP << PROTOCOL::ADMIN << endl;
		}
	if ( ( contestant( STONE::BLACK ) == a_poClientInfo )
			|| ( contestant( STONE::WHITE ) == a_poClientInfo ) )
		{
		STONE::stone_t stone = ( contestant( STONE::BLACK ) == a_poClientInfo ? STONE::BLACK : STONE::WHITE );
		contestant_gotup( contestant( stone ) );
		send_contestants();
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << a_poClientInfo->f_oLogin << " left this match." << endl << _out );
	return;
	M_EPILOG
	}

yaal::hcore::HString HGo::get_info() const
	{
	HLock l( f_oMutex );
	return ( HString( "go," ) + f_oName + "," + f_oPlayers.size() + "," + f_iGobanSize + "," + f_iKomi + "," + f_iHandicaps + "," + f_iMainTime + "," + f_iByoYomiPeriods + "," + f_iByoYomiTime );
	}

void HGo::reschedule_timeout( void )
	{
	M_PROLOG
	HAsyncCallerService::get_instance().register_call( 0,
			HCallInterface::ptr_t( new HCall<HGo&, typeof( &HGo::schedule_timeout )>( *this, &HGo::schedule_timeout ) ) );
	return;
	M_EPILOG
	}

void HGo::schedule_timeout( void )
	{
	M_PROLOG
	++ f_iMove;
	OPlayerInfo& p = *get_player_info( contestant( f_eState ) );
	if ( p.f_iByoYomiPeriods < f_iByoYomiPeriods )
		p.f_iTimeLeft = f_iByoYomiTime * SECONDS_IN_MINUTE;
	HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + p.f_iTimeLeft,
			HCallInterface::ptr_t( new HCall<HGo&, typeof( &HGo::on_timeout )>( *this, &HGo::on_timeout ) ) );
	return;
	M_EPILOG
	}

void HGo::on_timeout( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	OPlayerInfo& p = *get_player_info( contestant( f_eState ) );
	-- p.f_iByoYomiPeriods;
	if ( p.f_iByoYomiPeriods < 0 )
		{
		f_eState = STONE::NONE;
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::TOMOVE << PROTOCOL::SEP
				<< static_cast<char>( f_eState ) << endl << _out );
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP << "End of time." << endl << _out );
		}
	else
		{
		p.f_iTimeLeft = f_iByoYomiTime * SECONDS_IN_MINUTE;
		reschedule_timeout();
		send_contestants();
		}
	return;
	M_EPILOG
	}

void HGo::set_handicaps( int a_iHandicaps )
	{
	M_PROLOG
	if ( ( a_iHandicaps > 9 ) || ( a_iHandicaps < 0 ) )
		throw HLogicException( _out << "bad handicap value: " << a_iHandicaps << _out );
	::memset( f_oGame.raw(), STONE::NONE, f_iGobanSize * f_iGobanSize );
	f_oGame[ f_iGobanSize * f_iGobanSize ] = 0;
	::memset( f_oKoGame.raw(), STONE::NONE, f_iGobanSize * f_iGobanSize );
	f_oKoGame[ f_iGobanSize * f_iGobanSize ] = 0;
	if ( a_iHandicaps != f_iHandicaps )
		{
		if ( a_iHandicaps > 0 )
			f_iKomi = 0;
		else
			f_iKomi = setup.f_iKomi;
		}
	f_iHandicaps = a_iHandicaps;
	if ( f_iHandicaps > 1 )
		set_handi( f_iHandicaps );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::SETUP << PROTOCOL::SEP
			<< PROTOCOL::KOMI << PROTOCOL::SEPP << f_iKomi << endl << _out );
	send_goban();
	return;
	M_EPILOG
	}

void HGo::set_handi( int a_iHandi )
	{
	M_PROLOG
	int hoshi = 3 - ( f_iGobanSize == GOBAN_SIZE::TINY ? 1 : 0 );
	switch ( a_iHandi )
		{
		case ( 9 ):
			put_stone( f_iGobanSize / 2, f_iGobanSize / 2, STONE::BLACK );
		case ( 8 ):
			set_handi( 6 );
			put_stone( f_iGobanSize / 2, hoshi, STONE::BLACK );
			put_stone( f_iGobanSize / 2, ( f_iGobanSize - hoshi ) - 1, STONE::BLACK );
		break;
		case ( 7 ):
			put_stone( f_iGobanSize / 2, f_iGobanSize / 2, STONE::BLACK );
		case ( 6 ):
			set_handi( 4 );
			put_stone( hoshi, f_iGobanSize / 2, STONE::BLACK );
			put_stone( ( f_iGobanSize - hoshi ) - 1, f_iGobanSize / 2, STONE::BLACK );
		break;
		case ( 5 ):
			put_stone( f_iGobanSize / 2, f_iGobanSize / 2, STONE::BLACK );
		case ( 4 ):
			put_stone( ( f_iGobanSize - hoshi ) - 1, ( f_iGobanSize - hoshi ) - 1, STONE::BLACK );
		case ( 3 ):
			put_stone( hoshi, hoshi, STONE::BLACK );
		case ( 2 ):
			put_stone( hoshi, ( f_iGobanSize - hoshi ) - 1, STONE::BLACK );
			put_stone( ( f_iGobanSize - hoshi ) - 1, hoshi, STONE::BLACK );
		break;
		default:
			M_ASSERT( ! "unhandled case" );
		}
	return;
	M_EPILOG
	}

void HGo::put_stone( int a_iCol, int a_iRow, STONE::stone_t a_eStone )
	{
	M_PROLOG
	f_oGame[ a_iRow * f_iGobanSize + a_iCol ] = a_eStone;
	return;
	M_EPILOG
	}

void HGo::send_goban( void )
	{
	M_PROLOG
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::STONES << PROTOCOL::SEP << f_oGame.raw() << endl << _out );
	return;
	M_EPILOG
	}

char& HGo::goban( int a_iCol, int a_iRow )
	{
	return ( f_oKoGame[ a_iRow * f_iGobanSize + a_iCol ] );
	}

bool HGo::have_liberties( int a_iCol, int a_iRow, STONE::stone_t stone )
	{
	if ( ( a_iCol < 0 ) || ( a_iCol > ( f_iGobanSize - 1 ) )
			|| ( a_iRow < 0 ) || ( a_iRow > ( f_iGobanSize - 1 ) ) )
		return ( false );
	if ( goban( a_iCol, a_iRow ) == STONE::NONE )
		return ( true );
	if ( goban( a_iCol, a_iRow ) == stone )
		{
		goban( a_iCol, a_iRow ) = static_cast<char>( toupper( stone ) );
		return ( have_liberties( a_iCol, a_iRow - 1, stone )
				|| have_liberties( a_iCol, a_iRow + 1, stone )
				|| have_liberties( a_iCol - 1, a_iRow, stone )
				|| have_liberties( a_iCol + 1, a_iRow, stone ) );
		}
	return ( false );
	}

void HGo::clear_goban( bool removeDead )
	{
	for ( int i = 0; i < f_iGobanSize; i++ )
		{
		for ( int j = 0; j < f_iGobanSize; j++ )
			{
			if ( goban( i, j ) != STONE::NONE )
				{
				if ( removeDead && isupper( goban( i, j ) ) )
					goban( i, j ) = STONE::NONE;
				else
					goban( i, j ) = static_cast<char>( tolower( goban( i, j ) ) );
				}
			}
		}
	}

HGo::STONE::stone_t HGo::oponent( STONE::stone_t stone )
	{
	return ( stone == STONE::WHITE ? STONE::BLACK : STONE::WHITE );
	}

bool HGo::have_killed( int x, int y, STONE::stone_t stone )
	{
	bool killed = false;
	STONE::stone_t foeStone = oponent( stone );
	goban( x, y ) = stone;
	if ( ( x > 0 ) && ( goban( x - 1, y ) == foeStone ) && ( ! have_liberties( x - 1, y, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	if ( ( x < ( f_iGobanSize - 1 ) ) && ( goban( x + 1, y ) == foeStone ) && ( ! have_liberties( x + 1, y, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	if ( ( y > 0 ) && ( goban( x, y - 1 ) == foeStone ) && ( ! have_liberties( x, y - 1, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	if ( ( y < ( f_iGobanSize - 1 ) ) && ( goban( x, y + 1 ) == foeStone ) && ( ! have_liberties( x, y + 1, foeStone ) ) )
		clear_goban( killed = true );
	else
		clear_goban( false );
	goban( x, y ) = STONE::NONE;
	return ( killed );
	}

bool HGo::is_ko( void )
	{
	return ( ::memcmp( f_oKoGame.raw(), f_oOldGame.raw(), f_iGobanSize * f_iGobanSize ) == 0 );
	}

bool HGo::is_suicide( int x, int y, STONE::stone_t stone )
	{
	bool suicide = false;
	goban( x, y ) = stone;
	if ( ! have_liberties( x, y, stone ) )
		suicide = true;
	clear_goban( false );
	goban( x, y ) = STONE::NONE;
	return ( suicide );
	}	

void HGo::ensure_coordinates_validity( int x, int y )
	{
	if ( ( x < 0 ) || ( x > ( f_iGobanSize - 1 ) )
			|| ( y < 0 ) || ( y > ( f_iGobanSize - 1 ) ) )
		throw HLogicException( "move outside goban" );
	}

void HGo::make_move( int x, int y, STONE::stone_t stone )
	{
	M_PROLOG
	ensure_coordinates_validity( x, y );
	::memcpy( f_oKoGame.raw(), f_oGame.raw(), f_iGobanSize * f_iGobanSize );
	if ( goban( x, y ) != STONE::NONE )
		throw HLogicException( "position already occupied" );
	if ( ! have_killed( x, y, stone ) )
		{
		if ( is_suicide( x, y, stone ) )
			{
			clear_goban( false );
			throw HLogicException( "suicides forbidden" );
			}
		}	
	goban( x, y ) = stone;
	if ( is_ko() )
		throw HLogicException( "forbidden by ko rule" );
	::memcpy( f_oOldGame.raw(), f_oGame.raw(), f_iGobanSize * f_iGobanSize );
	commit();
	return;
	M_EPILOG
	}

void HGo::commit( void )
	{
	M_PROLOG
	::memcpy( f_oGame.raw(), f_oKoGame.raw(), f_iGobanSize * f_iGobanSize );
	return;
	M_EPILOG
	}

void HGo::mark_stone_dead( int col, int row )
	{
	M_PROLOG
	STONE::stone_t stone = goban( col, row );
	switch ( stone )
		{
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

OClientInfo*& HGo::contestant( STONE::stone_t stone )
	{
	M_ASSERT( ( stone == STONE::BLACK ) || ( stone == STONE::WHITE )
			|| ( stone == STONE::DEAD_BLACK ) || ( stone == STONE::DEAD_WHITE ) );
	return ( ( stone == STONE::BLACK ) || ( stone == STONE::DEAD_BLACK ) ? f_ppoContestants[ 0 ] : f_ppoContestants[ 1 ] );
	}

void HGo::contestant_gotup( OClientInfo* a_poClientInfo )
	{
	STONE::stone_t stone = ( contestant( STONE::BLACK ) == a_poClientInfo ? STONE::BLACK : STONE::WHITE );
	OClientInfo* foe = NULL;
	if ( ( f_eState != STONE::NONE )
			&& ( ( foe = contestant( STONE::BLACK ) ) || ( foe = contestant( STONE::WHITE ) ) ) )
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP
				<< a_poClientInfo->f_oLogin << " resigned - therefore " << foe->f_oLogin << " wins." << endl << _out );
	contestant( stone ) = NULL;
	f_eState = STONE::NONE;
	return;
	}

void HGo::send_contestants( void )
	{
	M_PROLOG
	send_contestant( STONE::BLACK );
	send_contestant( STONE::WHITE );
	return;
	M_EPILOG
	}

void HGo::send_contestant( char stone )
	{
	M_PROLOG
	OClientInfo* cinfo = contestant( stone );
	char const* name = "";
	int captured = 0;
	int time = 0;
	int byoyomi = 0;
	int score = 0;
	if ( cinfo )
		{
		OPlayerInfo& info = *get_player_info( cinfo );
		name = cinfo->f_oLogin.raw();
		captured = info.f_iStonesCaptured;
		time = static_cast<int>( info.f_iTimeLeft );
		byoyomi = info.f_iByoYomiPeriods;
		score = info.f_iScore;
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::CONTESTANT << PROTOCOL::SEP
			<< stone << PROTOCOL::SEPP
			<< name << PROTOCOL::SEPP
			<< captured << PROTOCOL::SEPP
			<< score << PROTOCOL::SEPP
			<< time << PROTOCOL::SEPP
			<< byoyomi << endl << _out );
	return;
	M_EPILOG
	}

int HGo::count_stones( STONE::stone_t stone )
	{
	int count = 0;
	int const size = f_iGobanSize * f_iGobanSize;
	char const* ptr = f_oGame.raw();
	for ( int i = 0; i < size; ++ i )
		if ( ptr[ i ] == stone )
			++ count;
	return ( count );
	}

void HGo::replace_stones( STONE::stone_t which, STONE::stone_t with )
	{
	int const size = f_iGobanSize * f_iGobanSize;
	char* ptr = f_oKoGame.raw();
	for ( int i = 0; i < size; ++ i )
		if ( ptr[ i ] == which )
			ptr[ i ] = with;
	return;
	}

void HGo::update_clocks( void )
	{
	M_PROLOG
	revoke_scheduled_tasks();
	int long l_iNow = time( NULL );
	OPlayerInfo& p = *get_player_info( contestant( oponent( f_eState ) ) );
	if ( f_iStart )
		p.f_iTimeLeft -= ( l_iNow - f_iStart );
	schedule_timeout();
	f_iStart = l_iNow;
	return;
	M_EPILOG
	}

void HGo::revoke_scheduled_tasks( void )
	{
	M_PROLOG
	HAsyncCallerService::get_instance().flush( this );
	HScheduledAsyncCallerService::get_instance().flush( this );
	return;
	M_EPILOG
	}

}

namespace logic_factory
{

class HGoCreator : public HLogicCreatorInterface
	{
protected:
	virtual HLogic::ptr_t do_new_instance( HString const& );
	} goCreator;

HLogic::ptr_t HGoCreator::do_new_instance( HString const& a_oArgv )
	{
	M_PROLOG
	out << "creating logic: " << a_oArgv << endl;
	HString l_oName = get_token( a_oArgv, ",", 0 );
	return ( HLogic::ptr_t( new go::HGo( l_oName ) ) );
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
	l_oSetup.format( "go:%d,%d,%d,%d,%d,%d", setup.f_iGobanSize, setup.f_iKomi, setup.f_iHandicaps,
			setup.f_iMainTime, setup.f_iByoYomiPeriods, setup.f_iByoYomiTime );
	factory.register_logic_creator( l_oSetup, &goCreator );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

}

