"use strict"

const _app_ = new Vue( {
	el: "#root",
	data: {
		sock: null,
		myLogin: null,
		registered: false,
		_messageBuffer: "",
		players: [],
		logics: [
			new Logic( Boggle ),
			new Logic( Galaxy ),
			new Logic( Go ),
			new Logic( Gomoku ),
			new Logic( SetBang )
		],
		partys: [ new Browser() ],
		selectedPartyLogic: null,
		currentTab: Browser.TAG,
		modal: null
	},
	methods: {
		/* helpers */
		logic_by_type: function( type_ ) {
			const idx = this.logics.findIndex( l => l.TAG == type_ )
			return ( idx >= 0 ? this.logics[idx] : null )
		},
		party_by_id: function( id_, active_ = false ) {
			const idx = this.partys.findIndex( p => p._id == id_ )
			let party = null
			if ( idx >= 0 ) {
				party = this.partys[idx]
			} else if ( ! active_ ) {
				for ( let logic of this.logics ) {
					party = logic.party_by_id( id_ )
					if ( party != null ) {
						break
					}
				}
			}
			return ( party )
		},
		make_visible: function( id_ ) {
			let idx = this.partys.findIndex( p => p._id == id_ )
			if ( idx < 0 ) {
				this.add_party( this.party_by_id( id_ ) )
				idx = this.partys.length - 1
			}
			this.partys[idx].visit()
			this.currentTab = id_
		},
		add_party: function( party_ ) {
			this.partys.push( party_ )
		},

		/* socket */
		is_connected: function() {
			return ( ( this.sock != null ) && ( this.sock.readyState === 1 ) )
		},
		connect: function( address, login, password ) {
			this.sock = new WebSocket( address )
			this.sock.onopen = event => {
				const s = this.sock; this.sock = null; this.sock = s
				this.sock.send( "login:4:" + login + ":" + sha1( password ) )
				this.myLogin = login
				this._messageBuffer = ""
				this.registered = password !== ""
				this.sock.send( "get_players" )
				this.sock.send( "get_logics" )
				this.sock.send( "get_partys" )
				document.getElementById( "snd-login" ).play()
			}
			this.sock.onerror = event => {
				const msg = "WebSocket connection error: " + event.type
				this.sock.onclose = null
				this.show_modal( msg, () => {
					console.log( msg )
					this.do_disconnect()
				} )
			}
			this.sock.onmessage = event => {
				this._messageBuffer += event.data
				while ( true ) {
					const eol = this._messageBuffer.indexOf( "\n" )
					if ( eol >= 0 ) {
						const message = this._messageBuffer.substr( 0, eol )
						this._messageBuffer = this._messageBuffer.substr( eol + 1 )
						console.log( "-> " + message )
						this.dispatch( message )
					} else {
						break
					}
				}
			}
			this.sock.onclose = event => {
				this.do_disconnect()
			}
			console.log( "After connection attempt..." )
		},
		do_disconnect: function() {
			document.getElementById( "snd-logout" ).play()
			this.modal = null
			this.myLogin = null
			this.registered = false
			this.selectedPartyLogic = null
			this.players.clear()
			this.make_visible( Browser.TAG )
			for ( const logic of this.logics ) {
				logic.drop_partys()
			}
			while ( this.partys.length > 1 ) {
				const party = this.partys.pop()
				this.close_party( party._id )
			}
			if ( this.sock !== null ) {
				this.sock.close()
			}
			this.sock = null
		},
		dispatch: function( message ) {
			const sepIdx = message.indexOf( ":" )
			if ( sepIdx > 0 ) {
				const proto = message.substr( 0, sepIdx )
				const data = message.substr( sepIdx + 1 )
				switch ( proto ) {
					case "say":
					case "msg": { this.on_msg( data ) } break
					case "err": { this.on_err( data ) } break
					case "player": { this.on_player( data ) } break
					case "logic": { this.on_logic( data ) } break
					case "party_info": { this.on_party_info( data ) } break
					case "party_close": { this.on_party_close( data ) } break
					case "player_quit": { this.on_player_quit( data ) } break
					case "party": { this.on_party( data ) } break
					case "account": { this.on_account( data ) } break
					default: console.log( message )
				}
			} else {
				console.log( message )
			}
		},

		/* gg protocol & logic */
		on_err: function( message ) {
			this.show_modal(
				"The GameGround server reported an error condition:\n" + message,
				() => this.do_disconnect()
			)
		},
		on_msg: function( message ) {
			const pad = document.querySelector( "#chat-view" )
			if ( pad ) {
				pad.log_message( message )
				if ( this.currentTab != "browser" ) {
					this.party_by_id( "browser" ).notify()
				}
			}
		},
		close_party: function( id_ ) {
			if ( id_ == "browser" ) {
				return
			}
			const idx = this.partys.findIndex( p => p._id == id_ )
			if ( idx < 0 ) {
				return
			}
			this.partys.plop( idx ).close()
			this.make_visible( this.partys[ Math.min( idx, this.partys.length - 1 ) ]._id )
		},
		on_player: function( message ) {
			const player = message.split( "," )
			const login = player[0]
			const index = this.players.findIndex( x => login == x.login )
			if ( index === -1 ) {
				this.players.push( new Player( login ) )
				this.players.sort( ( l, r ) => l.login.localeCompare( r.login ) )
			} else {
				for ( let p of this.partys ) {
					p.drop_player( login )
				}
			}
			player.plop( 0 )
			for ( let id of player ) {
				const party = this.party_by_id( id )
				if ( party != null ) {
					party.add_player( login )
					if ( login == this.myLogin ) {
						this.make_visible( id )
					}
				}
			}
		},
		on_logic: function( message ) {
			const defaults = message.chop( ":", 2 )
			const logic = this.logic_by_type( defaults[0] )
			if ( logic != null ) {
				logic._defaults = defaults[1]
			}
		},
		on_party_info: function( message ) {
			const conf = message.chop( ",", 4 )
			const logic = this.logic_by_type( conf[1] )
			if ( logic != null ) {
				const party = logic.create( this, conf[0], conf[2], conf[3] )
				logic.add_party( party )
			} else if ( conf[1] == Chat.TAG ) {
				Chat.update( this, conf[0], conf[2] )
			}
		},
		on_party_close: function( message ) {
			for ( const logic of this.logics ) {
				logic.drop_party( message )
			}
		},
		on_party: function( message ) {
			const msgData = message.chop( ",", 2 )
			const id = msgData[0]
			const party = this.party_by_id( id )
			if ( party == null ) {
				return
			}
			if ( id != this.currentTab ) {
				party.notify()
			}
			party.invoke( msgData[1] )
		},
		on_account: function( message ) {
			const tokens = message.chop( ",", 2 )
			const login = tokens[0]
			const info = tokens[1]
			this.add_party( new Chat( this, login, login, info ) )
			this.make_visible( login )
		},
		on_player_quit: function( message ) {
			const index = this.players.findIndex( x => message == x.login )
			if ( index !== -1 ) {
				this.players.splice( index, 1 )
			}
		},
		show_modal: function( message, resolve ) {
			this.modal = new MessageBox( this, message ).then( resolve )
		}
	}
} )

function colorMap( color ) {
	color = color | 0
	switch ( color ) {
		case   0: { color = "black" }         break
		case   1: { color = "red" }           break
		case   2: { color = "green" }         break
		case   3: { color = "brown" }         break
		case   4: { color = "blue" }          break
		case   5: { color = "magenta" }       break
		case   6: { color = "cyan" }          break
		case   7: { color = "lightgray" }     break
		case   8: { color = "darkgray" }      break
		case   9: { color = "brightred" }     break
		case  10: { color = "brightgreen" }   break
		case  11: { color = "yellow" }        break
		case  12: { color = "brightblue" }    break
		case  13: { color = "brightmagenta" } break
		case  14: { color = "brightcyan" }    break
		case  15: { color = "white" }         break
		case 256: { color = "black" }         break
		default:  { color = "black" }         break
	}
	return ( color )
}

function colorize( message ) {
	let m = ""
	let idx = 0
	let needClosing = false
	while ( true ) {
		let newIdx = message.substr( idx ).search( /\$\d+;/ )
		if ( newIdx == -1 ) {
			m += message.substr( idx )
			if ( needClosing ) {
				m += "</span>"
			}
			break
		}
		newIdx += idx
		m += message.substr( idx, newIdx - idx )
		const end = message.indexOf( ";", newIdx )
		const col = colorMap( message.substr( newIdx + 1, end - ( newIdx + 1 ) ) )
//		console.log( "len = " + message.length + ", idx = " + idx + ", end = " + end + ", newIdx = " + newIdx + ", col = " + col )
		if ( needClosing ) {
			m += "</span>"
		}
		needClosing = true
		m += "<span style=\"color: " + col + "\">"
		idx = end + 1
	}
	return (
		m
			.replace( /\\K/g, "," )
			.replace( /\\C/g, ":" )
			.replace( /\\A/g, "'" )
			.replace( /\\Q/g, "\"" )
			.replace( /\\n/g, "<br />" )
			.replace( /\n/g, "<br />" )
			.replace( /\\E/g, "\\" )
	)
}

function dump_exception( e ) {
	console.log( e.name + ": " + e.message + " in " + e.fileName + ":" + e.lineNumber )
}

function ensure( condition_, message_ ) {
	if ( ! condition_() ) {
		_app_.show_modal( message_ + "\n" + condition_.toString() )
		_app_.modal._title = "Failed Assertion!"
	}
}

function post_load() {
	for ( let i in postLoadFunctions ) {
		postLoadFunctions[ i ]()
	}
}
let postLoadFunctions = []

function register_post_load_function( func ) {
	postLoadFunctions.push( func )
}

(function() {
//	register_post_load_function( on_load )
})()

Array.prototype.clear = function() {
	this.splice( 0 )
}

Array.prototype.plop = function( idx_ ) {
	return ( this.splice( idx_, 1 )[0] )
}

String.prototype.chop = function( by, limit = null ) {
	let res = []
	let from = 0
	let i = 1
	while ( ( limit == null ) || ( i < limit ) ) {
		const idx = this.indexOf( by, from )
		if ( idx >= 0 ) {
			res.push( this.substr( from, idx - from ) )
			from = idx + 1
		} else {
			res.push( this.substr( from ) )
			from = this.length
			break
		}
		i += 1
	}
	if ( from < this.length ) {
		res.push( this.substr( from ) )
	}
	return ( res )
}

Date.prototype.iso8601 = function() {
	return (
		this.getFullYear().toString()
			+ "-" + ( this.getMonth() + 1 ).toString().padStart( 2, "0" )
			+ "-" + this.getDate().toString().padStart( 2, "0" )
			+ " " + this.getHours().toString().padStart( 2, "0" )
			+ ":" + this.getMinutes().toString().padStart( 2, "0" )
			+ ":" + this.getSeconds().toString().padStart( 2, "0" )
	)
}

Node.prototype.removeAllChildren = function() {
	while ( this.firstChild ) {
		this.removeChild( this.firstChild )
	}
}

HTMLDivElement.prototype.log_message = function( message_ ) {
	const scrollPos = this.scrollHeight - this.clientHeight
	const wantScroll = this.scrollTop === scrollPos
	this.appendChild( document.createTextNode( new Date().iso8601() ) )
	this.append_text( ": " + message_ )
	if ( wantScroll ) {
		this.scrollTop = this.scrollHeight
	}
}

HTMLDivElement.prototype.append_text = function( message_ ) {
	const scrollPos = this.scrollHeight - this.clientHeight
	const wantScroll = this.scrollTop === scrollPos
	this.innerHTML += colorize( message_ )
	this.appendChild( document.createElement( "br" ) )
	if ( wantScroll ) {
		this.scrollTop = this.scrollHeight
	}
}

