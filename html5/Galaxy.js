"use strict"

class System {
	constructor() {
		this._coordinateX = -1
		this._coordinateY = -1
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
		this._systems = []
		this._color = -1
		this._emperor = null
		this._emperors = new Map()
		this._handlers["setup"] = ( msg ) => this.on_setup( msg )
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
			ensure( () => systemCount == ( this._startupPlayerCount + this._neutralSystemCount ), "Invalid total system count." )
			for ( let i = 0; i < systemCount; ++ i ) {
				this._systems.push( new System() )
			}
		} else if ( setupItem == "system_coordinate" ) {
			const systemCoordinate = setupData.split( "," )
			const systemNo = parseInt( systemCoordinate[0] )
			const system = this._systems[systemNo]
			system._coordinateX = parseInt( systemCoordinate[1] )
			system._coordinateY = parseInt( systemCoordinate[2] )
		} else if ( setupItem == "emperor" ) {
			const tokens = setupData.split( "," )
			const index = parseInt( tokens[0] )
			this._emperors.set( index, tokens[1] )
			if ( tokens[1] == this._emperor ) {
				this._color = index
			}
		} else if ( setupItem == "system_info" ) {
		}
	}
}

Vue.component(
	"glx", {
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
		template: `
		<div class="tab-pane hbox galaxy-pane">
			<div class="galaxy-pane-left">
				<div class="board">
					<div v-for="i in data._boardSize"></div>
				</div>
			</div>
			<div class="galaxy-chat">
				<label>Private chat messages</label>
				<div class="messages" ref="messages"></div>
				<label>Type your message</label><br />
				<input class="long-input" type="text" name="input" maxlength="1024" title="Enter message You want to send to other players." v-on:keypress.enter="on_msg_enter">
			</div>
		</div>
`
	}
)

