"use strict"

class Browser extends Party {
	static get TAG() { return "browser" }
	constructor() {
		super( null, "browser", "Browser", "" )
	}
	get name() {
		return ( this._name )
	}
	drop_player( dummy_ ) {
	}
}

Vue.component(
	"browser", {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		methods: {
			on_enter: function( event ) {
				const source = event.target || event.srcElement
				if ( source.value !== "" ) {
					this.$parent.sock.send( "msg:" + source.value )
				}
				source.value = ""
			},
			on_player_dblclick: function( login ) {
				if ( login == this.$parent.myLogin ) {
					return
				}
				let id = null
				for ( let p of this.$parent.partys ) {
					if ( ( p.TAG == Chat.TAG ) && ( p._name == login ) ) {
						id = p._id
						break
					}
				}
				if ( id == null ) {
					this.$parent.sock.send( "get_account:" + login )
				} else {
					this.$parent.make_visible( id )
				}
			},
			select_logic: function( data ) {
				this.$parent.selectedPartyLogic = data
			},
			select_party: function( data, event ) {
				this.$parent.selectedPartyLogic = data
				event.stopPropagation()
			},
			join_party: function( data, event = null ) {
				this.$parent.selectedPartyLogic = data
				if ( event != null ) {
					event.stopPropagation()
				}
				if ( data._id !== undefined ) {
					const party = this.$parent.party_by_id( data._id, true )
					if ( party == null ) {
						this.$parent.sock.send( "join:" + data._id )
					} else {
						this.$parent.make_visible( data._id )
					}
				}
			},
			toggle_expand: function( data, event = null ) {
				data._expanded = ! data._expanded
				if ( event != null ) {
					event.stopPropagation()
				}
				if ( ! data._expanded && ( data._partys.indexOf( this.$parent.selectedPartyLogic ) >= 0 ) ) {
					this.$parent.selectedPartyLogic = data
				}
			},
			on_logic_context: function( data ) {
				this.on_create( data )
			},
			on_join: function() {
				this.join_party( this.$parent.selectedPartyLogic )
			},
			on_create: function( data ) {
				this.$parent.modal = new NewGameConfigurator( this.$parent, data )
			},
			on_account: function() {
			}
		},
		template: `
			<div id="browser" class="tab-pane">
				<label>Server messages</label>
				<label>Games</label>
				<label style="grid-column: 4">People</label>
				<div id="chat-view" class="messages"></div>
				<ul id="games" class="treewidget">
					<li :class="['noselect', { selected: $parent.selectedPartyLogic == null }]" v-on:click="select_logic( null )">GameGround</li>
					<li
						class="noselect"
						v-for="logic in $parent.logics"
						:key="logic._class.TAG"
						@click="select_logic( logic )"
						@dblclick="toggle_expand( logic )"
						@click.right="on_logic_context( logic )"
					>
						<span class="icon" @click="event => toggle_expand( logic, event )">
							{{ logic._partys.length === 0 ? "╰⊡" : ( ( logic._partys.length &gt; 0 &amp;&amp; logic._expanded ) ? "╰⊟" : "╰⊞" ) }}
						</span>
						<span
							:class="[{ selected: $parent.selectedPartyLogic == logic }]"
						>{{ logic.name }}</span>
						<ul class="treebranch" v-show="logic._partys.length > 0 && logic._expanded">
							<li
								:title="party.name"
								class="noselect"
								v-for="party in logic._partys"
								@click="event => select_party( party, event )"
								@dblclick="event => join_party( party, event )"
								:key="party._id"
							><span :class="[{ selected: $parent.selectedPartyLogic == party }]">{{ party.name }}</span>
							</li>
						</ul>
					</li>
				</ul>
				<ul id="players" class="listwidget">
					<li
						class="noselect"
						v-for="player in $parent.players"
						v-on:dblclick="on_player_dblclick( player.login )"
					>{{ player.login }}</li>
				</ul>
				<button id="btn-join" v-on:click="on_join" title="Click me to join pre-existing game." :disabled="( this.$parent.selectedPartyLogic == null ) || ( this.$parent.selectedPartyLogic._id === undefined ) || ( this.$parent.party_by_id( this.$parent.selectedPartyLogic._id, true ) != null )">Join</button>
				<button id="btn-create" v-on:click="on_create( null )" title="Click me to create a new game.">Create</button>
				<button id="btn-account" v-on:click="on_account" title="Click me to edit your account information." :disabled="!this.$parent.registered">Account</button>
				<button id="btn-disconnect" v-on:click="$parent.do_disconnect" title="Click me to disconnet from GameGround server.">Disconnect</button>
				<div id="chat-input-field">
					<label>Type your message</label><br />
					<input id="chat-input" type="text" name="input" maxlength="1024" title="Send message to all people on the server that are currently not in the game." v-on:keypress.enter="on_enter">
				</div>
			</div>
`
	}
)

