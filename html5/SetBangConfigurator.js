"use strict"

class SetBangConfigurator {
	static get TAG() { return ( SetBang.TAG + "-configurator" ) }
	constructor( logic_ ) {
		const defaults = logic_._defaults.chop( ",", 3 )
		this.players = 2
		this.decks = 1
		this.interRoundDelay = 15
	}
	get_configuration_string() {
		return ( this.players + "," + this.decks + "," + this.interRoundDelay )
	}
}

Vue.component(
	SetBangConfigurator.TAG, {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		template: `
		<div id="set_bang-configurator">
			<label>Players:</label><input v-model="players" type="number" min="2" title="Minimum number of players to start the match." />
			<label>Number of decks:</label><input v-model="decks" type="number" min="1" title="Number of decks dealed for whole match." />
			<label>Inter round delay:</label><input v-model="interRoundDelay" type="number" min="0" title="Interval between rounds expressed in seconds." />
		</div>
`
	}
)

