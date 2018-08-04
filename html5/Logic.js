
"use strict"

class Logic {
	constructor( class_ ) {
		this._class = class_
		this._defaults = null
		this._partys = []
		this._expanded = false
	}
	add_party( party_ ) {
		this._partys.push( party_ )
		this._partys.sort( ( l, r ) => l._name.localeCompare( r._name ) )
	}
	get name() {
		return ( this._class.NAME )
	}
	get TAG() {
		return ( this._class.TAG )
	}
	drop_party( id_ ) {
		const idx = this._partys.findIndex( p => p._id == id_ )
		if ( idx >= 0 ) {
			this._partys.plop( idx )
		}
	}
	party_by_id( id_ ) {
		const idx = this._partys.findIndex( p => p._id == id_ )
		return ( idx >= 0 ? this._partys[idx] : null )
	}
	drop_partys() {
		this._partys.clear()
	}
	create( app_, id_, name_, configuration_ ) {
		return ( this._class.create( app_, id_, name_, configuration_ ) )
	}
}

