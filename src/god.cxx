/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	god.cxx - this file is integral part of `gameground' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <iostream>

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )
#include "god.h"

#include "setup.h"
#include "clientinfo.h"
#include "logicfactory.h"
#include "spellchecker.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace go
{

HGo::STONE::stone_t const HGo::STONE::D_BLACK = 'b';
HGo::STONE::stone_t const HGo::STONE::D_WHITE = 'w';
HGo::STONE::stone_t const HGo::STONE::D_NONE = ' ';
HGo::STONE::stone_t const HGo::STONE::D_MARK = 'm';
HGo::STONE::stone_t const HGo::STONE::D_DEAD_BLACK = 's';
HGo::STONE::stone_t const HGo::STONE::D_DEAD_WHITE = 't';

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
static int const D_SECONDS_IN_MINUTE = 60;

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
	f_eState( STONE::D_NONE ), f_iGobanSize( setup.f_iGobanSize ),
	f_iKomi( setup.f_iKomi ), f_iHandicaps( setup.f_iHandicaps ), f_iMainTime( setup.f_iMainTime ),
	f_iByoYomiPeriods( setup.f_iByoYomiPeriods ), f_iByoYomiTime( setup.f_iByoYomiTime ),
	f_iMove( 0 ), f_iPass( 0 ),
	f_oGame( GOBAN_SIZE::D_NORMAL * GOBAN_SIZE::D_NORMAL + sizeof ( '\0' ) ),
	f_oKoGame( GOBAN_SIZE::D_NORMAL * GOBAN_SIZE::D_NORMAL + sizeof ( '\0' ) ),
	f_oOldGame( GOBAN_SIZE::D_NORMAL * GOBAN_SIZE::D_NORMAL + sizeof ( '\0' ) ),
	f_oPlayers(), f_oVarTmpBuffer(), f_oMutex()
	{
	M_PROLOG
	f_poContestants[ 0 ] = f_poContestants[ 1 ] = NULL;
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
	HAsyncCallerService::get_instance().flush( this );
	HScheduledAsyncCallerService::get_instance().flush( this );
	return;
	M_EPILOG
	}

void HGo::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP << a_poClientInfo->f_oName << ": " << a_roMessage << endl << _out );
	return;
	M_EPILOG
	}

void HGo::broadcast_contestants( yaal::hcore::HString const& a_oMessage )
	{
	M_PROLOG
	M_ASSERT( f_poContestants[ 0 ] && f_poContestants[ 1 ] );
	f_poContestants[ 0 ]->f_oSocket->write_until_eos( a_oMessage );
	f_poContestants[ 1 ]->f_oSocket->write_until_eos( a_oMessage );
	return;
	M_EPILOG
	}

void HGo::handler_setup( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	if ( *f_oClients.begin() != a_poClientInfo )
		throw HLogicException( "you are not admin" );
	if ( f_eState != STONE::D_NONE )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	HString item = a_roMessage.split( ",", 0 );
	int value = lexical_cast<int>( a_roMessage.split( ",", 1 ) );
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
	HString place = a_roMessage.split( ",", 1 );
	if ( place.get_length() < 1 )
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	else
		{
		char stone = place[ 0 ];
		if ( ( stone != STONE::D_BLACK ) && ( stone != STONE::D_WHITE ) )
			throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
		else if ( ( contestant( STONE::D_BLACK ) == a_poClientInfo )
				|| ( contestant( STONE::D_WHITE ) == a_poClientInfo ) )
			throw HLogicException( "you were already sitting" );
		else if ( contestant( stone ) != NULL )
			*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP << "Some one was faster." << endl;
		else
			{
			contestant( stone ) = a_poClientInfo;
			OPlayerInfo& info = *get_player_info( a_poClientInfo );
			info.f_iTimeLeft = f_iMainTime * D_SECONDS_IN_MINUTE;
			info.f_iByoYomiPeriods = f_iByoYomiPeriods;
			if ( ! ( contestant( STONE::D_BLACK ) && contestant( STONE::D_WHITE ) ) )
				set_handicaps( f_iHandicaps );
			else
				{
				f_eState = ( f_iHandicaps > 1 ? STONE::D_WHITE : STONE::D_BLACK );
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
	if ( ( contestant( STONE::D_BLACK ) != a_poClientInfo )
			&& ( contestant( STONE::D_WHITE ) != a_poClientInfo ) )
		throw HLogicException( "you are not sitting" );
	else
		{
		contestant_gotup( a_poClientInfo );
		OClientInfo* foe = NULL;
		if ( ( foe = contestant( STONE::D_BLACK ) ) || ( foe = contestant( STONE::D_WHITE ) ) )
			{
			broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
					<< PROTOCOL::MSG << PROTOCOL::SEP
					<< a_poClientInfo->f_oName << " resigned - therefore " << foe->f_oName << " wins." << endl << _out );
			}
		f_eState = STONE::D_NONE;
		}
	return;
	M_EPILOG
	}

void HGo::handler_put_stone( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	if ( f_eState == STONE::D_NONE )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	if ( contestant( f_eState ) != a_poClientInfo )
		throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	f_iPass = 0;
	int col = lexical_cast<int>( a_roMessage.split( ",", 1 ) );
	int row = lexical_cast<int>( a_roMessage.split( ",", 2 ) );
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
	if ( ( f_eState != STONE::D_BLACK ) && ( f_eState != STONE::D_WHITE ) )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	if ( contestant( f_eState ) != a_poClientInfo )
		throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	f_eState = oponent( f_eState );
	++ f_iPass;
	if ( f_iPass == 3 )
		{
		f_eState = STONE::D_MARK;
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP << "The match has ended." << endl << _out );
		broadcast_contestants( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::MSG << PROTOCOL::SEP << "Select your dead stones." << endl << _out );
		}
	return;
	M_EPILOG
	}

void HGo::handler_dead( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	int col = lexical_cast<int>( a_roMessage.split( ",", 1 ) );
	int row = lexical_cast<int>( a_roMessage.split( ",", 2 ) );
	if ( f_eState != STONE::D_MARK )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	ensure_coordinates_validity( col, row );
	STONE::stone_t stone = goban( col, row );
	if( ! ( ( stone == STONE::D_BLACK ) || ( stone == STONE::D_WHITE )
				|| ( stone == STONE::D_DEAD_BLACK ) || ( stone == STONE::D_DEAD_WHITE ) ) )
		throw HLogicException( "no stone here" );
	if ( contestant( goban( col, row ) ) != a_poClientInfo )
		throw HLogicException( "not your stone" );
	mark_stone_dead( col, row );
	send_goban();
	return;
	M_EPILOG
	}

void HGo::handler_accept( OClientInfo* )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HGo::handler_play( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	HString item = a_roMessage.split( ",", 0 );
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
	out << "new candidate " << a_poClientInfo->f_oName << endl;
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
					<< (*it)->f_oName << endl;
			*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
					<< PROTOCOL::MSG << PROTOCOL::SEP
					<< "Player " << (*it)->f_oName << " approched this table." << endl;
			}
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::PLAYER << PROTOCOL::SEP
			<< a_poClientInfo->f_oName << endl << _out );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << a_poClientInfo->f_oName << " approched this table." << endl << _out );
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
	if ( ( contestant( STONE::D_BLACK ) == a_poClientInfo )
			|| ( contestant( STONE::D_WHITE ) == a_poClientInfo ) )
		{
		STONE::stone_t stone = ( contestant( STONE::D_BLACK ) == a_poClientInfo ? STONE::D_BLACK : STONE::D_WHITE );
		contestant_gotup( contestant( stone ) );
		send_contestants();
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << a_poClientInfo->f_oName << " left this match." << endl << _out );
	return;
	M_EPILOG
	}

yaal::hcore::HString HGo::get_info() const
	{
	HLock l( f_oMutex );
	return ( HString( "go," ) + f_oName + "," + f_oPlayers.size() + "," + f_iGobanSize + "," + f_iKomi + "," + f_iHandicaps + "," + f_iMainTime + "," + f_iByoYomiPeriods + "," + f_iByoYomiTime );
	}

void HGo::schedule( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	schedule_timeout();
	return;
	M_EPILOG
	}

void HGo::schedule_timeout( void )
	{
	M_PROLOG
	++ f_iMove;
/*
	HScheduledAsyncCallerService::get_instance().register_call( time( NULL ) + f_iRoundTime,
			HCallInterface::ptr_t( new HCall<HGo&, typeof( &HGo::on_timeout )>( *this, &HGo::on_timeout ) ) );
*/
	return;
	M_EPILOG
	}

void HGo::on_timeout( void )
	{
	M_PROLOG
	HLock l( f_oMutex );
	return;
	M_EPILOG
	}

void HGo::set_handicaps( int a_iHandicaps )
	{
	M_PROLOG
	if ( ( a_iHandicaps > 9 ) || ( a_iHandicaps < 0 ) )
		throw HLogicException( _out << "bad handicap value: " << a_iHandicaps << _out );
	::memset( f_oGame.raw(), STONE::D_NONE, f_iGobanSize * f_iGobanSize );
	f_oGame[ f_iGobanSize * f_iGobanSize ] = 0;
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
	int hoshi = 3 - ( f_iGobanSize == GOBAN_SIZE::D_TINY ? 1 : 0 );
	switch ( a_iHandi )
		{
		case ( 9 ):
			put_stone( f_iGobanSize / 2, f_iGobanSize / 2, STONE::D_BLACK );
		case ( 8 ):
			set_handi( 6 );
			put_stone( f_iGobanSize / 2, hoshi, STONE::D_BLACK );
			put_stone( f_iGobanSize / 2, ( f_iGobanSize - hoshi ) - 1, STONE::D_BLACK );
		break;
		case ( 7 ):
			put_stone( f_iGobanSize / 2, f_iGobanSize / 2, STONE::D_BLACK );
		case ( 6 ):
			set_handi( 4 );
			put_stone( hoshi, f_iGobanSize / 2, STONE::D_BLACK );
			put_stone( ( f_iGobanSize - hoshi ) - 1, f_iGobanSize / 2, STONE::D_BLACK );
		break;
		case ( 5 ):
			put_stone( f_iGobanSize / 2, f_iGobanSize / 2, STONE::D_BLACK );
		case ( 4 ):
			put_stone( ( f_iGobanSize - hoshi ) - 1, ( f_iGobanSize - hoshi ) - 1, STONE::D_BLACK );
		case ( 3 ):
			put_stone( hoshi, hoshi, STONE::D_BLACK );
		case ( 2 ):
			put_stone( hoshi, ( f_iGobanSize - hoshi ) - 1, STONE::D_BLACK );
			put_stone( ( f_iGobanSize - hoshi ) - 1, hoshi, STONE::D_BLACK );
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
	if ( goban( a_iCol, a_iRow ) == STONE::D_NONE )
		return ( true );
	if ( goban( a_iCol, a_iRow ) == stone )
		{
		goban( a_iCol, a_iRow ) = toupper( stone );	
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
			if ( goban( i, j ) != STONE::D_NONE )
				{
				if ( removeDead && isupper( goban( i, j ) ) )
					goban( i, j ) = STONE::D_NONE;
				else
					goban( i, j ) = tolower( goban( i, j ) );
				}
			}
		}
	}

HGo::STONE::stone_t HGo::oponent( STONE::stone_t stone )
	{
	return ( stone == STONE::D_WHITE ? STONE::D_BLACK : STONE::D_WHITE );
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
	goban( x, y ) = STONE::D_NONE;
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
	goban( x, y ) = STONE::D_NONE;
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
	if ( goban( x, y ) != STONE::D_NONE )
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
		case ( STONE::D_BLACK ) : stone = STONE::D_DEAD_BLACK; break;
		case ( STONE::D_WHITE ) : stone = STONE::D_DEAD_WHITE; break;
		case ( STONE::D_DEAD_BLACK ) : stone = STONE::D_BLACK; break;
		case ( STONE::D_DEAD_WHITE ) : stone = STONE::D_WHITE; break;
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
	M_ASSERT( ( stone == STONE::D_BLACK ) || ( stone == STONE::D_WHITE )
			|| ( stone == STONE::D_DEAD_BLACK ) || ( stone == STONE::D_DEAD_WHITE ) );
	return ( ( stone == STONE::D_BLACK ) || ( stone == STONE::D_DEAD_BLACK ) ? f_poContestants[ 0 ] : f_poContestants[ 1 ] );
	}

void HGo::contestant_gotup( OClientInfo* a_poClientInfo )
	{
	STONE::stone_t stone = ( contestant( STONE::D_BLACK ) == a_poClientInfo ? STONE::D_BLACK : STONE::D_WHITE );
	contestant( stone ) = NULL;
	return;
	}

void HGo::send_contestants( void )
	{
	M_PROLOG
	send_contestant( STONE::D_BLACK );
	send_contestant( STONE::D_WHITE );
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
	if ( cinfo )
		{
		OPlayerInfo& info = *get_player_info( cinfo );
		name = cinfo->f_oName;
		captured = info.f_iStonesCaptured;
		time = info.f_iTimeLeft;
		byoyomi = info.f_iByoYomiPeriods;
		}
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::CONTESTANT << PROTOCOL::SEP
			<< stone << PROTOCOL::SEPP
			<< name << PROTOCOL::SEPP
			<< captured << PROTOCOL::SEPP
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

}

namespace logic_factory
{

HLogic::ptr_t create_logic_go( HString const& a_oArgv )
	{
	M_PROLOG
	out << "creating logic: " << a_oArgv << endl;
	HString l_oName = a_oArgv.split( ",", 0 );
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
	factory.register_logic_creator( l_oSetup, create_logic_go );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

