"use strict"

class Boggle extends Party {
	static get TAG() { return "bgl" }
	static get NAME() { return "Boggle" }
	static create( app_, id_, name_, configuration_ ) {
		return ( new Boggle( app_, id_, name_, configuration_ ) )
	}
	constructor( app_, id_, name_, configuration_ ) {
		super( app_, id_, name_ )
		this._scores = []
		this._handlers["player_quit"] = function(){}
		this._handlers["deck"] = ( msg ) => this.on_deck( msg )
		this._handlers["longest"] = ( msg ) => this._refs.longest.append_text( msg + "\n" )
		this._handlers["scored"] = ( msg ) => this._refs.scored.append_text( msg + "\n" )
		this._handlers["player"] = ( msg ) => this.on_player( msg )
		this._handlers["setup"] = ( msg ) => this.on_setup( msg )
		this._handlers["round"] = ( msg ) => this.on_round( msg )
		this._handlers["end_round"] = ( msg ) => this.on_end_round( msg )
		this._currentSort = "player"
	  this._currentSortDir = "asc"
		this._deck = null
		this._timeLeft = 0
		this._state = "init"
		const conf = configuration_.split( "," )
		this._roundTime = conf[1] | 0
		this._pauseTime = conf[3] | 0
		this.input = ""
	}
	on_setup( data_ ) {
		const data = data_.split( "," )
		this._state = data[0]
		if ( this._state == "init" ) {
			this._timeLeft = 0
		} else {
			this._timeLeft = data[1] | 0
		}
	}
	on_deck( data_ ) {
		document.getElementById( "snd-dice" ).play()
		this._deck = data_
		data_ = data_.toUpperCase()
		for ( let i = 0; i < 16; ++ i ) {
			const letter = data_[i]
			this._refs.cube[i].textContent = letter == "Q" ? "Qu" : letter
		}
		if ( this._state != "over" ) {
			if ( ( this._state == "init" ) || ( this._state == "pause" ) ) {
				this._timeLeft = this._roundTime
			}
			this._state = "round"
		}
	}
	on_end_round( data_ ) {
		this._timeLeft = this._pauseTime
		this._state = "pause"
	}
	on_round( data_ ) {
		this._refs.sent.removeAllChildren()
		this._refs.longest.removeAllChildren()
		this._refs.scored.removeAllChildren()
	}
	on_player( data_ ) {
		const info = data_.split( "," )
		const idx = this._scores.findIndex( s => s.player == info[0] )
		if ( idx >= 0 ) {
			const s = this._scores[idx]
			s.total = info[1] | 0
			s.last = info[2] | 0
		} else {
			this._scores.push( { player: info[0], total: info[1] | 0, last: info[2] | 0 } )
		}
	}
}

Vue.component(
	"bgl", {
		props: ["data"],
		data: function( arg ) {
			this.data._refs = this.$refs
			return ( this.data )
		},
		mounted: function() {
			this.data._interval = setInterval( this.updateTimeLeft, 1000 )
		},
		beforeDestroy: function() {
			clearInterval( this.data._interval )
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
			on_bgl_enter: function() {
				if ( this.input.match( /.*\S+.*/ ) != null ) {
					this.$parent.sock.send( "cmd:" + this.data._id + ":play:" + this.input )
					this.data._refs.sent.append_text( this.input + "\n" )
				}
				this.input = ""
			},
			sort: function(s) {
				if( s === this.data._currentSort ) {
					this.data._currentSortDir = this.data._currentSortDir === "asc" ? "desc" : "asc";
				}
				this.data._currentSort = s;
			},
			updateTimeLeft() {
				if ( this.data._timeLeft > 0 ) {
					this.data._timeLeft -= 1
				}
			},
			add_letter: function( event, i ) {
				if ( this.data._deck != null ) {
					this.input += this.data._deck[i]
				}
				event.stopPropagation()
			}
		},
		computed: {
			timeLeft: function() {
				return (
					( "" + ( ( this.data._timeLeft / 60 ) | 0 ) ).padStart( 2, "0" )
					+ ":" + ( "" + ( this.data._timeLeft % 60 ) ).padStart( 2, "0" )
				)
			},
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
		<div class="tab-pane hbox boggle-pane">
			<div class="boggle-pane-left">
				<div class="boggle" @click="on_bgl_enter">
					<div class="cubes">
						<div v-for="( c, i ) in 'GAMEGROUNDBOGGLE'" ref="cube" class="noselect" @click="event => add_letter( event, i )">{{c}}</div>
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
				<div class="hbox" style="grid-area: input;">
					<label style="margin: auto; padding-left: 8px;">{{ timeLeft }}</label>
					<div>
						<label>Enter your words here ...</label><br />
						<input v-model="input" class="long-input" type="text" name="boggle-input" maxlength="1024" title="Here you shall enter words that you have found." v-on:keypress.enter="on_bgl_enter">
					</div>
				</div>
			</div>
			<div class="boggle-chat">
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

