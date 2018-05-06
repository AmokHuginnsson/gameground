/* Read gameground/LICENSE.md file for copyright and licensing information. */

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
	static int const SET_CERTAIN_SET = 21;
protected:
	/*{*/
	typedef yaal::hcore::HMap<HClient*, OPlayerInfo> players_t;
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
	OPlayerInfo* get_player_info( HClient* );
	yaal::hcore::HString const& serialize_deck( void );
	void generate_deck( void );
	bool makes_set( int, int, int ) const;
	bool contains_set( int ) const;
	void update_table( void );
	virtual bool do_accept( HClient* );
	virtual void do_post_accept( HClient* );
	virtual void do_kick( HClient* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_set( HClient*, yaal::hcore::HString const& );
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

