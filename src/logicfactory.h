/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logicfactory.h - this file is integral part of `gameground' project.

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

#ifndef __LOGICFACTORY_H
#define __LOGICFACTORY_H

#include <yaal/hcore/hmap.h>
#include <yaal/hcore/hpointer.h>
#include <yaal/hcore/hsingleton.h>

#include "logic.h"

namespace gameground
{

class HLogicFactory
	{
	typedef HLogic::ptr_t ( *creator_t )( yaal::hcore::HString const& );
public:
	struct OCreator
		{
		OCreator( void ) : f_oInfo(), CREATOR( NULL ) {}
		yaal::hcore::HString f_oInfo;
		creator_t CREATOR;
		};
	typedef yaal::hcore::HMap<yaal::hcore::HString, OCreator> creators_t;
private:
	creators_t f_oCreators;
public:
	void register_logic_creator( yaal::hcore::HString const&, creator_t );
	HLogic::ptr_t create_logic( yaal::hcore::HString const&, yaal::hcore::HString const& );
	bool is_type_valid( yaal::hcore::HString const& );
	creators_t::iterator begin( void );
	creators_t::iterator end( void );
private:
	HLogicFactory( void ) : f_oCreators() {}
	~HLogicFactory( void ) {}
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HLogicFactory>;
	friend class yaal::hcore::HDestructor<HLogicFactory>;
	};

typedef yaal::hcore::HSingleton<HLogicFactory> HLogicFactoryInstance;

}

#endif /* not __LOGICFACTORY_H */

