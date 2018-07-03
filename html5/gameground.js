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

class Logic {
	constructor( class_ ) {
		this._class = class_
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
	drop_party( id_ ) {
		const idx = this._partys.findIndex( p => p._id == id_ );
		if ( idx >= 0 ) {
			this._partys.plop( idx )
		}
	}
	drop_partys() {
		this._partys.clear()
	}
	create( app_, id_, name_, configuration_ ) {
		return ( this._class.create( app_, id_, name_, configuration_ ) )
	}
}

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
		return ( this._name )
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

class Browser extends Party {
	static get TAG() { return "browser" }
	constructor() {
		super( null, "browser", "Browser", "" )
	}
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
			on_player_dblclick: function( login ) {
				if ( login == this.$parent.myLogin ) {
					return
				}
				let id = null
				for ( let p of this.$parent.partys ) {
					if ( ( p.TAG == Chat.TAG ) && ( p._name == login ) ) {
						id = p._id
						break
					}
				}
				if ( id == null ) {
					this.$parent.sock.send( "get_account:" + login )
				} else {
					this.$parent.make_visible( id )
				}
			},
			on_logic_dblclick: function( data ) {
				data._expanded = ! data._expanded
			},
			on_logic_context: function( data ) {
				alert( data )
			},
			on_join: function() {
			},
			on_create: function() {
				this.$parent.show_modal( "Create Game" )
			},
			on_account: function() {
			}
		},
		template: `
			<div id="browser" class="tab-pane">
				<label>Server messages</label>
				<label>Games</label>
				<label style="grid-column: 4">People</label>
				<div id="chat-view" class="messages"></div>
				<ul id="games" class="treewidget">
					<li
						class="noselect"
						v-for="logic in $parent.logics"
						v-bind:key="logic._class.TAG"
						v-bind:class="[{ empty: logic._partys.length === 0 }, { expanded: logic._partys.length > 0 && logic._expanded }, { collapsed: logic._partys.length > 0 && ! logic._expanded }]"
						v-on:dblclick="on_logic_dblclick( logic )"
						v-on:click.right="on_logic_context( logic )"
					>{{ logic.name }}
						<ul class="treebranch">
							<li
								class="noselect"
								v-for="party in logic._partys"
								v-bind:key="party"
							>{{ party }}
							</li>
						</ul>
					</li>
				</ul>
				<ul id="players" class="listwidget">
					<li
						class="noselect"
						v-for="player in $parent.players"
						v-on:dblclick="on_player_dblclick( player.login )"
					>{{ player.login }}</li>
				</ul>
				<button id="btn-join" v-on:click="on_join" title="Click me to join pre-existing game." disabled="disabled">Join</button>
				<button id="btn-create" v-on:click="on_create" title="Click me to create a new game.">Create</button>
				<button id="btn-account" v-on:click="on_account" title="Click me to edit your account information." disabled="disabled">Account</button>
				<button id="btn-disconnect" v-on:click="$parent.do_disconnect" title="Click me to disconnet from GameGround server.">Disconnect</button>
				<div id="chat-input-field">
					<label>Type your message</label><br />
					<input id="chat-input" type="text" name="input" maxlength="1024" title="Send message to all people on the server that are currently not in the game." v-on:keypress.enter="on_enter">
				</div>
			</div>
`
	}
)

class Chat extends Party {
	static get TAG() { return "chat" }
	static get NAME() { return "Chat" }
	static update( app_, id_, name_, configuration_ ) {
		const names = name_.split( ":" )
		const name = names[0] === app_.myLogin ? names[1] : names[0]
		const party = app_.party_by_id( name )
		if ( party == null ) {
			return
		}
		party.set_id( id_ )
	}
	constructor( app_, id_, name_, configuration_ ) {
		super( app_, id_, name_, configuration_ )
		this._online = false
		this._lineBuffer = []
		this._aboutToCreate = false
		this._handlers["say"] = ( msg ) => this.on_say( msg )
		this._handlers["player_quit"] = function(){}
	}
	close() {
		if ( this._online ) {
			super.close()
		}
	}
	on_say( message_ ) {
		this._refs.messages.log_message( message_ )
	}
	set_id( id_ ) {
		this._id = id_
		this._app.make_visible( this._id )
		this._online = true
		if ( ! this._aboutToCreate ) {
			document.getElementById( "snd-call" ).play()
		}
		this._aboutToCreate = false
		for ( let line of this._lineBuffer ) {
			this._app.sock.send( "cmd:" + this._id + ":say:" + line )
		}
		this._lineBuffer.clear()
	}
}

Vue.component(
	"chat", {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		mounted: function() {
			const info = this.data._configuration.split( "," )
			const m = this.$refs.messages
			this.data._refs = this.$refs
			m.append_text( "User: " + this.data._name )
			m.append_text( "Full name: " + info[0] )
			m.append_text( "Description:\n" + info[1] )
		},
		methods: {
			on_enter: function( event ) {
				const source = event.target || event.srcElement
				const msg = source.value
				if ( msg.match( /.*\S+.*/ ) != null ) {
					if ( this.data._online ) {
						this.$parent.sock.send( "cmd:" + this.data._id + ":say:" + msg )
					} else {
						if ( this.data._lineBuffer.length === 0 ) {
							this.data._aboutToCreate = true
							this.$parent.sock.send( "create:chat:" + this.data._name + "," + this.$parent.myLogin )
						}
						this.data._lineBuffer.push( msg )
					}
				}
				source.value = ""
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

class Boggle extends Party {
	static get TAG() { return "bgl" }
	static get NAME() { return "Boggle" }
}

class Go extends Party {
	static get TAG() { return "go" }
	static get NAME() { return "Go" }
}

class Gomoku extends Party {
	static get TAG() { return "gomoku" }
	static get NAME() { return "Gomoku" }
}

class SetBang extends Party {
	static get TAG() { return "set_bang" }
	static get NAME() { return "Set!" }
}

class Galaxy extends Party {
	static get TAG() { return "glx" }
	static get NAME() { return "Galaxy" }
}

const _app_ = new Vue( {
	el: "#root",
	data: {
		sock: null,
		myLogin: null,
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
		currentTab: "browser",
		modal: null
	},
	methods: {
		logic_by_type: function( type_ ) {
			const idx = this.logics.findIndex( l => l.TAG == type_ );
			return ( idx >= 0 ? this.logics[idx] : null )
		},
		party_by_id: function( id_ ) {
			const idx = this.partys.findIndex( p => p._id == id_ );
			return ( idx >= 0 ? this.partys[idx] : null )
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
			this.modal = null
			this.myLogin = null
			this.players.clear()
			this.make_visible( "browser" )
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
			this.show_modal(
				"The GameGround server reported an error condition:\n" + message,
				() => this.do_disconnect()
			)
		},
		on_msg: function( message ) {
			const pad = document.querySelector( "#chat-view" )
			if ( pad ) {
				pad.log_message( message )
			}
		},
		close_party: function( id_ ) {
			if ( id_ == "browser" ) {
				return
			}
			const idx = this.partys.findIndex( p => p._id == id_ );
			if ( idx < 0 ) {
				return
			}
			for ( const logic of this.logics ) {
				logic.drop_party( id_ )
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
				}
			}
		},
		on_logic: function( message ) {
			console.log( message )
		},
		on_party_info: function( message ) {
			const conf = message.split( ",", 4 )
			const logic = this.logic_by_type( conf[1] )
			if ( logic != null ) {
				const party = logic.create( this, conf[0], conf[2], conf[3] )
				this.add_party( party )
				logic.add_party( party )
			} else if ( conf[1] == Chat.TAG ) {
				Chat.update( this, conf[0], conf[2], conf[3] )
			}
		},
		on_party: function( message ) {
			const msgData = message.chop( ",", 2 )
			const party = this.party_by_id( msgData[0] )
			if ( party == null ) {
				return
			}
			party.invoke( msgData[1] )
		},
		on_account: function( message ) {
			const tokens = message.chop( ",", 2 )
			const login = tokens[0]
			const info = tokens[1]
			this.add_party( new Chat( this, login, login, info ) )
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

class MessageBox {
	static get TAG() { return ( "messagebox" ) }
	constructor( app_, message_, label_ = "OK", title_ = "alert!" ) {
		this._app = app_
		this._message = colorize( message_ )
		this._label = label_
		this._title = "GameGround - " + title_
		this._handlers = {
			"OK": () => this.on_ok()
		}
	}
	get message() {
		return ( this._message )
	}
	get label() {
		return ( this._label )
	}
	get title() {
		return ( this._title )
	}
	on_ok() {
		this.close()
	}
	then( callback_ ) {
		this._then = callback_
		return ( this )
	}
	on( label_, callback_ ) {
		this._handlers[label_] = callback_
		return ( this )
	}
	close() {
		this._app.modal = null
		if ( this._then != undefined ) {
			this._then()
		}
	}
}

Vue.component(
	"messagebox", {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		template: `
		<div class="modal">
			<div class="block"></div>
			<div class="messagebox">
				<div class="title">{{ data.title }}</div>
				<div class="content">
					<p v-html="data.message"></p>
					<button v-for="handler, label in data._handlers" v-on:click="handler()">{{ label }}</button>
				</div>
			</div>
		</div>
`
	}
)

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
		_app_.show_modal( errMsg )
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

HTMLDivElement.prototype.log_message = function( message_ ) {
	this.appendChild( document.createTextNode( new Date().iso8601() ) )
	this.append_text( ": " + message_ )
}

HTMLDivElement.prototype.append_text = function( message_ ) {
	this.innerHTML += colorize( message_ )
	this.appendChild( document.createElement( "br" ) )
}

