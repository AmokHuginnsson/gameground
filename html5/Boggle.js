"use strict"

class Boggle extends Party {
	static get TAG() { return "bgl" }
	static get NAME() { return "Boggle" }
	static create( app_, id_, name_, configuration_ ) {
		return ( new Boggle( app_, id_, name_, configuration_ ) )
	}
	constructor( app_, id_, name_, configuration_ ) {
		super( app_, id_, name_, configuration_ )
		this._handlers["player_quit"] = function(){}
		this._handlers["deck"] = ( msg ) => this.on_deck( msg )
		this._handlers["longest"] = ( msg ) => this._refs.longest.append_text( msg )
		this._handlers["scored"] = ( msg ) => this._refs.scored.append_text( msg )
	}
	on_deck( data_ ) {
		data_ = data_.toUpperCase()
		for ( let i = 0; i < 16; ++ i ) {
			this._refs.cube[i].textContent = data_[i]
		}
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
			on_msg_enter: function( event ) {
				const source = event.target || event.srcElement
				const msg = source.value
				if ( msg.match( /.*\S+.*/ ) != null ) {
					this.$parent.sock.send( "cmd:" + this.data._id + ":say:" + msg )
				}
				source.value = ""
			},
			on_bgl_enter: function( event ) {
				const source = event.target || event.srcElement
				const msg = source.value
				if ( msg.match( /.*\S+.*/ ) != null ) {
					this.$parent.sock.send( "cmd:" + this.data._id + ":play:" + msg )
					this.data._refs.sent.append_text( msg )
				}
				source.value = ""
			}
		},
		template: `
		<div class="tab-pane">
			<div class="hbox">
				<div class="boggle-pane">
					<div class="boggle">
						<div class="cubes">
							<div v-for="c in 'GAMEGROUNDBOGGLE'" ref="cube">{{c}}</div>
						</div>
					</div>
					<div class="box" style="grid-area: sent;">
						<label>Words sent...</label>
						<div ref="sent" class="messages" title="The word that you send to GameGround server."></div>
					</div>
					<div class="box" style="grid-area: scored;">
						<label>Words that scored...</label>
						<div ref="scored" class="messages" title="Your words that both valid and unique."></div>
					</div>
					<div class="box" style="grid-area: longest;">
						<label>Longest words...</label>
						<div ref="longest" class="messages" title="Longest words found by all players in last round."></div>
					</div>
					<div style="grid-area: input;">
						<label>Enter your words here ...</label><br />
						<input class="long-input" type="text" name="boggle-input" maxlength="1024" title="Here you shall enter words that you have found." v-on:keypress.enter="on_bgl_enter">
					</div>
				</div>
				<div class="boggle-chat">
					<label>Private chat messages</label>
					<div class="messages" ref="messages"></div>
					<label>Type your message</label><br />
					<input class="long-input" type="text" name="input" maxlength="1024" title="Enter message You want to send to other players." v-on:keypress.enter="on_msg_enter">
				</div>
			</div>
		</div>
`
	}
)

