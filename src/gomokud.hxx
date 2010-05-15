/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	god.hxx - this file is integral part of `gameground' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
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

namespace gomoku
{

class HGomoku : public HLogic
	{
	struct STONE
		{
		typedef char stone_t;
		static stone_t const NONE;
		static stone_t const BLACK;
		static stone_t const WHITE;
		};
	static int const GOBAN_SIZE = 15;
	struct PROTOCOL : public HLogic::PROTOCOL
		{
		static char const* const ADMIN;
		static char const* const CONTESTANT;
		static char const* const GETUP;
		static char const* const NAME;
		static char const* const PLAY;
		static char const* const PUTSTONE;
		static char const* const SETUP;
		static char const* const SIT;
		static char const* const STONE;
		static char const* const STONES;
		static char const* const TOMOVE;
		};
protected:
	/*{*/
	typedef yaal::hcore::HList<OClientInfo*> players_t;
	OClientInfo* _contestants[ 2 ];
	STONE::stone_t _state;
	int _move;
	int long _start;
	yaal::hcore::HChunk _game;
	players_t _players;
	yaal::hcore::HString _varTmpBuffer;
	mutable yaal::hcore::HMutex _mutex;
	/*}*/
public:
	/*{*/
	HGomoku( yaal::hcore::HString const& );
	virtual ~HGomoku( void );
	virtual yaal::hcore::HString get_info() const;
	/*}*/
protected:
	/*{*/
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
	void reschedule_timeout( void );
	void put_stone( int, int, STONE::stone_t );
	void send_goban( void );
	char& goban( int, int );
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
	void revoke_scheduled_tasks( void );
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

#endif /* not GOD_HXX_INCLUDED */

