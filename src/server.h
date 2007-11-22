/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	server.h - this file is integral part of `gameground' project.

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

#ifndef __SERVER_H
#define __SERVER_H

#include <yaal/yaal.h>

#include "clientinfo.h"
#include "logic.h"

class HServer : public yaal::tools::HProcess
	{
protected:
	/*{*/
	typedef void ( HServer::* handler_t )( OClientInfo&, yaal::hcore::HString const& );
	typedef yaal::hcore::HMap<yaal::hcore::HString, HLogic::ptr_t> logics_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, handler_t> handlers_t;
	typedef yaal::hcore::HMap<int, OClientInfo> clients_t;
	int f_iMaxConnections;
	yaal::hcore::HSocket f_oSocket;
	clients_t f_oClients;
	logics_t f_oLogics;
	handlers_t f_oHandlers;
	yaal::tools::HStringStream _out;
	/*}*/
	struct PROTOCOL
		{
		static char const* const ABANDON;
		static char const* const CMD;
		static char const* const CREATE;
		static char const* const ERR;
		static char const* const GAME;
		static char const* const GET_GAMES;
		static char const* const GET_GAME_INFO;
		static char const* const GET_LOGISTICS;
		static char const* const GET_PLAYERS;
		static char const* const JOIN;
		static char const* const KCK;
		static char const* const LOGIC;
		static char const* const MSG;
		static char const* const NAME;
		static char const* const PLAYER;
		static char const* const PLAYER_QUIT;
		static char const* const QUIT;
		static char const* const SEP;
		static char const* const SEPP;
		static char const* const SHUTDOWN;
		};
public:
	/*{*/
	HServer( int );
	~HServer( void );
	int init_server( int );
	using yaal::tools::HProcess::run;
	/*}*/
protected:
	/*{*/
	int handler_connection( int );
	int handler_message( int );
	void handler_shutdown( OClientInfo&, yaal::hcore::HString const& );
	void handler_quit( OClientInfo&, yaal::hcore::HString const& );
	void handler_abandon( OClientInfo&, yaal::hcore::HString const& );
	void handler_chat( OClientInfo&, yaal::hcore::HString const& );
	void kick_client( yaal::hcore::HSocket::ptr_t&, char const* const = NULL );
	void broadcast( yaal::hcore::HString const& );
	void broadcast_to_interested( yaal::hcore::HString const& );
	void set_client_name( OClientInfo&, yaal::hcore::HString const& );
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
	void remove_client_from_logic( OClientInfo& );
	/*}*/
	};

int main_server( void );

#endif /* not __SERVER_H */

