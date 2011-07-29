/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	logicfactory.hxx - this file is integral part of `gameground' project.

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
		OCreator( void ) : _info(), _instatiator( NULL ) {}
		OCreator( OCreator const& c ) : _info( c._info ), _instatiator( c._instatiator ) {}
		OCreator& operator = ( OCreator const& c ) { _info = c._info; _instatiator = c._instatiator; return ( *this ); }
		yaal::hcore::HString _info;
		HLogicCreatorInterface* _instatiator;
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

