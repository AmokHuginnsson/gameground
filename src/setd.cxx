/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	setd.cxx - this file is integral part of `gameground' project.

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

#include <cstring>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "setd.hxx"

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

namespace set_bang
{

char const* const HSetBang::PROTOCOL::PLAY = "play";

HSetBang::HSetBang( HServer* server_, HLogic::id_t const& id_, HString const& comment_, int startupPlayers_ )
	: HLogic( server_, id_, comment_ ), _startupPlayers( startupPlayers_ ),
	_players(), _varTmpBuffer()
	{
	M_PROLOG
	_handlers[ PROTOCOL::PLAY ] = static_cast<handler_t>( &HSetBang::handler_play );
	return;
	M_EPILOG
	}

HSetBang::~HSetBang ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HSetBang::handler_play( OClientInfo* clientInfo_, HString const& message_ )
	{
	M_PROLOG
	HLock l( _mutex );
	*clientInfo_->_socket << PROTOCOL::MSG << PROTOCOL::SEP << message_ << endl;
	return;
	M_EPILOG
	}

bool HSetBang::do_accept( OClientInfo* clientInfo_ )
	{
	out << "new candidate " << clientInfo_->_login << endl;
	return ( false );
	}

void HSetBang::do_post_accept( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	out << "conditions: _players.size() = " << _players.size() << ", _startupPlayers = " << _startupPlayers << endl;
	OPlayerInfo info;
	_players[ clientInfo_ ] = info;
	/*
	 * Send proto info about new contestant to all players.
	 */
	broadcast( _out << PROTOCOL::PLAYER << PROTOCOL::SEP
			<< clientInfo_->_login << PROTOCOL::SEPP << 0 << PROTOCOL::SEPP << 0 << endl << _out );
	broadcast(
			_out << PROTOCOL::MSG << PROTOCOL::SEP
			<< clientInfo_->_login
			<< " joined the mind contest." << endl << _out );
	out << "player [" << clientInfo_->_login << "] accepted" << endl;
	return;
	M_EPILOG
	}

void HSetBang::do_kick( OClientInfo* clientInfo_ )
	{
	M_PROLOG
	HLock l( _mutex );
	_players.erase( clientInfo_ );
	broadcast( _out << PROTOCOL::MSG << PROTOCOL::SEP
			<< "Player " << clientInfo_->_login << " left this match." << endl << _out );
	return;
	M_EPILOG
	}

yaal::hcore::HString HSetBang::do_get_info() const
	{
	HLock l( _mutex );
	return ( HString( "set_bang," ) + get_comment() + "," + _players.size() );
	}

}

namespace logic_factory
{

class HSetBangCreator : public HLogicCreatorInterface
	{
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual HString do_get_info( void ) const;
	} setBangCreator;

HLogic::ptr_t HSetBangCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ )
	{
	M_PROLOG
	out << "creating logic: " << argv_ << endl;
	HTokenizer t( argv_, "," );
	HString name = t[ 0 ];
	int players = lexical_cast<int>( t[ 2 ] );
	return ( make_pointer<set_bang::HSetBang>( server_, id_, name, players ) );
	M_EPILOG
	}

HString HSetBangCreator::do_get_info( void ) const
	{
	M_PROLOG
	HString setupMsg;
	setupMsg.format( "set_bang:%d", setup._setStartupPlayers );
	out << setupMsg << endl;
	return ( setupMsg );
	M_EPILOG
	}

namespace
{

bool registrar( void )
	{
	M_PROLOG
	bool volatile failed = false;
	HLogicFactory& factory = HLogicFactoryInstance::get_instance();
	factory.register_logic_creator( HTokenizer( setBangCreator.get_info(), ":"  )[0], &setBangCreator );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

}

