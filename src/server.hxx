/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	server.hxx - this file is integral part of `gameground' project.

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

#ifndef SERVER_HXX_INCLUDED
#define SERVER_HXX_INCLUDED

#include <yaal/yaal.hxx>

#include "clientinfo.hxx"
#include "logic.hxx"

namespace gameground
{

class HServer
	{
protected:
	/*{*/
	typedef void ( HServer::* handler_t )( OClientInfo&, yaal::hcore::HString const& );
	typedef yaal::hcore::HMap<HLogic::id_t, HLogic::ptr_t> logics_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, handler_t> handlers_t;
	typedef yaal::hcore::HMap<int, OClientInfo> clients_t;
	typedef yaal::hcore::HArray<OClientInfo*> dropouts_t;
	int _maxConnections;
	yaal::hcore::HSocket _socket;
	clients_t _clients;
	logics_t _logics;
	handlers_t _handlers;
	yaal::tools::HStringStream _out;
	yaal::dbwrapper::HDataBase::ptr_t _db;
	yaal::tools::HIODispatcher _dispatcher;
	HLogic::id_t _idPool;
	dropouts_t _dropouts;
	/*}*/
	struct PROTOCOL
		{
		static char const* const ABANDON;
		static char const* const ACCOUNT;
		static char const* const CMD;
		static char const* const CREATE;
		static char const* const ERR;
		static char const* const GAME;
		static char const* const GET_GAMES;
		static char const* const GET_GAME_INFO;
		static char const* const GET_LOGICS;
		static char const* const GET_PLAYERS;
		static char const* const JOIN;
		static char const* const KCK;
		static char const* const LOGIC;
		static char const* const MSG;
		static char const* const LOGIN;
		static char const* const PLAYER;
		static char const* const PLAYER_QUIT;
		static char const* const QUIT;
		static char const* const SEP;
		static char const* const SEPP;
		static char const* const SHUTDOWN;
		static char const* const VERSION;
		static char const* const VERSION_ID;
		};
public:
	/*{*/
	HServer( int );
	~HServer( void );
	int init_server( int );
	void run( void );
	void drop_client( OClientInfo* );
	void disect_dropouts( void );
	/*}*/
protected:
	/*{*/
	void handler_connection( int );
	void handler_message( int );
	void handler_shutdown( OClientInfo&, yaal::hcore::HString const& );
	void handler_quit( OClientInfo&, yaal::hcore::HString const& );
	void handler_abandon( OClientInfo&, yaal::hcore::HString const& );
	void handler_chat( OClientInfo&, yaal::hcore::HString const& );
	void kick_client( yaal::hcore::HSocket::ptr_t&, char const* const = NULL );
	void broadcast( yaal::hcore::HString const& );
	void broadcast_loose( yaal::hcore::HString const& );
	void broadcast_party( yaal::hcore::HNumber const&, yaal::hcore::HString const& );
	void broadcast_all_parties( OClientInfo*, yaal::hcore::HString const& );
	void handle_login( OClientInfo&, yaal::hcore::HString const& );
	void handle_account( OClientInfo&, yaal::hcore::HString const& );
	void pass_command( OClientInfo&, yaal::hcore::HString const& );
	void create_game( OClientInfo&, yaal::hcore::HString const& );
	void join_game( OClientInfo&, yaal::hcore::HString const& );
	void get_logics_info( OClientInfo&, yaal::hcore::HString const& );
	void get_players_info( OClientInfo&, yaal::hcore::HString const& );
	void get_games_info( OClientInfo&, yaal::hcore::HString const& );
	void get_game_info( OClientInfo&, yaal::hcore::HString const& );
	void send_logics_info( OClientInfo& );
	void send_players_info( OClientInfo& );
	void send_games_info( OClientInfo& );
	void send_game_info( OClientInfo&, yaal::hcore::HString const& );
	void remove_client_from_logic( OClientInfo&, HLogic::ptr_t, char const* const = NULL );
	void remove_client_from_all_logics( OClientInfo& );
	void flush_logics( void );
	void update_last_activity( OClientInfo const& );
	void flush_droupouts( void );
	HLogic::id_t create_id( void );
	/*}*/
	};

int main_server( void );

}

#endif /* not SERVER_HXX_INCLUDED */

