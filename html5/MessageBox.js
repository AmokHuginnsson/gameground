"use strict"

class MessageBox {
	static get TAG() { return ( "messagebox" ) }
	constructor( app_, message_, label_ = "OK", title_ = "alert!" ) {
		this._app = app_
		this._message = colorize( message_ )
		this._label = label_
		this._title = "GameGround - " + title_
		this._lastX = 0
		this._lastY = 0
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
	dragMouseDown( event ) {
		event = event || window.event
		event.preventDefault()
		// get the mouse cursor position at startup:
		this._lastX = event.clientX
		this._lastY = event.clientY
		document.onmouseup = this.closeDragElement
		// call a function whenever the cursor moves:
		document.onmousemove = ( event ) => this.elementDrag( event )
	}
	elementDrag( event ) {
		event = event || window.event
		event.preventDefault()
		// calculate the new cursor position:
		const offsetX = this._lastX - event.clientX
		const offsetY = this._lastY - event.clientY
		this._lastX = event.clientX
		this._lastY = event.clientY
		// set the element's new position:
		const m = this._refs.messagebox
		m.style.top = ( m.offsetTop - offsetY ) + "px"
		m.style.left = ( m.offsetLeft - offsetX ) + "px"
	}
	closeDragElement() {
		/* stop moving when mouse button is released:*/
		document.onmouseup = null
		document.onmousemove = null
	}
}

Vue.component(
	"messagebox", {
		props: ["data"],
		data: function( arg ) {
			this.data._refs = this.$refs
			return ( this.data )
		},
		template: `
		<div class="modal">
			<div class="block"></div>
			<div ref="messagebox" class="messagebox">
				<div @mousedown="( event ) => data.dragMouseDown( event )" class="title noselect">{{ data.title }}</div>
				<div class="content">
					<p v-html="data.message"></p>
					<button v-for="handler, label in data._handlers" v-on:click="handler()">{{ label }}</button>
				</div>
			</div>
		</div>
`
	}
)

