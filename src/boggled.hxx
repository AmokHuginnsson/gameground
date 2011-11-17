/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	boggled.hxx - this file is integral part of `gameground' project.

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

#ifndef GAMEGROUND_BOGGLED_HXX_INCLUDED
#define GAMEGROUND_BOGGLED_HXX_INCLUDED

#include <yaal/yaal.hxx>

#include "logic.hxx"

namespace gameground {

namespace boggle {

class HBoggle : public HLogic {
	struct OPlayerInfo {
		int _score;
		int _last;
		OPlayerInfo( void ) : _score( 0 ), _last( 0 ) {}
	};
	struct STATE {
		typedef enum {
			LOCKED,
			ACCEPTING
		} state_t;
	};
	struct EVENT {
		typedef enum {
			BEGIN_ROUND,
			END_ROUND
		} event_t;
	};
public:
	struct SCORING {
		typedef enum {
			ORIGINAL = 0,
			FIBONACCI = 1,
			GEOMETRIC = 2,
			FIBONACCI_4 = 3,
			GEOMETRIC_4 = 4,
			LONGEST_WORDS = 5
		} scoring_t;
	};
	struct PROTOCOL : public HLogic::PROTOCOL {
		static char const* const NAME;
		static char const* const PLAY;
		static char const* const DECK;
		static char const* const SETUP;
		static char const* const ROUND;
		static char const* const END_ROUND;
		static char const* const SCORED;
		static char const* const LONGEST;
	};
protected:
	/*{*/
	typedef yaal::hcore::HSet<OClientInfo*> client_set_t;
	typedef yaal::hcore::HPointer<client_set_t> client_set_ptr_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, client_set_ptr_t> words_t;
	typedef yaal::hcore::HMap<OClientInfo*, OPlayerInfo> players_t;
	static int RULES[6][16];
	STATE::state_t _state;
	SCORING::scoring_t _scoring;
	int _startupPlayers;
	int _roundTime;
	int _maxRounds;
	int _interRoundDelay;
	int _round;
	players_t _players;
	char _game[16][2];
	words_t _words;
	/*}*/
public:
	/*{*/
	HBoggle( HServer*, id_t const&, yaal::hcore::HString const&, SCORING::scoring_t, int, int, int, int );
	virtual ~HBoggle( void );
	void generate_game( void );
	/*}*/
protected:
	/*{*/
	OPlayerInfo* get_player_info( OClientInfo* );
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void on_begin_round( void );
	void on_end_round( void );
	void schedule( EVENT::event_t );
	void schedule_end_round( void );
	bool word_is_good( yaal::hcore::HString const&, int );
	bool is_good( int, char const*, int );
	yaal::hcore::HString make_deck( void );
	/*}*/
private:
	/*{*/
	HBoggle( HBoggle const& );
	HBoggle& operator = ( HBoggle const& );
	/*}*/
	friend class HServer;
};

}

}

#endif /* not GAMEGROUND_BOGGLED_HXX_INCLUDED */

