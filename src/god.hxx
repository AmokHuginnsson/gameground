/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	god.hxx - this file is integral part of `gameground' project.

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

#ifndef GOD_HXX_INCLUDED
#define GOD_HXX_INCLUDED

#include <yaal/yaal.hxx>

#include "logic.hxx"

namespace gameground
{

namespace go
{

class HGo : public HLogic
	{
	struct OPlayerInfo
		{
		int long f_iTimeLeft;
		int f_iByoYomiPeriods;
		int f_iStonesCaptured;
		int f_iScore;
		OPlayerInfo( void ) : f_iTimeLeft( 0 ), f_iByoYomiPeriods( 0 ), f_iStonesCaptured( 0 ), f_iScore( 0 ) {}
		};
	struct STONE
		{
		typedef char stone_t;
		static stone_t const D_BLACK;
		static stone_t const D_WHITE;
		static stone_t const D_NONE;
		static stone_t const D_MARK;
		static stone_t const D_DEAD_WHITE;
		static stone_t const D_DEAD_BLACK;
		static stone_t const D_TERITORY_BLACK;
		static stone_t const D_TERITORY_WHITE;
		static stone_t const D_TERITORY_NONE;
		};
	struct GOBAN_SIZE
		{
		static int const D_NORMAL = 19;
		static int const D_SMALL = 13;
		static int const D_TINY = 9;
		};
	struct PROTOCOL : public HLogic::PROTOCOL
		{
		static char const* const ADMIN;
		static char const* const BYOYOMIPERIODS;
		static char const* const BYOYOMITIME;
		static char const* const CONTESTANT;
		static char const* const GETUP;
		static char const* const GOBAN;
		static char const* const HANDICAPS;
		static char const* const KOMI;
		static char const* const MAINTIME;
		static char const* const NAME;
		static char const* const PASS;
		static char const* const PLAY;
		static char const* const PUTSTONE;
		static char const* const SETUP;
		static char const* const SIT;
		static char const* const STONE;
		static char const* const STONES;
		static char const* const TOMOVE;
		static char const* const DEAD;
		static char const* const ACCEPT;
		};
protected:
	/*{*/
	typedef yaal::hcore::HPair<OClientInfo*, OPlayerInfo> player_t;
	typedef yaal::hcore::HList<player_t> players_t;
	OClientInfo* f_ppoContestants[ 2 ];
	STONE::stone_t f_eState;
	int f_iGobanSize;
	int f_iKomi;
	int f_iHandicaps;
	int long f_iMainTime;
	int f_iByoYomiPeriods;
	int f_iByoYomiTime;
	int f_iMove;
	int f_iPass;
	int long f_iStart;
	yaal::hcore::HPool<char> f_oGame;
	yaal::hcore::HPool<char> f_oKoGame;
	yaal::hcore::HPool<char> f_oOldGame;
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
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void handler_setup( OClientInfo*, yaal::hcore::HString const& );
	void on_timeout( void );
	void schedule_timeout( void );
	void reschedule_timeout( void );
	void set_handicaps( int );
	void set_handi( int );
	void put_stone( int, int, STONE::stone_t );
	void send_goban( void );
	bool have_liberties( int, int, STONE::stone_t );
	char& goban( int, int );
	OClientInfo*& contestant( STONE::stone_t );
	void clear_goban( bool );
	bool have_killed( int, int, STONE::stone_t );
	HGo::STONE::stone_t oponent( STONE::stone_t );
	bool is_suicide( int, int, STONE::stone_t );
	bool is_ko( void );
	void make_move( int, int, STONE::stone_t );
	void contestant_gotup( OClientInfo* );
	void send_contestants( void );
	void send_contestant( char );
	int count_stones( STONE::stone_t );
	void handler_sit( OClientInfo*, yaal::hcore::HString const& );
	void handler_getup( OClientInfo*, yaal::hcore::HString const& );
	void handler_put_stone( OClientInfo*, yaal::hcore::HString const& );
	void handler_pass( OClientInfo*, yaal::hcore::HString const& );
	void handler_dead( OClientInfo*, yaal::hcore::HString const& );
	void handler_accept( OClientInfo* );
	void broadcast_contestants( yaal::hcore::HString const& );
	void ensure_coordinates_validity( int, int );
	void mark_stone_dead( int, int );
	void commit( void );
	void count_score( void );
	STONE::stone_t mark_teritory( int, int );
	void replace_stones( STONE::stone_t, STONE::stone_t );
	void update_clocks( void );
	void revoke_scheduled_tasks( void );
	/*}*/
private:
	/*{*/
	HGo( HGo const& );
	HGo& operator = ( HGo const& );
	/*}*/
	friend class HServer;
	};

}

}

#endif /* not GOD_HXX_INCLUDED */

