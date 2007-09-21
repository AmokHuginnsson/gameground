/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	server.h - this file is integral part of `galaxy' project.

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

class HServer : public yaal::hcore::HProcess
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
	/*}*/
public:
	/*{*/
	HServer( int );
	int init_server( int );
	using yaal::hcore::HProcess::run;
	/*}*/
protected:
	/*{*/
	int handler_connection( int );
	int handler_message( int );
	void kick_client( yaal::hcore::HSocket::ptr_t&, char const* const = NULL );
	void broadcast( OClientInfo&, yaal::hcore::HString const& );
	void set_client_name( OClientInfo&, yaal::hcore::HString const& );
	void pass_command( OClientInfo&, yaal::hcore::HString const& );
	void create_game( OClientInfo&, yaal::hcore::HString const& );
	void join_game( OClientInfo&, yaal::hcore::HString const& );
	void send_logics_info( OClientInfo& );
	/*}*/
	};

int main_server( void );

#endif /* not __SERVER_H */

