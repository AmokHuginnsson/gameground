/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	god.h - this file is integral part of `gameground' project.

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

#ifndef __GOD_H
#define __GOD_H

#include <yaal/yaal.h>

#include "logic.h"

namespace go
{

class HGo : public HLogic
	{
	struct OPlayerInfo
		{
		int long f_iTimeLeft;
		int f_iByoYomiPeriodsLeft;
		int f_iByoYomiTimeLeft;
		int f_iStonesCaptured;
		OPlayerInfo( void ) : f_iTimeLeft( 0 ), f_iByoYomiPeriodsLeft( 0 ), f_iByoYomiTimeLeft( 0 ), f_iStonesCaptured( 0 ) {}
		};
	struct STONE
		{
		typedef enum
			{
			D_BLACK = 'b',
			D_WHITE = 'w',
			D_NONE = ' '
			} stone_t;
		};
	struct GOBAN_SIZE
		{
		static int const D_NORMAL = 19;
		static int const D_SMALL = 13;
		static int const D_TINY = 9;
		};
	struct PROTOCOL
		{
		static char const* const SEP;
		static char const* const SEPP;
		static char const* const NAME;
		static char const* const SETUP;
		static char const* const ADMIN;
		static char const* const SAY;
		static char const* const MSG;
		static char const* const PLAY;
		static char const* const GOBAN;
		static char const* const KOMI;
		static char const* const HANDICAPS;
		static char const* const MAINTIME;
		static char const* const BYOYOMIPERIODS;
		static char const* const BYOYOMITIME;
		static char const* const STONES;
		static char const* const PUTSTONE;
		static char const* const PASS;
		static char const* const SIT;
		static char const* const GETUP;
		};
protected:
	/*{*/
	typedef yaal::hcore::HPair<OClientInfo*, OPlayerInfo> player_t;
	typedef yaal::hcore::HList<player_t> players_t;
	OClientInfo* f_oContestants[ 2 ];
	STONE::stone_t f_eState;
	int f_iGobanSize;
	int f_iKomi;
	int f_iHandicaps;
	int long f_iMainTime;
	int f_iByoYomiPeriods;
	int f_iByoYomiTime;
	int f_iMove;
	int f_iPass;
	yaal::hcore::HPool<char> f_oGame;
	players_t f_oPlayers;
	yaal::hcore::HString f_oVarTmpBuffer;
	mutable yaal::hcore::HMutex f_oMutex;
	/*}*/
public:
	/*{*/
	HGo( yaal::hcore::HString const& );
	virtual ~HGo( void );
	virtual yaal::hcore::HString get_info() const;
	/*}*/
protected:
	/*{*/
	OPlayerInfo* get_player_info( OClientInfo* );
	players_t::iterator find_player( OClientInfo* );
	virtual bool do_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void handler_setup( OClientInfo*, yaal::hcore::HString const& );
	void on_timeout( void );
	void schedule( void );
	void schedule_timeout( void );
	void set_handicaps( int );
	void set_handi( int );
	void put_stone( int, int, STONE::stone_t );
	void send_goban( void );
	bool have_liberties( int, int, STONE::stone_t );
	char& goban( int, int );
	OClientInfo*& contestant( STONE::stone_t );
	OClientInfo*& contestant( char );
	void clear_goban( bool );
	bool have_killed( int, int, STONE::stone_t );
	HGo::STONE::stone_t oponent( STONE::stone_t );
	bool is_suicide( int, int, STONE::stone_t );
	bool is_ko( int, int, STONE::stone_t );
	void make_move( int, int, STONE::stone_t );
	void contestant_gotup( OClientInfo* );
	/*}*/
private:
	/*{*/
	HGo( HGo const& );
	HGo& operator = ( HGo const& );
	/*}*/
	friend class HServer;
	};

}

#endif /* not __GOD_H */

