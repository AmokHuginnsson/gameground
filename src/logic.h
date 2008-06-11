/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logic.h - this file is integral part of `gameground' project.

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

#ifndef __LOGIC_H
#define __LOGIC_H

#include <yaal/yaal.h>

namespace gameground
{

struct OClientInfo;

class HLogic
	{
public:
	typedef yaal::hcore::HPointer<HLogic> ptr_t;
	typedef yaal::hcore::HSet<OClientInfo*> clients_t;
	typedef void ( HLogic::*handler_t ) ( OClientInfo*, yaal::hcore::HString const& );
private:
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, handler_t> handlers_t;
protected:
	/*{*/
	yaal::hcore::HString f_oSymbol;
	handlers_t f_oHandlers;
	clients_t f_oClients;
	yaal::hcore::HString f_oName;
	yaal::tools::HStringStream _out;
	/*}*/
	struct PROTOCOL
		{
		static char const* const SEP;
		static char const* const SEPP;
		static char const* const SAY;
		static char const* const MSG;
		static char const* const PLAYER;
		static char const* const PLAYER_QUIT;
		virtual ~PROTOCOL(){}
		};
public:
	/*{*/
	HLogic( yaal::hcore::HString const&, yaal::hcore::HString const& );
	virtual ~HLogic( void );
	bool process_command( OClientInfo*, yaal::hcore::HString const& );
	bool accept_client( OClientInfo* );
	int active_clients( void ) const;
	yaal::hcore::HString const& get_name() const;
	virtual yaal::hcore::HString get_info() const = 0;
	void kick_client( OClientInfo*, char const* const = NULL );
	/*}*/
protected:
	/*{*/
	virtual bool do_accept( OClientInfo* ) = 0;
	virtual void do_post_accept( OClientInfo* ) = 0;
	virtual void do_kick( OClientInfo* ) = 0;
	void broadcast( yaal::hcore::HString const& );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	/*}*/
private:
	/*{*/
	HLogic( HLogic const& );
	HLogic& operator = ( HLogic const& );
	/*}*/
	friend class HServer;
	};

typedef yaal::hcore::HExceptionT<HLogic> HLogicException;

class HLogicCreatorInterface
	{
protected:
	virtual void do_initialize_globals( void ){};
	virtual HLogic::ptr_t do_new_instance( yaal::hcore::HString const& ) = 0;
public:
	virtual ~HLogicCreatorInterface( void ){}
	void initialize_globals( void );
	HLogic::ptr_t new_instance( yaal::hcore::HString const& );
	};

}

#endif /* not __LOGIC_H */

