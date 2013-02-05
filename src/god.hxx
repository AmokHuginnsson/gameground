/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	god.hxx - this file is integral part of `gameground' project.

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

#ifndef GAMEGROUND_GOD_HXX_INCLUDED
#define GAMEGROUND_GOD_HXX_INCLUDED

#include <yaal/hcore/hmap.hxx>
#include <sgf/sgf.hxx>

#include "logic.hxx"

namespace gameground {

namespace go {

class HGo : public HLogic {
public:
	typedef HGo this_type;
	typedef HLogic base_type;
private:
	struct OPlayerInfo {
		int long _timeLeft;
		int _byoYomiPeriods;
		int _stonesCaptured;
		int _score;
		OClientInfo* _client;
		OPlayerInfo( void ) : _timeLeft( 0 ), _byoYomiPeriods( 0 ), _stonesCaptured( 0 ), _score( 0 ), _client( NULL ) {}
		void reset( int long time_, int score_, int byo_ ) {
			_timeLeft = time_;
			_byoYomiPeriods = byo_;
			_stonesCaptured = 0;
			_score = score_;
		}
	};
	struct STONE {
		typedef char stone_t;
		static stone_t const BLACK;
		static stone_t const WHITE;
		static stone_t const NONE;
		static stone_t const MARK;
		static stone_t const DEAD_WHITE;
		static stone_t const DEAD_BLACK;
		static stone_t const TERITORY_BLACK;
		static stone_t const TERITORY_WHITE;
		static stone_t const TERITORY_NONE;
	};
	struct GOBAN_SIZE {
		static int const NORMAL = 19;
		static int const SMALL = 13;
		static int const TINY = 9;
	};
	struct PROTOCOL : public HLogic::PROTOCOL {
		static char const* const ADMIN;
		static char const* const BYOYOMIPERIODS;
		static char const* const BYOYOMITIME;
		static char const* const CONTESTANT;
		static char const* const GETUP;
		static char const* const GOBAN;
		static char const* const HANDICAPS;
		static char const* const KOMI;
		static char const* const MAINTIME;
		static char const* const PASS;
		static char const* const PLAY;
		static char const* const NEWGAME;
		static char const* const UNDO;
		static char const* const PUTSTONE;
		static char const* const SELECT;
		static char const* const SETUP;
		static char const* const SGF;
		static char const* const SIT;
		static char const* const STONE;
		static char const* const TOMOVE;
		static char const* const DEAD;
		static char const* const ACCEPT;
	};
protected:
	/*{*/
	typedef yaal::hcore::HList<OClientInfo*> admin_queue_t;
	typedef yaal::hcore::HArray<int> path_t;
	typedef yaal::hcore::HArray<sgf::SGF::game_tree_t::const_node_t> branch_t;
	OPlayerInfo _contestants[ 2 ];
	STONE::stone_t _toMove;
	bool _marking;
	int _gobanSize;
	int _komi;
	int _handicaps;
	int _mainTime;
	int _byoYomiPeriods;
	int _byoYomiTime;
	int _move;
	int _pass;
	int long _start;
	yaal::hcore::HChunk _game;
	yaal::hcore::HChunk _koGame;
	yaal::hcore::HChunk _oldGame;
	sgf::SGF _sgf;
	admin_queue_t _adminQueue;
	path_t _path;
	branch_t _branch;
	yaal::hcore::HString _varTmpBuffer;
	/*}*/
public:
	/*{*/
	HGo( HServer*, id_t const&, yaal::hcore::HString const& );
	virtual ~HGo( void );
	/*}*/
protected:
	/*{*/
	OPlayerInfo* get_player_info( OClientInfo* );
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void handler_setup( OClientInfo*, yaal::hcore::HString const& );
	void on_timeout( void );
	void schedule_timeout( void );
	void reschedule_timeout( void );
	void reset_goban( bool );
	void set_handicaps( int );
	void put_handicap_stones( int, bool );
	void apply_move( sgf::SGF::game_tree_t::const_node_t );
	void put_stone( int, int, STONE::stone_t );
	void send_goban( OClientInfo* = NULL );
	bool have_liberties( int, int, STONE::stone_t );
	char& goban( int, int );
	OPlayerInfo& contestant( STONE::stone_t );
	OPlayerInfo const& contestant( STONE::stone_t ) const;
	OPlayerInfo* contestant( OClientInfo const* );
	OPlayerInfo const* contestant( OClientInfo const* ) const;
	void clear_goban( bool );
	bool have_killed( int, int, STONE::stone_t );
	HGo::STONE::stone_t opponent( STONE::stone_t );
	bool is_suicide( int, int, STONE::stone_t );
	bool is_ko( void );
	void make_move( int, int );
	void contestant_gotup( OClientInfo* );
	void send_contestants( OClientInfo* = NULL );
	void send_contestant( char, OClientInfo* );
	int count_stones( STONE::stone_t );
	void handler_sit( OClientInfo*, yaal::hcore::HString const& );
	void handler_getup( OClientInfo*, yaal::hcore::HString const& );
	void handler_put_stone( OClientInfo*, yaal::hcore::HString const& );
	void handler_pass( OClientInfo*, yaal::hcore::HString const& );
	void handler_sgf( OClientInfo*, yaal::hcore::HString const& );
	void handler_dead( OClientInfo*, yaal::hcore::HString const& );
	void handler_newgame( OClientInfo*, yaal::hcore::HString const& );
	void handler_select( OClientInfo*, yaal::hcore::HString const& );
	void handler_accept( OClientInfo* );
	void handler_undo( OClientInfo* );
	void broadcast_contestants( yaal::hcore::HString const& );
	void ensure_coordinates_validity( int, int );
	void mark_stone_dead( int, int );
	void commit( void );
	void count_score( void );
	void after_move( void );
	void mark_teritory( void );
	void apply_teritory_marker( void );
	void end_match( void );
	STONE::stone_t mark_teritory( int, int );
	void replace_stones( STONE::stone_t, STONE::stone_t );
	void update_clocks( void );
	void revoke_scheduled_tasks( void );
	bool is_admin( OClientInfo* ) const;
	bool can_play( OClientInfo* ) const;
	bool can_setup( OClientInfo* ) const;
	bool ongoing_match( void ) const;
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

#endif /* not GAMEGROUND_GOD_HXX_INCLUDED */

