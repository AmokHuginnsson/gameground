/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GAMEGROUND_GOMOKUD_HXX_INCLUDED
#define GAMEGROUND_GOMOKUD_HXX_INCLUDED

#include <yaal/hcore/hmap.hxx>
#include <yaal/hcore/hstring.hxx>
#include <sgf/sgf.hxx>

#include "logic.hxx"

namespace gameground {

namespace gomoku {

class HGomoku : public HLogic {
public:
	typedef HGomoku this_type;
	typedef HLogic base_type;
private:
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
	typedef yaal::hcore::HMap<HClient*, int> players_t;
	HClient* _contestants[ 2 ];
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
	virtual bool do_accept( HClient* );
	virtual void do_post_accept( HClient* );
	virtual void do_kick( HClient* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_play( HClient*, yaal::hcore::HString const& );
	void reschedule_timeout( void );
	void send_goban( void );
	char& goban( int, int );
	char goban( int, int ) const;
	HClient*& contestant( STONE::stone_t );
	void clear_goban( bool );
	HGomoku::STONE::stone_t opponent( STONE::stone_t );
	void make_move( int, int, STONE::stone_t );
	void contestant_gotup( HClient* );
	void send_contestants( void );
	void send_contestant( char );
	int count_stones( STONE::stone_t );
	void handler_sit( HClient*, yaal::hcore::HString const& );
	void handler_getup( HClient*, yaal::hcore::HString const& );
	void handler_put_stone( HClient*, yaal::hcore::HString const& );
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

