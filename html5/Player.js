"use strict"

class Player {
	constructor( login_ ) {
		this._login = login_
	}
	get login() {
		return ( this._login )
	}
}

