/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	gomokud.hxx - this file is integral part of `gameground' project.

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

#ifndef GAMEGROUND_GOMOKUD_HXX_INCLUDED
#define GAMEGROUND_GOMOKUD_HXX_INCLUDED

#include <yaal/hcore/hmap.hxx>
#include <yaal/hcore/hstring.hxx>
#include <sgf/sgf.hxx>

#include "logic.hxx"

namespace gameground {

namespace gomoku {

class HGomoku : public HLogic {
	struct STONE {
		typedef char stone_t;
		static stone_t const NONE;
		static stone_t const BLACK;
		static stone_t const WHITE;
	};
	static int const GOBAN_SIZE = 15;
	static int const FIVE_IN_A_ROW = 5;
	struct PROTOCOL : public HLogic::PROTOCOL {
		static char const* const SPECTATOR;
		static char const* const CONTESTANT;
		static char const* const GETUP;
		static char const* const PLAY;
		static char const* const PUTSTONE;
		static char const* const SGF;
		static char const* const SIT;
		static char const* const STONE;
		static char const* const TOMOVE;
		static char const* const FIVE_IN_A_ROW;
	};
protected:
	/*{*/
	typedef yaal::hcore::HMap<OClientInfo*, int> players_t;
	OClientInfo* _contestants[ 2 ];
	STONE::stone_t _state;
	int _move;
	int long _start;
	yaal::hcore::HChunk _game;
	sgf::SGF _sgf;
	players_t _players;
	yaal::hcore::HString _varTmpBuffer;
	/*}*/
public:
	/*{*/
	HGomoku( HServer*, HLogic::id_t const&, yaal::hcore::HString const& );
	virtual ~HGomoku( void );
	/*}*/
protected:
	/*{*/
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void reschedule_timeout( void );
	void send_goban( void );
	char& goban( int, int );
	char goban( int, int ) const;
	OClientInfo*& contestant( STONE::stone_t );
	void clear_goban( bool );
	HGomoku::STONE::stone_t opponent( STONE::stone_t );
	void make_move( int, int, STONE::stone_t );
	void contestant_gotup( OClientInfo* );
	void send_contestants( void );
	void send_contestant( char );
	int count_stones( STONE::stone_t );
	void handler_sit( OClientInfo*, yaal::hcore::HString const& );
	void handler_getup( OClientInfo*, yaal::hcore::HString const& );
	void handler_put_stone( OClientInfo*, yaal::hcore::HString const& );
	void broadcast_contestants( yaal::hcore::HString const& );
	void ensure_coordinates_validity( int, int );
	int is_winning_stone( int, int ) const;
	/*}*/
private:
	/*{*/
	HGomoku( HGomoku const& );
	HGomoku& operator = ( HGomoku const& );
	/*}*/
	friend class HServer;
};

}

}

#endif /* not GAMEGROUND_GOMOKUD_HXX_INCLUDED */

