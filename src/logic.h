/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logic.h - this file is integral part of `galaxy' project.

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

class HLogic
	{
public:
	typedef yaal::hcore::HPointer<HLogic, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> ptr_t;
private:
	typedef void ( HLogic::*handler_t ) ( int, yaal::hcore::HString & );
	typedef yaal::hcore::HMap<int, yaal::hcore::HSocket::ptr_t> clients_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, handler_t> handlers_t;
protected:
	/*{*/
	handlers_t f_oHandlers;
	clients_t f_oClients;
	/*}*/
public:
	/*{*/
	HLogic( void );
	virtual ~HLogic( void );
	void set_socket( yaal::hcore::HSocket::ptr_t );
	virtual void process_command( int, yaal::hcore::HString & ) = 0;
	yaal::hcore::HSocket::ptr_t get_socket( int );
	/*}*/
protected:
	/*{*/
	void handler_login ( int, yaal::hcore::HString & );
	void broadcast( yaal::hcore::HString const & );
	void handler_message ( int, yaal::hcore::HString & );
	void handler_play ( int, yaal::hcore::HString & );
	/*}*/
private:
	/*{*/
	HLogic ( HLogic const & );
	HLogic & operator = ( HLogic const & );
	/*}*/
	friend class HServer;
	};

#endif /* not __LOGIC_H */

