"use strict"

class Party {
	constructor( app_, id_, name_, configuration_ ) {
		this._id = id_
		this._name = name_
		this._configuration = configuration_
		this._players = []
		this._handlers = {}
		this._app = app_
	}
	get name() {
		return ( this._name + ":" + this._id )
	}
	close() {
		this._app.sock.send( "abandon:" + this._id )
		this._app = null
	}
	add_player( player_ ) {
		const idx = this._players.indexOf( player_ );
		if ( idx >= 0 ) {
			return
		}
		this._players.push( player_ )
		this._players.sort()
	}
	drop_player( player_ ) {
		const idx = this._players.indexOf( player_ );
		if ( idx >= 0 ) {
			this._players.plop( idx )
		}
	}
	invoke( message_ ) {
		const message = message_.chop( ":", 2 )
		const handler = this._handlers[message[0]]
		if ( handler !== undefined ) {
			handler( message[1] )
		}
	}
}

