"use strict"

class Boggle extends Party {
	static get TAG() { return "bgl" }
	static get NAME() { return "Boggle" }
	static create( app_, id_, name_, configuration_ ) {
		return ( new Boggle( app_, id_, name_, configuration_ ) )
	}
	constructor( app_, id_, name_, configuration_ ) {
		super( app_, id_, name_, configuration_ )
		this._handlers["say"] = ( msg ) => this.on_say( msg )
		this._handlers["player_quit"] = function(){}
	}
	on_say( message_ ) {
		this._refs.messages.log_message( message_ )
	}
}

Vue.component(
	"bgl", {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		mounted: function() {
			this.data._refs = this.$refs
		},
		methods: {
			on_enter: function( event ) {
				const source = event.target || event.srcElement
				const msg = source.value
				if ( msg.match( /.*\S+.*/ ) != null ) {
					this.$parent.sock.send( "cmd:" + this.data._id + ":say:" + msg )
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

