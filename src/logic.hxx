/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logic.hxx - this file is integral part of `gameground' project.

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

#ifndef GAMEGROUND_LOGIC_HXX_INCLUDED
#define GAMEGROUND_LOGIC_HXX_INCLUDED

#include <yaal/yaal.hxx>

namespace gameground {

struct OClientInfo;
class HServer;
class HLogic;

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, HLogic const* );

class HLogic {
public:
	typedef yaal::hcore::HString id_t;
	typedef yaal::hcore::HPointer<HLogic> ptr_t;
	typedef yaal::hcore::HSet<OClientInfo*> clients_t;
	typedef void ( HLogic::*handler_t ) ( OClientInfo*, yaal::hcore::HString const& );
protected:
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, handler_t> handlers_t;
	HServer* _server;
	/*{*/
	id_t _id;
	yaal::hcore::HString _name;
	handlers_t _handlers;
	clients_t _clients;
	yaal::hcore::HString _comment;
	yaal::tools::HStringStream _out;
	mutable yaal::hcore::HMutex _mutex;
	/*}*/
	struct PROTOCOL {
		static char const* const SEP;
		static char const* const SEPP;
		static char const* const SAY;
		static char const* const MSG;
		static char const* const PARTY;
		static char const* const PLAYER;
		static char const* const PLAYER_QUIT;
		virtual ~PROTOCOL(){}
	};
public:
	/*{*/
	HLogic( HServer*, id_t const&, yaal::hcore::HString const& );
	virtual ~HLogic( void );
	bool process_command( OClientInfo*, yaal::hcore::HString const& );
	bool accept_client( OClientInfo* );
	void post_accept_client( OClientInfo* );
	int active_clients( void ) const;
	yaal::hcore::HString const& get_comment() const;
	yaal::hcore::HString get_info() const;
	void kick_client( OClientInfo*, char const* const = NULL );
	id_t id( void ) const;
	yaal::hcore::HString const& get_name( void ) const;
	bool is_private( void ) const;
	/*}*/
protected:
	/*{*/
	void drop_client( OClientInfo* );
	virtual bool do_accept( OClientInfo* ) = 0;
	virtual void do_post_accept( OClientInfo* ) = 0;
	virtual void do_kick( OClientInfo* ) = 0;
	virtual bool do_is_private( void ) const;
	virtual yaal::hcore::HString do_get_info() const = 0;
	void broadcast( yaal::hcore::HString const& );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void party( yaal::hcore::HStreamInterface& ) const;
	friend yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, HLogic const& );
	/*}*/
private:
	/*{*/
	HLogic( HLogic const& );
	HLogic& operator = ( HLogic const& );
	/*}*/
	friend class HServer;
};

typedef yaal::hcore::HExceptionT<HLogic> HLogicException;

class HLogicCreatorInterface {
protected:
	virtual void do_initialize_globals( void ){};
	virtual void do_cleanup_globals( void ){};
	virtual HLogic::ptr_t do_new_instance( HServer*, HLogic::id_t const&, yaal::hcore::HString const& ) = 0;
	virtual yaal::hcore::HString do_get_info( void ) const = 0;
public:
	virtual ~HLogicCreatorInterface( void ){}
	void initialize_globals( void );
	void cleanup_globals( void );
	yaal::hcore::HString get_info( void ) const;
	HLogic::ptr_t new_instance( HServer*, HLogic::id_t const&, yaal::hcore::HString const& );
};

}

#endif /* not GAMEGROUND_LOGIC_HXX_INCLUDED */

