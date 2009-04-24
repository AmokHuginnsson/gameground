/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	gomokud.cxx - this file is integral part of `gameground' project.

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
#include "gomokud.hxx"

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

namespace gomoku
{

HGomoku::STONE::stone_t const HGomoku::STONE::D_BLACK = 'b';
HGomoku::STONE::stone_t const HGomoku::STONE::D_WHITE = 'w';
HGomoku::STONE::stone_t const HGomoku::STONE::D_NONE = ' ';

char const* const HGomoku::PROTOCOL::NAME = "gomoku";
char const* const HGomoku::PROTOCOL::SETUP = "setup";
char const* const HGomoku::PROTOCOL::ADMIN = "admin";
char const* const HGomoku::PROTOCOL::PLAY = "play";
char const* const HGomoku::PROTOCOL::CONTESTANT = "contestant";
char const* const HGomoku::PROTOCOL::STONES = "stones";
char const* const HGomoku::PROTOCOL::STONE = "stone";
char const* const HGomoku::PROTOCOL::TOMOVE = "to_move";
char const* const HGomoku::PROTOCOL::PUTSTONE = "put_stone";
char const* const HGomoku::PROTOCOL::SIT = "sit";
char const* const HGomoku::PROTOCOL::GETUP = "get_up";
static int const D_SECONDS_IN_MINUTE = 60;
static int const D_ACCEPTED = -7;

int const GO_MSG_NOT_YOUR_TURN = 0;
int const GO_MSG_YOU_CANT_DO_IT_NOW = 1;
int const GO_MSG_MALFORMED = 2;
char const* const GO_MSG[] =
	{
	_( "not your turn" ),
	_( "you cannot do it now" ),
	_( "malformed packet" )
	};

HGomoku::HGomoku( HString const& a_oName )
	: HLogic( "gomoku", a_oName ),
	f_eState( STONE::D_NONE ),
	f_iMove( 0 ), f_iStart( 0 ),
	f_oGame( D_GOBAN_SIZE * D_GOBAN_SIZE + sizeof ( '\0' ) ),
	f_oPlayers(), f_oVarTmpBuffer(), f_oMutex()
	{
	M_PROLOG
	f_ppoContestants[ 0 ] = f_ppoContestants[ 1 ] = NULL;
	HRandomizer l_oRandom;
	l_oRandom.set( time ( NULL ) );
	f_oHandlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HGomoku::handler_play );
	f_oHandlers[ PROTOCOL::SAY ] = static_cast<handler_t>( &HGomoku::handler_message );
	return;
	M_EPILOG
	}

HGomoku::~HGomoku ( void )
	{
	M_PROLOG
	revoke_scheduled_tasks();
	return;
	M_EPILOG
	}

void HGomoku::handler_message ( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::MSG << PROTOCOL::SEP << a_poClientInfo->f_oLogin << ": " << a_roMessage << endl << _out );
	return;
	M_EPILOG
	}

void HGomoku::broadcast_contestants( yaal::hcore::HString const& a_oMessage )
	{
	M_PROLOG
	M_ASSERT( f_ppoContestants[ 0 ] && f_ppoContestants[ 1 ] );
	f_ppoContestants[ 0 ]->f_oSocket->write_until_eos( a_oMessage );
	f_ppoContestants[ 1 ]->f_oSocket->write_until_eos( a_oMessage );
	return;
	M_EPILOG
	}

void HGomoku::handler_sit( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HString place = get_token( a_roMessage, ",", 1 );
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
			if ( ! ( contestant( STONE::D_BLACK ) && contestant( STONE::D_WHITE ) ) )
				{
				f_iStart = 0;
				f_iMove = 0;
				}
			else
				broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
						<< PROTOCOL::MSG << PROTOCOL::SEP << "The Gomoku match started." << endl << _out );
			}
		}
	return;
	M_EPILOG
	}

void HGomoku::handler_getup( OClientInfo* a_poClientInfo, HString const& /*a_roMessage*/ )
	{
	M_PROLOG
	contestant_gotup( a_poClientInfo );
	f_eState = STONE::D_NONE;
	return;
	M_EPILOG
	}

void HGomoku::handler_put_stone( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	if ( ( f_eState != STONE::D_BLACK ) && ( f_eState != STONE::D_WHITE ) )
		throw HLogicException( GO_MSG[ GO_MSG_YOU_CANT_DO_IT_NOW ] );
	if ( contestant( f_eState ) != a_poClientInfo )
		throw HLogicException( GO_MSG[ GO_MSG_NOT_YOUR_TURN ] );
	int col = lexical_cast<int>( get_token( a_roMessage, ",", 1 ) );
	int row = lexical_cast<int>( get_token( a_roMessage, ",", 2 ) );
	make_move( col, row, f_eState ); /* TODO implement winning condition test */
	f_eState = oponent( f_eState );
	send_goban();
	return;
	M_EPILOG
	}

void HGomoku::handler_play( OClientInfo* a_poClientInfo, HString const& a_roMessage )
	{
	M_PROLOG
	HLock l( f_oMutex );
	HString item = get_token( a_roMessage, ",", 0 );
	if ( ( item != PROTOCOL::SIT )
			&& ( contestant( STONE::D_BLACK ) != a_poClientInfo )
			&& ( contestant( STONE::D_WHITE ) != a_poClientInfo ) )
		throw HLogicException( "you are not playing" );
	if ( item == PROTOCOL::PUTSTONE )
		handler_put_stone( a_poClientInfo, a_roMessage );
	else if ( item == PROTOCOL::SIT )
		handler_sit( a_poClientInfo, a_roMessage );
	else if ( item == PROTOCOL::GETUP )
		handler_getup( a_poClientInfo, a_roMessage );
	else
		throw HLogicException( GO_MSG[ GO_MSG_MALFORMED ] );
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::TOMOVE << PROTOCOL::SEP
			<< static_cast<char>( f_eState ) << endl << _out );
	send_contestants();
	return;
	M_EPILOG
	}

bool HGomoku::do_accept( OClientInfo* a_poClientInfo )
	{
	out << "new candidate " << a_poClientInfo->f_oLogin << endl;
	return ( false );
	}

void HGomoku::do_post_accept( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	if ( f_oPlayers.size() == 0 )
		*a_poClientInfo->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::SETUP << PROTOCOL::SEP << PROTOCOL::ADMIN << endl;
	f_oPlayers.push_back( a_poClientInfo );
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

void HGomoku::do_kick( OClientInfo* a_poClientInfo )
	{
	M_PROLOG
	HLock l( f_oMutex );
	bool newadmin = false;
	
	if ( ! f_oPlayers.is_empty() && ( a_poClientInfo == *f_oPlayers.begin() ) )
		newadmin = true;
	f_oPlayers.pop_front();
	if ( newadmin )
		{
		players_t::iterator it = f_oPlayers.begin();
		if ( it != f_oPlayers.end() )
			*(*it)->f_oSocket << PROTOCOL::NAME << PROTOCOL::SEP
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
			<< "Player " << a_poClientInfo->f_oLogin << " left this match." << endl << _out );
	return;
	M_EPILOG
	}

yaal::hcore::HString HGomoku::get_info() const
	{
	HLock l( f_oMutex );
	return ( HString( "gomoku," ) + f_oName + "," + f_oPlayers.size() );
	}

void HGomoku::put_stone( int a_iCol, int a_iRow, STONE::stone_t a_eStone )
	{
	M_PROLOG
	f_oGame[ a_iRow * D_GOBAN_SIZE + a_iCol ] = a_eStone;
	return;
	M_EPILOG
	}

void HGomoku::send_goban( void )
	{
	M_PROLOG
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP << PROTOCOL::STONES << PROTOCOL::SEP << f_oGame.raw() << endl << _out );
	return;
	M_EPILOG
	}

char& HGomoku::goban( int a_iCol, int a_iRow )
	{
	return ( f_oGame[ a_iRow * D_GOBAN_SIZE + a_iCol ] );
	}

void HGomoku::clear_goban( bool removeDead )
	{
	for ( int i = 0; i < D_GOBAN_SIZE; i++ )
		{
		for ( int j = 0; j < D_GOBAN_SIZE; j++ )
			{
			if ( goban( i, j ) != STONE::D_NONE )
				{
				if ( removeDead && isupper( goban( i, j ) ) )
					goban( i, j ) = STONE::D_NONE;
				else
					goban( i, j ) = static_cast<char>( tolower( goban( i, j ) ) );
				}
			}
		}
	}

HGomoku::STONE::stone_t HGomoku::oponent( STONE::stone_t stone )
	{
	return ( stone == STONE::D_WHITE ? STONE::D_BLACK : STONE::D_WHITE );
	}

void HGomoku::ensure_coordinates_validity( int x, int y )
	{
	if ( ( x < 0 ) || ( x > ( D_GOBAN_SIZE - 1 ) )
			|| ( y < 0 ) || ( y > ( D_GOBAN_SIZE - 1 ) ) )
		throw HLogicException( "move outside goban" );
	}

void HGomoku::make_move( int x, int y, STONE::stone_t stone )
	{
	M_PROLOG
	ensure_coordinates_validity( x, y );
	goban( x, y ) = stone;
	return;
	M_EPILOG
	}

OClientInfo*& HGomoku::contestant( STONE::stone_t stone )
	{
	M_ASSERT( ( stone == STONE::D_BLACK ) || ( stone == STONE::D_WHITE ) );
	return ( ( stone == STONE::D_BLACK ) ? f_ppoContestants[ 0 ] : f_ppoContestants[ 1 ] );
	}

void HGomoku::contestant_gotup( OClientInfo* a_poClientInfo )
	{
	STONE::stone_t stone = ( contestant( STONE::D_BLACK ) == a_poClientInfo ? STONE::D_BLACK : STONE::D_WHITE );
	OClientInfo* foe = NULL;
	if ( ( f_eState != STONE::D_NONE )
			&& ( ( foe = contestant( STONE::D_BLACK ) ) || ( foe = contestant( STONE::D_WHITE ) ) ) )
		broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
				<< PROTOCOL::MSG << PROTOCOL::SEP
				<< a_poClientInfo->f_oLogin << " resigned - therefore " << foe->f_oLogin << " wins." << endl << _out );
	contestant( stone ) = NULL;
	f_eState = STONE::D_NONE;
	return;
	}

void HGomoku::send_contestants( void )
	{
	M_PROLOG
	send_contestant( STONE::D_BLACK );
	send_contestant( STONE::D_WHITE );
	return;
	M_EPILOG
	}

void HGomoku::send_contestant( char stone )
	{
	M_PROLOG
	OClientInfo* cinfo = contestant( stone );
	char const* name = "";
	if ( cinfo )
		name = cinfo->f_oLogin.raw();
	broadcast( _out << PROTOCOL::NAME << PROTOCOL::SEP
			<< PROTOCOL::CONTESTANT << PROTOCOL::SEP
			<< stone << PROTOCOL::SEPP
			<< name << PROTOCOL::SEPP << endl << _out );
	return;
	M_EPILOG
	}

void HGomoku::revoke_scheduled_tasks( void )
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

class HGomokuCreator : public HLogicCreatorInterface
	{
protected:
	virtual HLogic::ptr_t do_new_instance( HString const& );
	} gomokuCreator;

HLogic::ptr_t HGomokuCreator::do_new_instance( HString const& a_oArgv )
	{
	M_PROLOG
	out << "creating logic: " << a_oArgv << endl;
	HString l_oName = get_token( a_oArgv, ",", 0 );
	return ( HLogic::ptr_t( new gomoku::HGomoku( l_oName ) ) );
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
	l_oSetup.format( "gomoku:" );
	factory.register_logic_creator( l_oSetup, &gomokuCreator );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

}

