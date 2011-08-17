/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	chatd.cxx - this file is integral part of `gameground' project.

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
#include "chatd.hxx"

#include "setup.hxx"
#include "clientinfo.hxx"
#include "logicfactory.hxx"
#include "spellchecker.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace gameground
{

namespace chat
{

HChat::HChat( HServer* server_, HLogic::id_t const& id_, HString const& comment_ )
	: HLogic( server_, id_, "chat", comment_ )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HChat::~HChat ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

bool HChat::do_accept( OClientInfo* clientInfo_ )
	{
	out << "new candidate " << clientInfo_->_login << endl;
	return ( false );
	}

void HChat::do_post_accept( OClientInfo* )
	{
	M_PROLOG
	HLock l( _mutex );
	return;
	M_EPILOG
	}

void HChat::do_kick( OClientInfo* )
	{
	M_PROLOG
	HLock l( _mutex );
	return;
	M_EPILOG
	}

yaal::hcore::HString HChat::do_get_info() const
	{
	HLock l( _mutex );
	return ( HString( "chat," ) + get_comment() );
	}

}

namespace logic_factory
{

class HChatCreator : public HLogicCreatorInterface
	{
protected:
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, HString const& );
	virtual HString do_get_info( void ) const;
	} chatCreator;

HLogic::ptr_t HChatCreator::do_new_instance( HServer* server_, HLogic::id_t const& id_, HString const& argv_ )
	{
	M_PROLOG
	out << "creating logic: " << argv_ << endl;
	HString name( get_token( argv_, ",", 0 ) );
	return ( make_pointer<chat::HChat>( server_, id_, name ) );
	M_EPILOG
	}

HString HChatCreator::do_get_info( void ) const
	{
	M_PROLOG
	HString setup;
	setup.format( "chat:" );
	out << setup << endl;
	return ( setup );
	M_EPILOG
	}

namespace
{

bool registrar( void )
	{
	M_PROLOG
	bool volatile failed( false );
	HLogicFactory& factory( HLogicFactoryInstance::get_instance() );
	factory.register_logic_creator( HTokenizer( chatCreator.get_info(), ":"  )[0], &chatCreator );
	return ( failed );
	M_EPILOG
	}

bool volatile registered = registrar();

}

}

}

