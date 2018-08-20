"use strict"

class Chat extends Party {
	static get TAG() { return "chat" }
	static get NAME() { return "Chat" }
	static update( app_, id_, name_ ) {
		const names = name_.split( ":" )
		const name = names[0] === app_.myLogin ? names[1] : names[0]
		const party = app_.party_by_id( name )
		if ( party == null ) {
			return
		}
		party.set_id( id_ )
	}
	constructor( app_, id_, name_, configuration_ ) {
		super( app_, id_, name_ )
		this._online = false
		this._lineBuffer = []
		this._aboutToCreate = false
		this._handlers["say"] = ( msg ) => this.on_say( msg )
		this._handlers["player_quit"] = function(){}
		this._configuration = configuration_
	}
	get name() {
		return ( this._name )
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
			m.append_text( "User: " + this.data._name + "\n" )
			m.append_text( "Full name: " + info[0] + "\n" )
			m.append_text( "Description:\n" + info[1] + "\n" )
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
			<input type="text" name="input" maxlength="1024" title="Send message to all people in this private chat room." v-on:keypress.enter="on_enter">
		</div>
`
	}
)

