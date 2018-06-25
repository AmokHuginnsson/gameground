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
	constructor( type_, id_, name_, configuration_ ) {
		this._type = type_
		this._id = id_
		this._name = name_
		this._configuration = configuration_
		this._players = []
		this._logic = null
	}
	get name() {
		return ( this._name )
	}
}

class Logic {
}

class Browser extends Party {
	constructor() {
		super( "browser", "browser", "Browser", "" )
	}
}

class Chat extends Party {
	constructor( id_, name_, configuration_ ) {
		super( "chat", id_, name_, configuration_ )
	}
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

Vue.component(
	"browser", {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		methods: {
			on_enter: function( event ) {
				const source = event.target || event.srcElement
				if ( source.value !== "" ) {
					this.$parent.sock.send( "msg:" + source.value )
				}
				source.value = ""
			},
			on_player_dblclick: function( event ) {
				const source = event.target || event.srcElement
				const login = source.textContent
				if ( login == this.$parent.myLogin ) {
					return
				}
				let id = null
				for ( let p of this.$parent.partys ) {
					if ( ( p._type == "chat" ) && ( p._name == login ) ) {
						id = p._id
						break
					}
				}
				if ( id == null ) {
					this.$parent.sock.send( "get_account:" + login )
				} else {
					this.$parent.make_visible( id )
				}
			}
		},
		template: `
			<div id="browser" class="tab-pane">
				<label>Server messages</label>
				<label>Games</label>
				<label style="grid-column: 4">People</label>
				<div id="chat-view" class="messages"></div>
				<div id="games"></div>
				<ul id="players" class="listwidget">
					<li
						class="noselect"
						v-for="player in $parent.players"
						v-on:dblclick="on_player_dblclick"
					>{{ player.login }}</li>
				</ul>
				<button id="btn-join" title="Click me to join pre-existing game." disabled="disabled">Join</button>
				<button id="btn-create" title="Click me to create a new game.">Create</button>
				<button id="btn-account" title="Click me to edit your account information." disabled="disabled">Account</button>
				<button id="btn-disconnect" title="Click me to disconnet from GameGround server." v-on:click="$parent.do_disconnect">Disconnect</button>
				<div id="chat-input-field">
					<label>Type your message</label><br />
					<input id="chat-input" type="text" name="input" maxlength="1024" title="Send message to all people on the server that are currently not in the game." v-on:keypress.enter="on_enter">
				</div>
			</div>
`
	}
)

Vue.component(
	"chat", {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		mounted: function() {
			const info = this.data._configuration.split( "," )
			const m = this.$refs.messages
			m.append_text( "User: " + this.data._name )
			m.append_text( "Full name: " + info[0] )
			m.append_text( "Description:\n" + info[1] )
		},
		methods: {
			on_enter: function() {
			}
		},
		template: `
		<div class="tab-pane chat">
			<label>Private chat messages</label>
			<div class="messages" ref="messages"></div>
			<label>Type your message</label>
			<input class="chat-input" type="text" name="input" maxlength="1024" title="Send message to all people in this private chat room." v-on:keypress.enter="on_enter">
		</div>
`
	}
)

const _app_ = new Vue( {
	el: "#root",
	data: {
		sock: null,
		myLogin: null,
		_messageBuffer: "",
		players: [],
		partys: [ new Browser() ],
		currentTab: "browser"
	},
	methods: {
		party_by_id: function( id_ ) {
			let p = null
			for ( let e of this.partys ) {
				if ( e._id == id_ ) {
					p = e
					break
				}
			}
			return ( p )
		},
		add_party: function( party_ ) {
			this.partys.push( party_ )
			this.currentTab = party_._id
			this.make_visible( party_._id )
		},
		make_visible: function( id_ ) {
			this.currentTab = id_
		},
		is_connected: function() {
			return ( ( this.sock != null ) && ( this.sock.readyState === 1 ) )
		},
		connect: function( address, login, password ) {
			this.sock = new WebSocket( address )
			this.sock.onopen = event => {
				const s = this.sock; this.sock = null; this.sock = s
				this.sock.send( "login:4:" + login + ":" + sha1( password ) )
				this.myLogin = login
				this.sock.send( "get_players" )
				this.sock.send( "get_logics" )
				this.sock.send( "get_partys" )
			}
			this.sock.onerror = event => {
				const msg = "WebSocket connection error: " + event.type
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
			this.myLogin = null
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
					case "account": { this.on_account( data ) } break
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
				pad.log_message( message )
			}
		},
		on_player: function( message ) {
			const player = message.split( "," )
			const index = this.players.findIndex( x => player[0] == x.login )
			if ( index === -1 ) {
				this.players.push( new Player( player[0] ) )
				this.players.sort( ( l, r ) => l.login.localeCompare( r.login ) )
			}
			if ( player.length > 1 ) {
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
		on_account: function( message ) {
			const tokens = message.chop( ",", 2 )
			const login = tokens[0]
			const info = tokens[1]
			this.add_party( new Chat( login, login, info ) )
		},
		on_player_quit: function( message ) {
			const index = this.players.findIndex( x => message == x.login )
			if ( index !== -1 ) {
				this.players.splice( index, 1 )
			}
		},
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
	return ( m.replace( /\\K/g, "," ).replace( /\\C/g, ":" ).replace( /\\A/g, "'" ).replace( /\\Q/g, "\"" ).replace( /\\n/g, "<br />" ).replace( /\\E/g, "\\" ) )
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

HTMLDivElement.prototype.log_message = function( message_ ) {
	this.appendChild( document.createTextNode( new Date().iso8601() ) )
	this.append_text( ": " + message_ )
}

HTMLDivElement.prototype.append_text = function( message_ ) {
	this.innerHTML += colorize( message_ )
	this.appendChild( document.createElement( "br" ) )
}

