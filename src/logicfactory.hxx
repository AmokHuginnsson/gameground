/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef LOGICFACTORY_HXX_INCLUDED
#define LOGICFACTORY_HXX_INCLUDED

#include <yaal/hcore/hmap.hxx>
#include <yaal/hcore/hpointer.hxx>
#include <yaal/hcore/hsingleton.hxx>

#include "logic.hxx"

namespace gameground {

class HLogicFactory {
public:
	typedef HLogicFactory this_type;
	struct OCreator {
		OCreator( void ) : _instantiator( NULL ) {}
		OCreator( OCreator const& c ) : _instantiator( c._instantiator ) {}
		OCreator& operator = ( OCreator const& c ) { _instantiator = c._instantiator; return ( *this ); }
		HLogicCreatorInterface* _instantiator;
		yaal::hcore::HString get_info( void ) const { return ( _instantiator->get_info() ); }
	};
	typedef yaal::hcore::HMap<yaal::hcore::HString, OCreator> creators_t;
private:
	creators_t _creators;
public:
	void register_logic_creator( yaal::hcore::HString const&, HLogicCreatorInterface* );
	HLogic::ptr_t create_logic( yaal::hcore::HString const&, HServer*, HLogic::id_t const&, yaal::hcore::HString const& );
	bool is_type_valid( yaal::hcore::HString const& );
	creators_t::iterator begin( void );
	creators_t::iterator end( void );
	void initialize_globals( void );
	void cleanup_globals( void );
private:
	HLogicFactory( void ) : _creators() {}
	~HLogicFactory( void ) {}
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HLogicFactory>;
	friend class yaal::hcore::HDestructor<HLogicFactory>;
};

typedef yaal::hcore::HSingleton<HLogicFactory> HLogicFactoryInstance;

}

#endif /* not LOGICFACTORY_HXX_INCLUDED */

