"use strict"

class SetBang extends Party {
	static get TAG() { return "set_bang" }
	static get NAME() { return "Set!" }
	static create( app_, id_, name_, configuration_ ) {
		return ( new SetBang( app_, id_, name_, configuration_ ) )
	}
	constructor( app_, id_, name_, configuration_ ) {
		super( app_, id_, name_ )
		this._scores = []
		this._handlers["player_quit"] = function(){}
		//this._handlers["player"] = ( msg ) => this.on_player( msg )
		//this._handlers["setup"] = ( msg ) => this.on_setup( msg )
		this._currentSort = "player"
	  this._currentSortDir = "asc"
		this.input = ""
	}
}

Vue.component(
	SetBang.TAG, {
		props: ["data"],
		data: function( arg ) {
			this.data._refs = this.$refs
			return ( this.data )
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
		},
		computed: {
			scores: function() {
				return (
					this.data._scores.sort( ( a, b ) => {
						let modifier = 1
						if ( this.data._currentSortDir === "desc" ) {
							modifier = -1
						}
						if ( a[this.data._currentSort] < b[this.data._currentSort] ) {
							return ( -1 * modifier )
						}
						if ( a[this.data._currentSort] > b[this.data._currentSort] ) {
							return ( 1 * modifier )
						}
						return ( 0 )
					})
				)
			}
		},
		template: `
		<div class="tab-pane hbox setbang-pane">
			<div class="setbang-pane-left">
			</div>
			<div class="setbang-chat">
				<div class="scores">
					<table>
						<tr>
							<th @click="sort('player')">Player</th>
							<th @click="sort('total')">Score</th>
							<th @click="sort('last')">Last</th>
						</tr>
						<tr v-for="score in scores">
							<td>{{score.player}}</td>
							<td>{{score.total}}</td>
							<td>{{score.last}}</td>
						</tr>
					</table>
				</div>
				<label>Party chat messages</label>
				<div class="messages" ref="messages"></div>
				<label>Type your message</label><br />
				<input class="long-input" type="text" name="input" maxlength="1024" title="Enter message You want to send to other players." v-on:keypress.enter="on_msg_enter">
			</div>
		</div>
`
	}
)
