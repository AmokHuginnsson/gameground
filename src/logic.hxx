/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GAMEGROUND_LOGIC_HXX_INCLUDED
#define GAMEGROUND_LOGIC_HXX_INCLUDED

#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hhashmap.hxx>
#include <yaal/hcore/hthread.hxx>
#include <yaal/hcore/hstreaminterface.hxx>
#include <yaal/tools/hstringstream.hxx>

namespace gameground {

struct HClient;
class HServer;
class HLogic;

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, HLogic const* );

class HLogic {
public:
	typedef HLogic this_type;
	typedef yaal::hcore::HString id_t;
	typedef yaal::hcore::HPointer<HLogic> ptr_t;
	typedef yaal::hcore::HSet<HClient*> clients_t;
	typedef void ( HLogic::*handler_t ) ( HClient*, yaal::hcore::HString const& );
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
	yaal::hcore::HString _bcastPrefix;
private:
	yaal::hcore::HString _cache;
public:
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
	bool process_command( HClient*, yaal::hcore::HString const& );
	bool accept_client( HClient* );
	void post_accept_client( HClient* );
	int active_clients( void ) const;
	yaal::hcore::HString const& get_comment() const;
	yaal::hcore::HString get_info() const;
	void kick_client( HClient*, yaal::hcore::HString const& = yaal::hcore::HString() );
	id_t id( void ) const;
	yaal::hcore::HString const& get_name( void ) const;
	bool is_private( void ) const;
	/*}*/
protected:
	/*{*/
	void drop_client( HClient* );
	virtual bool do_accept( HClient* ) = 0;
	virtual void do_post_accept( HClient* ) = 0;
	virtual void do_kick( HClient* ) = 0;
	virtual bool do_is_private( void ) const;
	virtual yaal::hcore::HString do_get_info() const = 0;
	void broadcast( yaal::hcore::HString const& );
	void handler_message( HClient*, yaal::hcore::HString const& );
	void handler_play( HClient*, yaal::hcore::HString const& );
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

