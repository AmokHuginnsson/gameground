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
		this._when = new Date().iso8601()
		this._id = Message.next_id()
	}
	append( data_ ) {
		this._data += data_
	}
}
Message._idGenerator = 1

Vue.component(
	Message.TAG, {
		props: ["data"],
		data: function() {
			return ( this.data )
		},
		template: `
		<div :class="$data._side" :title="$data._when">
			<span v-show="$data._side != 'me'" style="padding: 2px; background-color: rgba( 255, 255, 255, 0.7 );">{{ $data._who }}</span>
			{{ $data._data }}
		</div>
`
	}
)

class LogPad {
	static get TAG() { return ( "logpad" ) }
	constructor( app_, colorMap_ = null ) {
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
		const what = isPlayer
			? tokens[1]
			: tokens[0]
		return ( isPlayer ? [ who, what, side ] : [ who, message_, "" ] )
	}
	add_message( message_ ) {
		const [ who, what, side ] = this.parse_message( message_ )
		this._messages.push( new Message( who, what, side ) )
	}
	append_text( message_ ) {
		const [ who, what, side ] = this.parse_message( message_ )
		const messageCount = this._messages.length
		const last = ( messageCount > 0 ) ? this._messages[messageCount - 1] : null
		if ( ( messageCount == 0 ) || ( last._side != side ) ) {
			this._messages.push( new Message( who, what, side ) )
		} else {
			last.append( message_ )
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

