class Text {
	constructor( text_, class_ ) {
		this._text = text_
		this._class = class_
		console.log( "text = " + this._text + ", color = " + this._class )
	}
}

class Message {
	static get TAG() { return ( "message" ) }
	static next_id() {
		const id = Message._idGenerator
		Message._idGenerator += 1
		return id
	}
	constructor( who_, data_, side_ ) {
		this._who = side_ != "me" ? who_ : null
		this._data = data_
		this._side = side_
		this._when = new Date()
		this._id = Message.next_id()
	}
	append( data_ ) {
		this._data.push( null )
		this._data = this._data.concat( data_ )
	}
}
Message._idGenerator = 1

Vue.component(
	Message.TAG, {
		props: ["data"],
		data: function() {
			return ( this.data )
		},
		methods: {
			linkize: function( link ) {
				const url = link.startsWith( "http" ) ? link : "https://" + link
				return ( url )
			}
		},
		template: `
		<div :class="$data._side" :title="$data._when.iso8601()">
			<span v-show="$data._side == 'them'" style="padding: 2px; background-color: rgba( 255, 255, 255, 0.7 );">{{ $data._who }}</span>
			<template v-for="item in $data._data">
				<span v-if="item && ( typeof item === 'object' )" :style="'color: ' + item._class">{{ item._text }}</span>
				<a v-else-if="typeof item === 'string'" :href="linkize( item )">{{ item }}</a>
				<br v-else />
			</template>
		</div>
`
	}
)

class LogPad {
	static get TAG() { return ( "logpad" ) }
	constructor( app_, colorMap_ = colorMap ) {
		this._app = app_
		this._colorMap = colorMap_
		this._messages = []
	}
	parse_message( message_ ) {
		const tokens = message_.chop( ":", 2 )
		const isPlayer = ( tokens.length == 2 ) && ( this._app.players.findIndex( x => x.login == tokens[0] ) >= 0 )
		const who = isPlayer ? tokens[0] : null
		const side = isPlayer
			? ( who == this._app.myLogin ? "me" : "them" )
			: ""
		const whatRaw = isPlayer
			? tokens[1]
			: tokens[0]
		const linkPattern = /([^\s<]+[.][^\s<]+)/
		const colorPattern = /(\$\d+;)/
		const splitPattern = /(\$\d+;)|(\n)|([^\s<]+[.][^\s<]+)/g
		const items = whatRaw.split( splitPattern );
		const what = []
		let color = "";
		for ( const item of items ) {
			if ( ( item === "" ) || ( item === undefined ) ) {
				continue
			}
			console.log( "item = " + item )
			if ( colorPattern.test( item ) ) {
				color = this._colorMap( item.substr( 1, item.length - 2 ) )
			} else if ( linkPattern.test( item ) ) {
				what.push( item )
			} else if ( item == "\n" ) {
				what.push( null )
			} else {
				what.push( new Text( item, color ) )
			}
		}
		return ( isPlayer ? [ who, what, side ] : [ who, what, "" ] )
	}
	add_message( message_ ) {
		const [ who, what, side ] = this.parse_message( message_ )
		const messageCount = this._messages.length
		const last = ( messageCount > 0 ) ? this._messages[messageCount - 1] : null
		const now = new Date()
		if ( ( messageCount == 0 ) || ( last._side != side ) || ( ( now - last._when ) > 60000 ) ) {
			this._messages.push( new Message( who, what, side ) )
		} else {
			last.append( what )
			last._when = now
		}
	}
}

Vue.component(
	LogPad.TAG, {
		props: ["app"],
		data: function() {
			return ( new LogPad( this.app ) )
		},
		template: `
		<div class="messages">
			<message v-for="message in $data._messages" :data="message" :key="message._id" />
		</div>
`
	}
)

