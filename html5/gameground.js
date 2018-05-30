/*let browser = Vue.component(
	"browser", {
		el: "#browser"
	}
)*/
let app = null

function on_load() {
	app = new Vue( {
		el: "#root",
		data: {
			sock: null,
			currentTab: "browser",
			tabs: ["browser"],
			players: []
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
				let self = this
				this.sock = new WebSocket( address )
				this.sock.onopen = function( event ) {
					let s = self.sock; self.sock = null; self.sock = s
					self.sock.send( "login:4:" + login + ":" + sha1( password ) )
					self.sock.send( "get_players" )
					self.sock.send( "get_logics" )
					self.sock.send( "get_partys" )
				}
				this.sock.onerror = function( event ) {
					msg = "WebSocket connection error: " + event.type
					alert( msg );
					console.log( msg )
					self.do_disconnect()
				}
				this.sock.onmessage = function( event ) {
					let message = event.data.trim()
					console.log( "-> " + message )
					self.dispatch( message )
				}
				this.sock.onclose = function( event ) {
					self.do_disconnect()
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
				let sepIdx = message.indexOf( ":" )
				if ( sepIdx > 0 ) {
					let proto = message.substr( 0, sepIdx )
					let data = message.substr( sepIdx + 1 )
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
				let pad = document.querySelector( "#chat-view" )
				if ( pad ) {
					pad.appendChild( document.createTextNode( now() ) )
					pad.innerHTML = pad.innerHTML + ": " + colorize( message )
					pad.appendChild( document.createElement( "br" ) )
				}
			},
			on_player: function( message ) {
				let index = this.players.indexOf( message )
				if ( index === -1 ) {
					this.players.push( message )
					this.players.sort()
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
				let index = this.players.indexOf( message )
				if ( index !== -1 ) {
					this.players.splice( index, 1 )
				}
			},
			on_enter: function( event ) {
				let chatInput = document.getElementById( "chat-input" )
				if ( chatInput.value !== "" ) {
					app.sock.send( "msg:" + chatInput.value )
				}
				chatInput.value = ""
			}
		}
	} )
}

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
		let end = message.indexOf( ";", newIdx )
		let col = colorMap( message.substr( newIdx + 1, end - ( newIdx + 1 ) ) )
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
	let t = new Date()
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
	let f = document.forms["connect"]
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
		let f = document.forms["connect"]
		let address = "wss://" + f["server"].value + ":" + f["port"].value
		app.connect( address, f["login"].value, f["password"].value )
	} catch ( e ) {
		dump_exception( e )
	}
}

function dump_exception( e ) {
	console.log( e.name + ": " + e.message + " in " + e.fileName + ":" + e.lineNumber );
}

function post_load() {
	for ( let i in postLoadFunctions ) {
		postLoadFunctions[ i ]();
	}
}
let postLoadFunctions = [];

function register_post_load_function( func ) {
	postLoadFunctions.push( func );
}

(function() {
	register_post_load_function( on_load );
})();
