/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	chatd.hxx - this file is integral part of `gameground' project.

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

#ifndef GAMEGROUND_CHATD_HXX_INCLUDED
#define GAMEGROUND_CHATD_HXX_INCLUDED

#include <yaal/hcore/hstring.hxx>
#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hmap.hxx>

#include "logic.hxx"

namespace gameground {

namespace chat {

class HChat : public HLogic {
	typedef yaal::hcore::HMap<yaal::hcore::HString, HLogic::ptr_t> chats_t;
	typedef yaal::hcore::HSet<yaal::hcore::HString> chatter_names_t;
	typedef yaal::hcore::HPointer<chatter_names_t> chatter_names_ptr_t;
	yaal::hcore::HString _key;
	chatter_names_ptr_t _chatterNames;
	yaal::hcore::HString _info;
public:
	HChat( HServer*, HLogic::id_t const&, chatter_names_ptr_t, yaal::hcore::HString const& );
	virtual ~HChat( void );
	static HLogic::ptr_t get_chat( HServer*, HLogic::id_t const&, yaal::hcore::HString const& );
	static void cleanup( void );
protected:
	virtual bool do_accept( OClientInfo* );
	virtual void do_post_accept( OClientInfo* );
	virtual void do_kick( OClientInfo* );
	void handler_message( OClientInfo*, yaal::hcore::HString const& );
	virtual yaal::hcore::HString do_get_info() const;
	virtual bool do_is_private( void ) const;
private:
	static chats_t _chats_;
	HChat( HChat const& );
	HChat& operator = ( HChat const& );
	friend class HServer;
};

}

}

#endif /* not GAMEGROUND_CHATD_HXX_INCLUDED */

