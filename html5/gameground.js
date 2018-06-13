"use strict"

class Player {
	constructor( login_ ) {
		if ( login_.match( /^[a-z]+$/i ) == null ) {
			throw Error( "Invalid login: " + login_ )
		}
		this._login = login_
	}
	get login() {
		return ( this._login )
	}
}

class Party {
	constructor( id_, name_, configuration_ ) {
		this._id = id_
		this._name = name_
		this._configuration = configuration_
		this._players = []
		this._logic = null
	}
}

class Logic {
}

class Chat extends Logic {
}

class Boggle extends Logic {
}

class Go extends Logic {
}

class Gomoku extends Logic {
}

class SetBang extends Logic {
}

class Galaxy extends Logic {
}

/*let browser = Vue.component(
	"browser", {
		el: "#browser"
	}
)*/
const _app_ = new Vue( {
	el: "#root",
	data: {
		sock: null,
		_messageBuffer: "",
		currentTab: "browser",
		tabs: ["browser"],
		players: [],
		partys: []
	},
	computed: {
		currentTabComponent: function () {
			return 'tab-' + this.currentTab.toLowerCase()
		}
	},
	methods: {
		is_connected: function() {
			return ( ( this.sock != null ) && ( this.sock.readyState === 1 ) )
		},
		connect: function( address, login, password ) {
			this.sock = new WebSocket( address )
			this.sock.onopen = event => {
				const s = this.sock; this.sock = null; this.sock = s
				this.sock.send( "login:4:" + login + ":" + sha1( password ) )
				this.sock.send( "get_players" )
				this.sock.send( "get_logics" )
				this.sock.send( "get_partys" )
			}
			this.sock.onerror = event => {
				msg = "WebSocket connection error: " + event.type
				alert( msg );
				console.log( msg )
				this.do_disconnect()
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
					case "player_quit": { this.on_player_quit( data ) } break
					case "party": { this.on_party( data ) } break
					default: console.log( message )
				}
			} else {
				console.log( message )
			}
		},
		on_err: function( message ) {
			alert( "The GameGround server reported an error condition:\n" + message )
			this.do_disconnect()
		},
		on_msg: function( message ) {
			const pad = document.querySelector( "#chat-view" )
			if ( pad ) {
				pad.appendChild( document.createTextNode( now() ) )
				pad.innerHTML = pad.innerHTML + ": " + colorize( message )
				pad.appendChild( document.createElement( "br" ) )
			}
		},
		on_player: function( message ) {
			const player = message.split( "," )
			if ( player.length > 1 ) {
				const index = this.players.findIndex( x => player[0] == x.login )
				if ( index === -1 ) {
					this.players.push( new Player( player[0] ) )
					this.players.sort( ( l, r ) => l.login.localeCompare( r.login ) )
				}
			}
		},
		on_logic: function( message ) {
			console.log( message )
		},
		on_party_info: function( message ) {
			console.log( message )
		},
		on_party: function( message ) {
			console.log( message )
		},
		on_player_quit: function( message ) {
			const index = this.players.findIndex( x => message == x.login )
			if ( index !== -1 ) {
				this.players.splice( index, 1 )
			}
		},
		on_enter: function( event ) {
			const chatInput = document.getElementById( "chat-input" )
			if ( chatInput.value !== "" ) {
				this.sock.send( "msg:" + chatInput.value )
			}
			chatInput.value = ""
		}
	}
} )

function colorMap( color ) {
	switch ( color ) {
		case "0": { color = "black" } break
		case "1": { color = "red" } break
		case "2": { color = "green" } break
		case "3": { color = "brown" } break
		case "4": { color = "blue" } break
		case "5": { color = "magenta" } break
		case "6": { color = "cyan" } break
		case "7": { color = "lightgray" } break
		case "8": { color = "darkgray" } break
		case "9": { color = "brightred" } break
		case "10": { color = "brightgreen" } break
		case "11": { color = "yellow" } break
		case "12": { color = "brightblue" } break
		case "13": { color = "brightmagenta" } break
		case "14": { color = "brightcyan" } break
		case "15": { color = "white" } break
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
			break;
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
	return ( m )
}

function now() {
	const t = new Date()
	return (
		t.getFullYear().toString()
			+ "-" + ( t.getMonth() + 1 ).toString().padStart( 2, "0" )
			+ "-" + t.getDate().toString().padStart( 2, "0" )
			+ " " + t.getHours().toString().padStart( 2, "0" )
			+ ":" + t.getMinutes().toString().padStart( 2, "0" )
			+ ":" + t.getSeconds().toString().padStart( 2, "0" )
	)
}

function on_connect_click() {
	const f = document.forms["connect"]
	let errMsg = "Your setup containg following errors:\n"
	let setupOk = true
	if ( f["login"].value == "" ) {
		errMsg += "name not set\n"
		setupOk = false
	}
	if ( f["server"].value == "" ) {
		errMsg += "server not set\n"
		server = false
	}
	if ( setupOk ) {
		do_connect()
	} else {
		alert( errMsg )
	}
}

function do_connect() {
	try {
		const f = document.forms["connect"]
		const address = "wss://" + f["server"].value + ":" + f["port"].value
		_app_.connect( address, f["login"].value, f["password"].value )
	} catch ( e ) {
		dump_exception( e )
	}
}

function dump_exception( e ) {
	console.log( e.name + ": " + e.message + " in " + e.fileName + ":" + e.lineNumber )
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
})();
