/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logicfactory.hxx - this file is integral part of `gameground' project.

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

#ifndef LOGICFACTORY_HXX_INCLUDED
#define LOGICFACTORY_HXX_INCLUDED

#include <yaal/hcore/hmap.hxx>
#include <yaal/hcore/hpointer.hxx>
#include <yaal/hcore/hsingleton.hxx>

#include "logic.hxx"

namespace gameground
{

class HLogicFactory
	{
public:
	struct OCreator
		{
		OCreator( void ) : f_oInfo(), f_poInstatiator( NULL ) {}
		OCreator( OCreator const& c ) : f_oInfo( c.f_oInfo ), f_poInstatiator( c.f_poInstatiator ) {}
		OCreator& operator = ( OCreator const& c ) { f_oInfo = c.f_oInfo; f_poInstatiator = c.f_poInstatiator; return ( *this ); }
		yaal::hcore::HString f_oInfo;
		HLogicCreatorInterface* f_poInstatiator;
		};
	typedef yaal::hcore::HMap<yaal::hcore::HString, OCreator> creators_t;
private:
	creators_t f_oCreators;
public:
	void register_logic_creator( yaal::hcore::HString const&, HLogicCreatorInterface* );
	HLogic::ptr_t create_logic( yaal::hcore::HString const&, yaal::hcore::HString const& );
	bool is_type_valid( yaal::hcore::HString const& );
	creators_t::iterator begin( void );
	creators_t::iterator end( void );
	void initialize_globals( void );
private:
	HLogicFactory( void ) : f_oCreators() {}
	~HLogicFactory( void ) {}
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HLogicFactory>;
	friend class yaal::hcore::HDestructor<HLogicFactory>;
	};

typedef yaal::hcore::HSingleton<HLogicFactory> HLogicFactoryInstance;

}

#endif /* not LOGICFACTORY_HXX_INCLUDED */

