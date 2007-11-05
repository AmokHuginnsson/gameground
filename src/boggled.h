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
		typedef yaal::hcore::HSet<yaal::hcore::HString> word_set_t;
		typedef yaal::hcore::HPointer<word_set_t, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> word_set_ptr_t;
		int f_iScore;
		int f_iLast;
		word_set_ptr_t f_oWords;
		OPlayerInfo( void ) : f_iScore( 0 ), f_iLast( 0 ), f_oWords() {}
		};
	typedef yaal::hcore::HMap<OClientInfo*, OPlayerInfo> players_t;
protected:
	/*{*/
	int f_iPlayers;
	int f_iRoundTime;
	int f_iMaxRounds;
	int f_iRound;
	players_t f_oPlayers;
	int f_ppiGame[16][2];
	/*}*/
public:
	/*{*/
	HBoggle( yaal::hcore::HString const&, int, int, int );
	virtual ~HBoggle( void );
	int get_color( OClientInfo* );
	void generate_game( void );
	virtual yaal::hcore::HString get_info() const;
	/*}*/
protected:
	/*{*/
	OPlayerInfo* get_player_info( OClientInfo* );
	virtual bool do_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void send_map( void );
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

