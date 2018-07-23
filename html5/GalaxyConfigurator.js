"use strict"

class GalaxyConfigurator {
	static get TAG() { return ( Galaxy.TAG + "-configurator" ) }
	constructor( logic_ ) {
		const defaults = logic_._defaults.chop( ",", 3 )
		this.emperors = parseInt( defaults[0] )
		this.boardSize = parseInt( defaults[1] )
		this.systems = parseInt( defaults[2] )
	}
	get_configuration_string() {
		return ( this.emperors + "," + this.boardSize + "," + this.systems )
	}
}

Vue.component(
	GalaxyConfigurator.TAG, {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		template: `
		<div id="glx-configurator">
			<label>Emperors:</label><input v-model="emperors" type="number" min="2" title="Minimum number of players to start the match." />
			<label>Systems:</label><input v-model="systems" type="number" min="0" title="Total numbers of neutral star systems in the game." />
			<label>Board size:</label><input v-model="boardSize" type="number" min="6" title="Map grid size." />
		</div>
`
	}
)

