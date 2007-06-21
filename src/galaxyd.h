/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	galaxyd.h - this file is integral part of `galaxy' project.

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

#ifndef __GALAXYD_H
#define __GALAXYD_H

#include <yaal/yaal.h>

#include "logic.h"

namespace galaxy
{

class HSystem;
class HGalaxy;
class HFleet
	{
protected:
	/*{*/
	int f_iSize;
	int f_iEmperor;
	int f_iArrivalTime;
	/*}*/
public:
	/*{*/
	/*}*/
protected:
	/*{*/
	/*}*/
	friend class HSystem;
	friend class HGalaxy;
	};

class HGalaxy;
class HSystem
	{
protected:
	/*{*/
	typedef yaal::hcore::HList<HFleet> attackers_t;
	int f_iId;
	int f_iCoordinateX;
	int f_iCoordinateY;
	int f_iProduction;
	int f_iFleet;
	int f_iEmperor;
	attackers_t f_oAttackers;
	/*}*/
public:
	/*{*/
	HSystem ( void );
	void do_round ( HGalaxy & );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	HSystem ( HSystem const & );
	HSystem * operator = ( HSystem const & );
	/*}*/
	friend class HGalaxy;
	};

class HGalaxy : public HLogic
	{
	typedef void ( HGalaxy::*handler_t ) ( int, yaal::hcore::HString & );
	typedef yaal::hcore::HHashMap < yaal::hcore::HString, handler_t > handlers_t;
	typedef yaal::hcore::HHashMap < int, yaal::hcore::HString > names_t;
	typedef yaal::hcore::HArray < int > int_array_t;
protected:
	/*{*/
	int f_iBoardSize;
	int f_iSystems;
	int f_iEmperors;
	int f_iRound;
	int f_iReady;
	yaal::hcore::HArray < HSystem > f_oSystems;
	yaal::hcore::HSocket::ptr_t f_oSocket;
	handlers_t f_oHandlers;
	names_t f_oNames;
	int_array_t f_oColors;
	int_array_t f_oEmperorCounter;
	/*}*/
public:
	/*{*/
	HGalaxy ( int, int, int );
	virtual ~HGalaxy ( void );
	void set_socket ( yaal::hcore::HSocket::ptr_t );
	void process_command ( int, yaal::hcore::HString & );
	int get_color ( int );
	yaal::hcore::HSocket::ptr_t get_socket ( int );
	void mark_alive ( int );
	/*}*/
protected:
	/*{*/
	void handler_login ( int, yaal::hcore::HString & );
	int assign_system ( int, int & );
	void broadcast ( yaal::hcore::HString const & );
	void handler_message ( int, yaal::hcore::HString & );
	void handler_play ( int, yaal::hcore::HString & );
	/*}*/
private:
	/*{*/
	HGalaxy ( HGalaxy const & );
	HGalaxy & operator = ( HGalaxy const & );
	/*}*/
	friend class HServer;
	};

}

#endif /* not __GALAXYD_H */

