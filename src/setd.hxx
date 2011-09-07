/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	setd.hxx - this file is integral part of `gameground' project.

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

#ifndef GAMEGROUND_SETD_HXX_INCLUDED
#define GAMEGROUND_SETD_HXX_INCLUDED

#include <yaal/yaal.hxx>

#include "logic.hxx"

namespace gameground
{

namespace set_bang
{

class HSetBang : public HLogic
	{
	struct OPlayerInfo
		{
		int _score;
		int _last;
		OPlayerInfo( void ) : _score( 0 ), _last( 0 ) {}
		};
	struct PROTOCOL : public HLogic::PROTOCOL
		{
		static char const* const PLAY;
		};
protected:
	/*{*/
	typedef yaal::hcore::HMap<OClientInfo*, OPlayerInfo> players_t;
	int _startupPlayers;
	players_t _players;
	yaal::hcore::HString _varTmpBuffer;
	/*}*/
public:
	/*{*/
	HSetBang( HServer*, HLogic::id_t const&, yaal::hcore::HString const&, int );
	virtual ~HSetBang( void );
	/*}*/
protected:
	/*{*/
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_play( OClientInfo*, yaal::hcore::HString const& );
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

