"use strict"

class System {
	constructor( id_, coordX_, coordY_ ) {
		this._id = id_
		this._coordinateX = coordX_
		this._coordinateY = coordY_
		this._production = -1
		this._fleet = -1
		this._emperor = -1
	}
}

class Move {
	constructor( sourceSystem_, destinationSystem_, fleet_ ) {
		this._sourceSystem = sourceSystem_
		this._destinationSystem = destinationSystem_
		this._fleet = fleet_
	}
}

class Galaxy extends Party {
	static get TAG() { return ( "glx" ) }
	static get NAME() { return ( "Galaxy" ) }
	static get STATE() {
		return ( {
			WAIT: 0,
			LOCKED: 1,
			NORMAL: 2,
			SELECT: 3,
			INPUT: 4,
		} )
	}
	static get STATUSES() {
		return [
			"Wait for match to begin ...",
			"A waiting for GameGround events ...",
			"Make your imperial fleet moves ...",
			"Select destination for Your fleet ...",
			"How many destroyers You wish to send?",
		]
	}
	static get SYSTEM_NAMES() {
		return ( [ [
			"Aldebaran",
			"Betelgeuse",
			"Canis Major",
			"Deneb",
			"Eridanus",
			"Fomalhaut",
			"Gemini",
			"Hydra",
			"Izar",
			"Jabhat al Akrab",
			"Kochab",
			"Lupus",
			"Monoceros",
			"Norma",
			"Orion",
			"Procyon",
			"Quantum",
			"Reticulum",
			"Sirius",
			"Taurus",
			"Ursa Minor",
			"Vega",
			"Warrior",
			"Xerkses",
			"Yarn",
			"Zubenelgenubi",
			"Primum",
			"Secundum",
			"Tertium",
			"Quartum",
			"Quintum",
			"Sextum",
			"Septimum",
			"Octavum",
			"Nonum",
			"Decimum"
		], [
			"Aegir",
			"Balder",
			"C-Frey",
			"D-Tyr",
			"E-Frigg",
			"Freya",
			"Gullveig",
			"Hel",
			"Idun",
			"Jord",
			"Kvasir",
			"Loki",
			"Magni",
			"Njord",
			"Odin",
			"P-Forseti",
			"Q-Hod",
			"Ran",
			"Skadi",
			"Thor",
			"Ull",
			"Ve",
			"W-Vidar",
			"X-Sif",
			"Ymir",
			"Z-Heimdall",
			"Asgard",
			"Vanaheim",
			"Alfheim",
			"Jotunheim",
			"Hrimthursheim",
			"Muspellheim",
			"Midgard",
			"Svartalfheim",
			"Niflheim",
			"Yggdrasil"
		] ] )
	}
	static get SYMBOLS() {
		return ( [
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
			'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
		] )
	}
	static get COLOR() {
		return ( [
			"DodgerBlue",
			"LimeGreen",
			"brightred",
			"cyan",
			"DeepPink",
			"yellow",
			"blue",
			"green",
			"red",
			"DarkTurquoise",
			"MediumVioletRed",
			"brown",
			"lightgray",
			"othergray",
			"dimgray",
			"white",
			"lightgray"
		] )
	}
	static get COLORS() {
		return {
			WHITE: 15,
		}
	}
	static get COLOR_NORMAL() {
		return ( Galaxy.COLOR.length - 1 )
	}
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
		this._emperor = -1
		this._emperors = new Map()
		this._coordToSystem = new Map()
		this._handlers["setup"] = ( msg ) => this.on_setup( msg )
		this._handlers["play"] = ( msg ) => this.on_play( msg )
		this._moves = []
		this._sourceSystem = -1
		this._destinationSystem = -1
		this.nameSet = ( new Date().getHours() | 0 ) % 2
		this.hovered = null
		this.turn = 0
		this.arrival = 0
		this.state = Galaxy.STATE.WAIT
		this.showNames = false
		this.fleet = 0
		const fr = " 1fr".repeat( this._boardSize )
		this._boardStyle = "grid-template-columns:" + fr + "; grid-template-rows:" + fr + ";"
		// console.log( "ec = " + this._emperorCount + ", spc = " + this._startupPlayerCount + ", bs = " + this._boardSize + ", nsc = " + this._neutralSystemCount )
	}
	get state() {
		return ( this._state )
	}
	set state( state_ ) {
		this._state = state_
		this.arrival = this.turn
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
			if ( tokens[1] == this._app.myLogin ) {
				this._emperor = index
			}
		} else if ( setupItem == "ok" ) {
			this.state = Galaxy.STATE.NORMAL
		}
	}
	on_play( data_ ) {
		const play = data_.split( "=" )
		const playItem = play[0]
		const playData = play[1]
		if ( playItem == "system_info" ) {
			const systemInfo = playData.split( "," )
			const event = systemInfo[0]
			const sysNo = parseInt( systemInfo[1] )
			const production = parseInt( systemInfo[2] )
			const system = this._systems.get( sysNo )
			if ( production >= 0 ) {
				system._production = production
			}
			system._fleet = parseInt( systemInfo[4] )
			let emperor = parseInt( systemInfo[3] )
			let value = this._emperors.get( emperor )
			switch ( event ) {
				case ( "c" ): /* conquered */
				case ( "d" ): { /* defeted */
					this.log( value, emperor )
					this.log( " conquered ", Galaxy.COLOR_NORMAL )
					let temp = system._emperor
					temp = ( temp >= 0 ) ? temp : Galaxy.COLOR_NORMAL
					this.log( this.system_name( sysNo ), temp )
					this.log( "(" + Galaxy.SYMBOLS[ sysNo ] + ")", temp )
					this.log( ".\n", Galaxy.COLOR_NORMAL )
					system._emperor = emperor
				} break
				case ( 'r' ): { /* reinforcements */
					this.log( "Reinforcements for ", Galaxy.COLOR_NORMAL )
					this.log( this.system_name( sysNo ), emperor )
					this.log( "(" + Galaxy.SYMBOLS[ sysNo ] + ")", emperor )
					this.log( " arrived.\n", Galaxy.COLOR_NORMAL )
				} break
				case ( 'f' ):
				case ( 's' ): { /* resisted attack */
					if ( event == 'f' ) { /* failed to conquer */
						system._emperor = emperor
						emperor = this._emperor
						value = this._emperors.get( emperor )
					}
					let temp = system._emperor
					temp = ( temp >= 0 ) ? temp : Galaxy.COLOR_NORMAL
					this.log( this.system_name( sysNo ), temp )
					this.log( "(" + Galaxy.SYMBOLS[ sysNo ] + ")", temp )
					this.log( " resisted attack from ", Galaxy.COLOR_NORMAL )
					this.log( value, emperor )
					this.log( ".\n", Galaxy.COLOR_NORMAL )
				} break
				case ( 'i' ): /* info */ {
					system._emperor = emperor
				} break
				default: {
					break
				}
			}
		} else if ( playItem == "round" ) {
			this.log( "----- ", Galaxy.COLORS.WHITE )
			this.log( " round: ", Galaxy.COLOR_NORMAL )
			this.turn = parseInt( playData )
			this.log( playData + " -----\n", Galaxy.COLORS.WHITE )
			this.state = Galaxy.STATE.NORMAL
		}
		this.vm.$forceUpdate()
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
	system_name( id_ ) {
		return ( Galaxy.SYSTEM_NAMES[this.nameSet][id_] )
	}
	color_map( emp_ ) {
		return ( emp_ < Galaxy.COLOR.length ? Galaxy.COLOR[emp_] : Galaxy.COLOR[Galaxy.COLOR.length - 1] )
	}
	log( msg_, emp_ ) {
		this._refs.messages.append_text( msg_, emp_, this.color_map )
	}
	on_say( message_ ) {
		this._refs.messages.log_message( message_, this.color_map )
	}
	on_msg( message_ ) {
		this._refs.messages.append_text( message_ + "\n", null, this.color_map )
	}
	on_end_round() {
		this.state = Galaxy.STATE.LOCKED
		for ( let m of this._moves ) {
			this._app.sock.send( "cmd:" + this._id + ":play:move=" + m._sourceSystem + "," + m._destinationSystem + "," + m._fleet + "\n" );
		}
		this._app.sock.send( "cmd:" + this._id + ":play:end_round\n" );
		this._moves.clear()
	}
	on_click( idx_ ) {
		const system = this._coordToSystem.get( idx_ )
		if ( system ) {
			if ( ( this.state == Galaxy.STATE.NORMAL ) && ( system._emperor == this._emperor ) && ( system._fleet > 0 ) ) {
				this.state = Galaxy.STATE.SELECT
				this._sourceSystem = system._id
			} else if ( this.state == Galaxy.STATE.SELECT ) {
				this.state = Galaxy.STATE.INPUT
				this._destinationSystem = system._id
				this.fleet = this._systems.get( this._sourceSystem )._fleet
				this._refs.fleet.disabled = false
				this._refs.fleet.focus()
				this._refs.fleet.select()
				setTimeout( () => this._refs.fleet.select(), 50 )
			}
		}
	}
	on_fleet() {
		if ( this.state == Galaxy.STATE.INPUT ) {
			const src = this._systems.get( this._sourceSystem )
			if ( this.fleet > src._fleet ) {
				this.fleet = src._fleet
			}
			src._fleet -= this.fleet
			this._moves.push( new Move( this._sourceSystem, this._destinationSystem, this.fleet ) )
			this.state = Galaxy.STATE.NORMAL
			this.fleet = 0
		}
	}
}

Vue.component(
	Galaxy.TAG, {
		props: ["data"],
		data: function( arg ) {
			this.data._refs = this.$refs
			return ( this.data )
		},
		mounted: function() {
			this.data.vm = this
		},
		methods: {
			on_msg_enter( event ) {
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
			},
			dynamic_class( idx_ ) {
				const system = this.data._coordToSystem.get( idx_ )
				return ( ( ( system && ( system._emperor >= 0 ) ) || ( this.hovered && ( this.hovered === system ) ) ) ? "active" : "" )
			},
			dynamic_style( idx_ ) {
				const system = this.data._coordToSystem.get( idx_ )
				return ( system && ! ( this.hovered && ( this.hovered === system ) ) && ( system._emperor >= 0 ) ? "border-color: " + Galaxy.COLOR[system._emperor] + ";" : "" )
			},
			system_name( idx_ ) {
				const system = this.data._coordToSystem.get( idx_ )
				return ( system ? this.data.system_name( system._id ) : "" )
			},
			on_rpress() {
				this.showNames = true
			},
			on_rrelease() {
				this.showNames = false
			},
			status_text() {
				return ( Galaxy.STATUSES[this.data.state] )
			}
		},
		computed: {
		},
		template: `
		<div class="tab-pane hbox galaxy-pane">
			<div class="galaxy-pane-left">
				<div style="float: left; height: 2em;">
					<span class="galaxy-label">Galaxy</span><label>Emperor </label><label :style="'color: ' + data.color_map( data._emperor )">{{data._app.myLogin}}</label>
				</div>
				<div style="float: right;">
					<label>Fleet size:</label>
					<input ref="fleet" v-model="fleet" type="number" min="0" size="4" v-on:keypress.enter="data.on_fleet" :disabled="data.state != data.constructor.STATE.INPUT" />
					<label style="display: inline-block; width: 6em; padding-left: 1em;">Turn: {{turn}}</label>
					<label style="display: inline-block; width: 7em;">Arrival: {{arrival}}</label>
					<button @click="data.on_end_round()" :disabled="data.state != data.constructor.STATE.NORMAL">End round!</button>
				</div>
				<div ref="board" class="board" :style="data._boardStyle" @mousedown.right="on_rpress()" @mouseup.right="on_rrelease()">
					<div
						v-for="(_, i) in ( data._boardSize * data._boardSize )"
						@mouseover="on_mouseover( i )"
						@mouseout="on_mouseout( i )"
						@click="data.on_click( i )"
						:class="['system-box', dynamic_class( i )]"
						:style="dynamic_style( i )"
					>
						<div :class="['system', data.system_class(i)]"></div>
						<span v-show="showNames" class="system-name">{{ system_name( i ) }}</span>
					</div>
				</div>
			</div>
			<div class="galaxy-chat">
				<div class="galaxy-info">
					<label>System: {{hovered ? data.system_name( hovered._id ) : "?"}}</label></br>
					<label>Emperor: {{hovered && ( hovered._emperor >= 0 ) ? data._emperors.get( hovered._emperor ) : "?"}}</label></br>
					<label style="display: inline-block; width: 50%;">Production: {{hovered && ( hovered._production >= 0 ) ? hovered._production : "?"}}</label>
					<label>Fleet: {{hovered && ( hovered._fleet >= 0 ) ? hovered._fleet : "?"}}</label>
				</div>
				<label>Party chat messages</label>
				<div class="messages" ref="messages"></div>
				<label>Type your message</label><br />
				<input class="long-input" type="text" name="input" maxlength="1024" title="Enter message You want to send to other players." v-on:keypress.enter="on_msg_enter"><br />
				<span>{{status_text()}}</span>
			</div>
		</div>
`
	}
)

