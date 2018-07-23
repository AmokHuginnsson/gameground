"use strict"

class MessageBox {
	static get TAG() { return ( "messagebox" ) }
	constructor( app_, message_, label_ = "OK", title_ = "alert!" ) {
		this._app = app_
		this._message = colorize( message_ )
		this._label = label_
		this._title = "GameGround - " + title_
		this._handlers = {
			"OK": () => this.on_ok()
		}
	}
	get message() {
		return ( this._message )
	}
	get label() {
		return ( this._label )
	}
	get title() {
		return ( this._title )
	}
	on_ok() {
		this.close()
	}
	then( callback_ ) {
		this._then = callback_
		return ( this )
	}
	on( label_, callback_ ) {
		this._handlers[label_] = callback_
		return ( this )
	}
	close() {
		this._app.modal = null
		if ( this._then != undefined ) {
			this._then()
		}
	}
}

Vue.component(
	"messagebox", {
		props: ["data"],
		data: function( arg ) {
			return ( this.data )
		},
		template: `
		<div class="modal">
			<div class="block"></div>
			<div class="messagebox">
				<div class="title noselect">{{ data.title }}</div>
				<div class="content">
					<p v-html="data.message"></p>
					<button v-for="handler, label in data._handlers" v-on:click="handler()">{{ label }}</button>
				</div>
			</div>
		</div>
`
	}
)

