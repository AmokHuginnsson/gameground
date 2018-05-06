/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GAMEGROUND_SERVER_HXX_INCLUDED
#define GAMEGROUND_SERVER_HXX_INCLUDED

#include <yaal/hcore/hnumber.hxx>
#include <yaal/hcore/hsocket.hxx>
#include <yaal/tools/hiodispatcher.hxx>
#include <yaal/tools/hexclusiveaccessor.hxx>
#include <yaal/dbwrapper/hdatabase.hxx>

#include "client.hxx"
#include "logic.hxx"

namespace gameground {

class HServer {
public:
	typedef HServer this_type;
protected:
	typedef void ( HServer::* handler_t )( HClient&, yaal::hcore::HString const& );
	typedef yaal::hcore::HMap<HLogic::id_t, HLogic::ptr_t> logics_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, handler_t> handlers_t;
	typedef yaal::hcore::HMap<yaal::tools::HIODispatcher::stream_t::value_type const*, HClient> clients_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HClient*> logins_t;
	typedef yaal::hcore::HArray<HClient*> dropouts_t;
	typedef yaal::tools::HExclusiveAccessor<yaal::dbwrapper::HDataBase::ptr_t> db_accessor_t;
	int _maxConnections;
	yaal::hcore::HSocket::ptr_t _socket;
	clients_t _clients;
	logins_t _logins;
	logics_t _logics;
	handlers_t _handlers;
	yaal::tools::HStringStream _out;
	yaal::dbwrapper::HDataBase::ptr_t _db;
	yaal::hcore::HMutex _mutex;
	yaal::tools::HIODispatcher _dispatcher;
	yaal::hcore::HNumber _idPool;
	dropouts_t _dropouts;
public:
	struct PROTOCOL {
		static char const* const ABANDON;
		static char const* const ACCOUNT;
		static char const* const CLIENT_SETUP;
		static char const* const CMD;
		static char const* const CREATE;
		static char const* const ERR;
		static char const* const PARTY;
		static char const* const PARTY_INFO;
		static char const* const PARTY_CLOSE;
		static char const* const GET_PARTYS;
		static char const* const GET_PARTY_INFO;
		static char const* const GET_LOGICS;
		static char const* const GET_PLAYERS;
		static char const* const GET_ACCOUNT;
		static char const* const JOIN;
		static char const* const KCK;
		static char const* const LOGIC;
		static char const* const MSG;
		static char const* const SAY;
		static char const* const LOGIN;
		static char const* const PLAYER;
		static char const* const PLAYER_QUIT;
		static char const* const QUIT;
		static char const* const SEP;
		static char const* const SEPP;
		static char const* const SHUTDOWN;
		static char const* const VERSION;
		static char const* const VERSION_ID;
		static char const* const WARN;
	};
	HServer( int );
	~HServer( void );
	int init_server( int );
	void run( void );
	void drop_client( HClient* );
	HClient* get_client( yaal::hcore::HString const& );
	db_accessor_t db( void );
	void join_party( HClient&, yaal::hcore::HString const& );
	void handle_get_account( HClient&, yaal::hcore::HString const& );
protected:
	void handler_connection( yaal::tools::HIODispatcher::stream_t& );
	void handler_message( yaal::tools::HIODispatcher::stream_t& );
	void handler_shutdown( HClient&, yaal::hcore::HString const& );
	void handler_quit( HClient&, yaal::hcore::HString const& );
	void handler_abandon( HClient&, yaal::hcore::HString const& );
	void handler_chat( HClient&, yaal::hcore::HString const& );
	void kick_client( HClient&, char const* const = NULL );
	void broadcast( yaal::hcore::HString const& );
	void broadcast_party( yaal::hcore::HString const&, yaal::hcore::HString const& );
	void broadcast_private( HLogic&, yaal::hcore::HString const& );
	void broadcast_all_parties( HClient*, yaal::hcore::HString const& );
	void handle_login( HClient&, yaal::hcore::HString const& );
	void websocket_handshake( HClient&, yaal::hcore::HString const& );
	void handle_account( HClient&, yaal::hcore::HString const& );
	void pass_command( HClient&, yaal::hcore::HString const& );
	void create_party( HClient&, yaal::hcore::HString const& );
	void handle_get_logics( HClient&, yaal::hcore::HString const& );
	void handle_get_players( HClient&, yaal::hcore::HString const& );
	void handle_get_partys( HClient&, yaal::hcore::HString const& );
	void send_logics_info( HClient& );
	void send_players_info( HClient& );
	void send_player_info( HClient&, HClient& );
	void broadcast_player_info( HClient& );
	void broadcast_player_info( HClient&, HLogic& );
	void send_partys_info( HClient& );
	void remove_client_from_logic( HClient&, HLogic::ptr_t, yaal::hcore::HString const& = yaal::hcore::HString() );
	void remove_client_from_all_logics( HClient& );
	void flush_logics( void );
	void update_last_activity( HClient const& );
	void flush_droupouts( void );
	HLogic::id_t create_id( void );
	void free_id( HLogic::id_t const& );
	void cleanup( void );
};

int main_server( void );

}

#endif /* not GAMEGROUND_SERVER_HXX_INCLUDED */

