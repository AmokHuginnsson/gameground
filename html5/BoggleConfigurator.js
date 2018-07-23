"use strict"

class BoggleConfigurator {
	static get TAG() { return ( Boggle.TAG + "-configurator" ) }
	constructor( logic_ ) {
		const defaults = logic_._defaults.chop( ",", 6 )
		this.language = defaults[0]
		this.scoring = defaults[1]
		this.players = parseInt( defaults[2] )
		this.roundTime = parseInt( defaults[3] )
		this.noRounds = parseInt( defaults[4] )
		this.interRoundDelay = parseInt( defaults[5] )
	}
	get_configuration_string() {
		return (
			this.language
			+ "," + this.scoring
			+ "," + this.players
			+ "," + this.roundTime
			+ "," + this.noRounds
			+ "," + this.interRoundDelay
		)
	}
}

Vue.component(
	BoggleConfigurator.TAG, {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		template: `
		<div id="bgl-configurator">
			<label>Language:</label>
			<select v-model="language" title="Language for dices and dictionary.">
				<option value="en">English</option>
				<option value="pl">Polish</option>
			</select>
			<label>Scoring system:</label>
			<select v-model="scoring" title="Choose the way the score is calculated.">
				<option value="original">Original Boggle</option>
				<option value="fibonacci">Fibonacci</option>
				<option value="geometric">Geometric</option>
				<option value="fibonacci4">Fibonacci 4-based</option>
				<option value="geometric4">Geometric 4-based</option>
				<option value="longestwords">Longest words</option>
			</select>
			<label>Players:</label><input v-model="players" type="number" min="2" title="Minimum number of players to start the match." />
			<label>Round time:</label><input v-model="roundTime" type="number" min="60" title="Length of one round expressed in seconds." />
			<label>Number of rounds:</label><input v-model="noRounds" type="number" min="1" title="Number of rounds for whole match." />
			<label>Inter round delay:</label><input v-model="interRoundDelay" type="number" min="0" title="Interval between rounds expressed in seconds." />
		</div>
`
	}
)

