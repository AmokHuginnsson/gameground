"use strict"

class System {
	constructor( id_, coordX_, coordY_ ) {
		this._id = id_
		this._coordinateX = coordX_
		this._coordinateY = coordY_
		this._production = -1
		this._fleet = -1
		this._emperor = null
	}
}

class Galaxy extends Party {
	static get TAG() { return "glx" }
	static get NAME() { return "Galaxy" }
	static create( app_, id_, name_, configuration_ ) {
		return ( new Galaxy( app_, id_, name_, configuration_ ) )
	}
	constructor( app_, id_, name_, configuration_ ) {
		super( app_, id_, name_ )
		const conf = configuration_.split( "," )
		this._emperorCount = parseInt( conf[0] )
		this._startupPlayerCount = parseInt( conf[1] )
		this._boardSize = parseInt( conf[2] )
		this._neutralSystemCount = parseInt( conf[3] )
		this._systems = new Map()
		this._color = -1
		this._emperor = null
		this._emperors = new Map()
		this._coordToSystem = new Map()
		this._handlers["setup"] = ( msg ) => this.on_setup( msg )
		this._handlers["play"] = ( msg ) => this.on_play( msg )
		this.statuses = [
		 	"Make your imperial fleet moves...",
		]
		this.hovered = null
		this.turn = 0
		this.arrival = 0
		this.status = 0
		const fr = " 1fr".repeat( this._boardSize )
		this._boardStyle = "grid-template-columns:" + fr + "; grid-template-rows:" + fr + ";"
		// console.log( "ec = " + this._emperorCount + ", spc = " + this._startupPlayerCount + ", bs = " + this._boardSize + ", nsc = " + this._neutralSystemCount )
	}
	on_setup( data_ ) {
		const setup = data_.split( "=" )
		const setupItem = setup[0]
		const setupData = setup[1]
		if ( setupItem == "board_size" ) {
			ensure( () => parseInt( setupData ) == this._boardSize, "Invalid board size." )
		} else if ( setupItem == "systems" ) {
			const systemCount = parseInt( setupData )
			ensure( () => systemCount == this.system_count(), "Invalid total system count." )
		} else if ( setupItem == "system_coordinates" ) {
			const systemCoordinate = setupData.split( "," )
			const systemNo = parseInt( systemCoordinate[0] )
			const system = new System( systemNo, parseInt( systemCoordinate[1] ), parseInt( systemCoordinate[2] ) )
		 	this._systems.set( systemNo, system )
			this._coordToSystem.set( this.idx( system ), system )
			if ( this._systems.size == this.system_count() ) {
				this.vm.$forceUpdate()
			}
		} else if ( setupItem == "emperor" ) {
			const tokens = setupData.split( "," )
			const index = parseInt( tokens[0] )
			this._emperors.set( index, tokens[1] )
			if ( tokens[1] == this._emperor ) {
				this._color = index
			}
		}
	}
	on_play( data_ ) {
		const play = data_.split( "=" )
		const playItem = play[0]
		const playData = play[1]
		if ( playItem == "system_info" ) {
		} else if ( playItem == "round" ) {
		}
	}
	idx( system_ ) {
		return ( this._boardSize * system_._coordinateY + system_._coordinateX )
	}
	system_class( idx_ ) {
		const system = this._coordToSystem.get( idx_ )
		return ( system ? "s" + system._id : "s" )
	}
	system_count() {
		return ( this._startupPlayerCount + this._neutralSystemCount )
	}
}

Vue.component(
	"glx", {
		props: ["data"],
		data: function( arg ) {
			this.data._refs = this.$refs
			return ( this.data )
		},
		mounted: function() {
			this.data.vm = this
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
			on_mouseover( idx_ ) {
				const system = this.data._coordToSystem.get( idx_ )
				if ( system ) {
					this.hovered = system
				}
			},
			on_mouseout( idx_ ) {
				if ( this.hovered == this.data._coordToSystem.get( idx_ ) ) {
					this.hovered = null
				}
			}
		},
		template: `
		<div class="tab-pane hbox galaxy-pane">
			<div class="galaxy-pane-left">
				<div style="float: left; height: 2em;">
					<span class="galaxy-label">Galaxy</span><label>Emperor </label><label>{{data._app.myLogin}}</label>
				</div>
				<div style="float: right;">
					<label>Fleet size:</label>
					<input type="number" size="4" value="0" />
					<label style="display: inline-block; width: 6em; padding-left: 2em;">Turn: {{turn}}</label>
					<label style="display: inline-block; width: 7em;">Arrival: {{arrival}}</label>
					<button>End round!</button>
				</div>
				<div ref="board" class="board" :style="data._boardStyle">
					<div v-for="(_, i) in ( data._boardSize * data._boardSize )" @mouseover="on_mouseover( i )" @mouseout="on_mouseout( i )" :class="['system-box', { active: hovered && ( data.idx( hovered ) == i ) }]">
						<div :class="['system', data.system_class(i)]"></div>
					</div>
				</div>
			</div>
			<div class="galaxy-chat">
				<div class="galaxy-info">
					<label>System: {{hovered ? hovered._id : "?"}}</label></br>
					<label>Emperor: {{hovered && hovered._emperor != null ? hovered._emperor : "?"}}</label></br>
					<label style="display: inline-block; width: 50%;">Production: {{hovered ? hovered._production : "?"}}</label><label>Fleet: {{hovered ? hovered._fleet : "?"}}</label>
				</div>
				<label>Party chat messages</label>
				<div class="messages" ref="messages"></div>
				<label>Type your message</label><br />
				<input class="long-input" type="text" name="input" maxlength="1024" title="Enter message You want to send to other players." v-on:keypress.enter="on_msg_enter"><br />
				<span>{{statuses[status]}}</span>
			</div>
		</div>
`
	}
)

