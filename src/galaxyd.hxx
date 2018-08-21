/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GALAXYD_HXX_INCLUDED
#define GALAXYD_HXX_INCLUDED

#include <yaal/hcore/harray.hxx>
#include <yaal/hcore/hlist.hxx>
#include <yaal/hcore/hmap.hxx>

#include "logic.hxx"

namespace gameground {

namespace galaxy {

class HSystem;
class HGalaxy;
class HFleet {
protected:
	/*{*/
	int _size;
	HClient* _emperor;
	int _arrivalTime;
	/*}*/
public:
	/*{*/
	/*}*/
protected:
	/*{*/
	/*}*/
	friend class HSystem;
	friend class HGalaxy;
};

class HGalaxy;
class HSystem {
protected:
	/*{*/
	typedef yaal::hcore::HList<HFleet> attackers_t;
	int _id;
	int _coordinateX;
	int _coordinateY;
	int _production;
	int _fleet;
	HClient* _emperor;
	HLogic const* _logic;
	attackers_t _attackers;
	/*}*/
public:
	/*{*/
	HSystem( HLogic const* );
	HSystem( HSystem const& );
	HSystem& operator = ( HSystem const& );
	void do_round( HGalaxy & );
	void swap( HSystem& );
	/*}*/
protected:
	friend class HGalaxy;
};

inline void swap( HSystem& a, HSystem& b )
	{ a.swap( b ); }

class HGalaxy : public HLogic {
public:
	typedef HGalaxy this_type;
	typedef HLogic base_type;
private:
	struct OEmperorInfo {
		int _color;
		int _systems;
		OEmperorInfo( void ) : _color( -1 ), _systems( -1 ) {}
	};
	typedef yaal::hcore::HMap<HClient*, OEmperorInfo> emperors_t;
	typedef yaal::hcore::HArray<HSystem> systems_t;
protected:
	/*{*/
	int _boardSize;
	int _neutralSystemCount;
	int _startupPlayers;
	int _round;
	int _ready;
	systems_t _systems;
	emperors_t _emperors;
	/*}*/
public:
	/*{*/
	HGalaxy( HServer*, id_t const&, yaal::hcore::HString const&, int, int, int );
	virtual ~HGalaxy( void );
	int get_color( HClient* );
	void mark_alive( HClient* );
	/*}*/
protected:
	/*{*/
	OEmperorInfo* get_emperor_info( HClient* );
	int assign_system( HClient* );
	virtual bool do_accept( HClient* );
	virtual void do_post_accept( HClient* );
	virtual void do_kick( HClient* );
	virtual yaal::hcore::HString do_get_info() const;
	void handler_message( HClient*, yaal::hcore::HString const& );
	void handler_play( HClient*, yaal::hcore::HString const& );
	void end_round( void );
	/*}*/
private:
	/*{*/
	HGalaxy( HGalaxy const& );
	HGalaxy& operator = ( HGalaxy const& );
	/*}*/
	friend class HServer;
};

}

}

#endif /* not GALAXYD_HXX_INCLUDED */

