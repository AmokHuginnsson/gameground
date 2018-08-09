"use strict"

Vue.component(
	"login", {
		data: function( arg ) {
			const o = {
				login: "",
				password: "",
				server: "",
				port: 7777
			}
			return ( o )
		},
		mounted: function() {
			if ( this.is_networked() ) {
				this.server = window.location.hostname
			} else {
				this.server = "127.0.0.1"
			}
		},
		methods: {
			is_networked: function() {
				const proto = window.location.protocol
				return ( ( proto == "https:" ) || ( proto == "http:" ) )
			},
			show_net: function() {
				const urlParams = new URLSearchParams( window.location.search );
				const showNet = ! this.is_networked() || urlParams.has( "verbose" )
				return ( showNet )
			},
			enable_net: function() {
				const urlParams = new URLSearchParams( window.location.search );
				const enableNet = this.show_net() && urlParams.has( "debug" )
				return ( enableNet )
			},
			on_connect: function() {
				let errMsg = "Your setup containg following errors:\n"
				if ( this.login == "" ) {
					errMsg += "name not set\n"
				} else if ( this.login.match( /^[a-z][a-z0-9_]+$/i ) == null ) {
					errMsg += ( "Invalid login: " + this.login )
				} else if ( this.server == "" ) {
					errMsg += "server not set\n"
				} else if ( isNaN( this.port ) ) {
					errMsg += "invalid port specified"
				} else if ( this.port <= 1024 ) {
					errMsg += "invalid port number (must be over 1024)\n"
				} else {
					const address = "wss://" + this.server + ":" + this.port
					try {
						_app_.connect( address, this.login, this.password )
						return
					} catch ( e ) {
						dump_exception( e )
						errMsg = e.message
					}
				}
				_app_.show_modal( errMsg )
			}
		},
		template: `
		<div id="connect">
			<div id="logo" style="height: 140px;background: url(gameground.svg) no-repeat center center;"></div>
			<form name="connect" @submit.prevent="on_connect">
				<div id="connect-info">
					<label>Your name:</label>
					<input v-model.trim="login" name="login" autocomplete="login" type="text" title="Your GameGround world name (required)." />
					<label>Password:</label>
					<input v-model="password" name="password" type="password" title="Your password (optional)." />
					<label v-show="show_net()">Server:</label>
					<input v-show="show_net()" v-model.trim="server" name="server" type="text" title="Address of the server that hosts the game (required)." :disabled="!enable_net()" />
					<label v-show="show_net()">Port:</label>
					<input v-show="show_net()" v-model.number="port" name="port" type="number" min="1025" title="Port that game server is bound to (optional)." :disabled="!enable_net()" />
				</div>
				<div id="connect-buttons">
					<button title="Click me to join the GameGround server.">Connect!</button>
					<button title="go to codestation.org" onclick="JavaScript:window.location.href='https://codestation.org/'">Exit!</button>
				</div>
			</form>
		</div>
`
	}
)

