"use strict"

class NewGameConfigurator extends MessageBox {
	static get TAG() { return ( "new-game-configuration" ) }
	constructor( app_, logic_ ) {
		super( app_, "" )
		this._configurators = [
			new BoggleConfigurator( app_.logic_by_type( Boggle.TAG ) ),
			new GalaxyConfigurator( app_.logic_by_type( Galaxy.TAG ) ),
			new GoConfigurator( app_.logic_by_type( Go.TAG ) ),
			new GomokuConfigurator( app_.logic_by_type( Gomoku.TAG ) ),
			new SetBangConfigurator( app_.logic_by_type( SetBang.TAG ) )
		]
		this._selected = -1
		this.name = ""
		if ( logic_ != null ) {
			this._selected = this._configurators.findIndex( c => c.constructor.TAG == ( logic_.TAG + "-configurator" ) )
			this.name = app_.myLogin + "'s room ..."
		}
	}
	on_logic_context( data ) {
		this._selected = data
	}
	get selected() {
		return ( this._selected >= 0 ? this._configurators[this._selected] : null )
	}
	on_ok() {
		const selected = this.selected
		const confStr = selected.get_configuration_string()
		const TAG = selected.constructor.TAG
		let conf = TAG.substr( 0, TAG.length - "-configurator".length ) + ":" + this.name
		if ( confStr != "" ) {
			conf += ( "," + confStr )
		}
		this._app.sock.send( "create:" + conf )
		this.close()
	}
	on_cancel() {
		this.close()
	}
	close() {
		this._app.modal = null
	}
}

Vue.component(
	NewGameConfigurator.TAG, {
		props: ["data"],
		data: function( arg ) {
			this.data._refs = this.$refs
			return ( this.data )
		},
		mounted: function() {
			this.$refs.name.focus()
		},
		methods: {
			on_key_press: function( event ) {
				switch ( event.key ) {
					case ( "Home" ): {
						this.data._selected = 0
					} break
					case ( "End" ): {
						this.data._selected = this.data._configurators.length - 1
					} break
					case ( "ArrowUp" ): {
						if ( this.data._selected > 0 ) {
							this.data._selected -= 1
						}
					} break
					case ( "ArrowDown" ): {
						if ( this.data._selected < ( this.data._configurators.length - 1 ) ) {
							this.data._selected += 1
						}
					} break
				}
			}
		},
		template: `
		<div class="modal" tabindex="1" v-on:keypress.escape="data.close()">
			<div class="block"></div>
			<div ref="messagebox" class="messagebox">
				<div @mousedown="( event ) => data.dragMouseDown( event )" class="title noselect">Create new game ...</div>
				<div class="creator">
					<div class="vbox">
						<div class="hbox">
							<div class="vbox">
								<label>Game type ...</label>
								<ul ref="games" class="listwidget" title="Available game types." v-on:keypress="on_key_press" tabindex="0">
									<li
										class="noselect"
										v-for="( logic, index ) in $parent.logics"
										:class="[{selected: ( data._selected >= 0 ) && ( data._selected == index )}]"
										v-bind:key="logic._class.TAG"
										v-on:click="data.on_logic_context( index )"
									>{{ logic.name }}
									</li>
								</ul>
							</div>
							<div class="vbox">
								<div class="center label">
									<label>Name: </label><input ref="name" v-model="name" type="text" title="Name for newly created game." />
								</div>
								<label class="edge">Configuration ...</label>
								<div id="configuration">
									<component v-if="data._selected >= 0" :data="data.selected" :is="data.selected.constructor.TAG"></component>
									<div v-else></div>
								</div>
							</div>
						</div>
						<div id="creator-buttons">
							<button v-on:click="data.on_ok()" :disabled="( data.selected == null ) || ( name == '' )">Ok</button>
							<button v-on:click="data.on_cancel()">Cancel</button>
						</div>
					</div>
				</div>
			</div>
		</div>
`
	}
)

