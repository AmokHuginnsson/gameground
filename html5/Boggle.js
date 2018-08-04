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
					this.$parent.sock.send( "cmd:" + this.data._id + ":say:" + msg )
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
							<div ref="cube0">G</div>
							<div ref="cube1">A</div>
							<div ref="cube2">M</div>
							<div ref="cube3">E</div>
							<div ref="cube4">G</div>
							<div ref="cube5">R</div>
							<div ref="cube6">O</div>
							<div ref="cube7">U</div>
							<div ref="cube8">N</div>
							<div ref="cube9">D</div>
							<div ref="cube10">B</div>
							<div ref="cube11">O</div>
							<div ref="cube12">G</div>
							<div ref="cube13">G</div>
							<div ref="cube14">L</div>
							<div ref="cube15">E</div>
						</div>
					</div>
					<div class="box" style="grid-area: sent;">
						<label>Words sent...</label>
						<ul class="listwidget" title="The word that you send to GameGround server."></ul>
					</div>
					<div class="box" style="grid-area: scored;">
						<label>Words that scored...</label>
						<ul class="listwidget" title="Your words that both valid and unique."></ul>
					</div>
					<div class="box" style="grid-area: longest;">
						<label>Longest words...</label>
						<ul class="listwidget" title="Longest words found by all players in last round."></ul>
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

