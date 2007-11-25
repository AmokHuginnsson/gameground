/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	boggled.h - this file is integral part of `gameground' project.

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

#ifndef __BOGGLED_H
#define __BOGGLED_H

#include <yaal/yaal.h>

#include "logic.h"

namespace boggle
{

class HBoggle : public HLogic
	{
	struct OPlayerInfo
		{
		int f_iScore;
		int f_iLast;
		OPlayerInfo( void ) : f_iScore( 0 ), f_iLast( 0 ) {}
		};
	struct STATE
		{
		typedef enum
			{
			D_LOCKED,
			D_ACCEPTING
			} state_t;
		};
	struct EVENT
		{
		typedef enum
			{
			D_BEGIN_ROUND,
			D_END_ROUND
			} event_t;
		};
protected:
	/*{*/
	typedef yaal::hcore::HSet<OClientInfo*> client_set_t;
	typedef yaal::hcore::HPointer<client_set_t, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> client_set_ptr_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, client_set_ptr_t> words_t;
	typedef yaal::hcore::HMap<OClientInfo*, OPlayerInfo> players_t;
	STATE::state_t f_eState;
	size_t f_iPlayers;
	int f_iRoundTime;
	int f_iMaxRounds;
	int f_iInterRoundDelay;
	int f_iRound;
	players_t f_oPlayers;
	char f_ppcGame[16][2];
	words_t f_oWords;
	mutable yaal::hcore::HMutex f_oMutex;
	/*}*/
	struct PROTOCOL
		{
		static char const* const SEP;
		static char const* const SEPP;
		static char const* const NAME;
		static char const* const SAY;
		static char const* const MSG;
		static char const* const PLAY;
		static char const* const PLAYER;
		static char const* const DECK;
		static char const* const ROUND;
		static char const* const SCORED;
		static char const* const LONGEST;
		};
public:
	/*{*/
	HBoggle( yaal::hcore::HString const&, int, int, int, int );
	virtual ~HBoggle( void );
	void generate_game( void );
	virtual yaal::hcore::HString get_info() const;
	/*}*/
protected:
	/*{*/
	OPlayerInfo* get_player_info( OClientInfo* );
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
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

#endif /* not __BOGGLED_H */

