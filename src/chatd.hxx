/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GAMEGROUND_CHATD_HXX_INCLUDED
#define GAMEGROUND_CHATD_HXX_INCLUDED

#include <yaal/hcore/hstring.hxx>
#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hmap.hxx>

#include "logic.hxx"

namespace gameground {

namespace chat {

class HChat : public HLogic {
public:
	typedef HChat this_type;
	typedef HLogic base_type;
private:
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
	virtual bool do_accept( HClient* );
	virtual void do_post_accept( HClient* );
	virtual void do_kick( HClient* );
	void handler_message( HClient*, yaal::hcore::HString const& );
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

