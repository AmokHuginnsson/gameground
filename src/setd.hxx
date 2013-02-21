/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	setd.hxx - this file is integral part of `gameground' project.

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

#ifndef GAMEGROUND_SETD_HXX_INCLUDED
#define GAMEGROUND_SETD_HXX_INCLUDED

#include <yaal/hcore/hmap.hxx>

#include "logic.hxx"

namespace gameground {

namespace set_bang {

class HSetBang : public HLogic {
public:
	typedef HSetBang this_type;
	typedef HLogic base_type;
private:
	struct OPlayerInfo {
		int _score;
		int _last;
		OPlayerInfo( void ) : _score( 0 ), _last( 0 ) {}
	};
	struct PROTOCOL : public HLogic::PROTOCOL {
		static char const* const DECK;
		static char const* const SET;
	};
	static int const SET_DECK_CARD_COUNT = 3 * 3 * 3 * 3;
	static int const SET_TABLE_CARD_COUNT = 12;
protected:
	/*{*/
	typedef yaal::hcore::HMap<OClientInfo*, OPlayerInfo> players_t;
	int _startupPlayers;
	int _cardsOnTable;
	int _cardsInDeck;
	int _deckNo;
	int _deckCount;
	int _interRoundDelay;
	players_t _players;
	int _deck[SET_DECK_CARD_COUNT];
	yaal::hcore::HString _varTmpBuffer;
	/*}*/
public:
	/*{*/
	HSetBang( HServer*, HLogic::id_t const&, yaal::hcore::HString const&, int, int, int );
	virtual ~HSetBang( void );
	/*}*/
protected:
	/*{*/
	yaal::hcore::HString const& serialize_deck( void );
	void generate_deck( void );
	bool makes_set( int, int, int ) const;
	bool contains_set( int ) const;
	void update_table( void );
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_set( OClientInfo*, yaal::hcore::HString const& );
	/*}*/
private:
	/*{*/
	HSetBang( HSetBang const& );
	HSetBang& operator = ( HSetBang const& );
	/*}*/
	friend class HServer;
};

}

}

#endif /* not GAMEGROUND_SETD_HXX_INCLUDED */

