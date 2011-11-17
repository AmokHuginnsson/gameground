/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logicfactory.hxx - this file is integral part of `gameground' project.

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

#ifndef LOGICFACTORY_HXX_INCLUDED
#define LOGICFACTORY_HXX_INCLUDED

#include <yaal/hcore/hmap.hxx>
#include <yaal/hcore/hpointer.hxx>
#include <yaal/hcore/hsingleton.hxx>

#include "logic.hxx"

namespace gameground {

class HLogicFactory {
public:
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

