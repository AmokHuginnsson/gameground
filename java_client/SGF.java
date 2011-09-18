import java.util.Vector;

class SGFException extends Exception {
}

public class SGF {
	public class GAME_TYPE {
		enum game_type_t {
			GO( 1 ),
			GOMOKU( 4 )
		}
	}
	public class ERROR {
		enum code_t {
			UNEXPECTED_EOF( 0 ),
			UNEXPECTED_DATA( 1 ),
			GT_OPEN_EXPECTED( 2 ),
			GT_CLOSE_EXPECTED( 3 ),
			PROP_IDENT_EXPECTED( 4 ),
			PROP_VAL_OPEN_EXPECTED( 5 ),
			PROP_VAL_CLOSE_EXPECTED( 6 ),
			NODE_MARK_EXPECTED( 7 ),
			BAD_GAME_TYPE( 8 ),
			BAD_FILE_FORMAT( 9 )
		}
	}
	private class TERM {
		static final char GT_OPEN = '(';
		static final char GT_CLOSE = ')';
		static final char PROP_VAL_OPEN = '[';
		static final char PROP_VAL_CLOSE = ']';
		static final char NODE_MARK = ';';
		static final char ESCAPE = '\\';
	}
	private class Game {
		class Move {
			class Player {
				enum player_t {
					BLACK,
					WHITE,
					UNSET
				}
			}
			char[] _coord = { 0, 0, 0 };
			String _comment = "";
			Move() { }
			Move( int col_, int row_ )
				{
				_coord[0] = (char)( col_ + 'a' );
				_coord[1] = (char)( row_ + 'a' );
				}
			Move( final String coord_ ) {
				coord( coord_ );
			}
			String coord() {
				return ( _coord );
			}
			void coord( final String coord_ ) {
				if ( coord_.length() >= 2 )
					{
					_coord[0] = coord_[0];
					_coord[1] = coord_[1];
					}
				else
					_coord[0] = _coord[1] = 0;
			}
			int col() {
				return ( _coord[0] - 'a' );
			}
			int row() {
				return ( _coord[1] - 'a' );
			}
		}
		static final int RESIGN = 0xffff;
		static final int TIME = 0x7fff;
		String _blackName = "";
		String _whiteName = "";
		String _blackRank = "30k";
		String _whiteRank = "30k";
		Vector<Move> _blackPreset = new Vector<Move>();
		Vector<Move> _whitePreset = new Vector<move>();
		HTree<Move> _tree = new HTree<Move>();
		Player.player_t _firstToMove = Player.UNSET;
		int _gobanSize = 19;
		int _time = 0;
		int _handicap = 0;
		double _komi = 5.5;
		int _result = 0;
		String _place = "";
		String _comment = "";
		Game() { }
		HNode<Move> move( HTree<Move>.HNove<Move> node_, Player.player_t player_, int col_, int row_ )
			{
			return ( _game.move( node_, player_, col_, row_ ) );
			}

		void add_stone( Player.player_t player_, int col_, int row_ ) {
			add_stone( player_, Move( col_, row_ ) );
			return;
		}

		void add_stone( Player.player_t player_, Move final& move_ ) {
			if ( player_ == Player.BLACK )
				_blackPreset.push_back( move_ );
			else
				_whitePreset.push_back( move_ );
			return;
		}

		HTree<Move>.HNode<Move> move( HTree<Move>.HNode<Move> node_, Player.player_t, int col_, int row_ ) {
			return ( &*node_.add_node( Move( col_, row_ ) ) );
		}

		void clear() {
			_blackPreset.clear();
			_whitePreset.clear();
			_tree.clear();
			_firstToMove = Player.UNSET;
			_blackName.clear();
			_blackRank.clear();
			_whiteName.clear();
			_whiteRank.clear();
			_komi = 5.5;
			_handicap = 0;
			_gobanSize = 19;
			_result = 0;
			_place.clear();
			_comment.clear();
			return;
		}
	}
	GAME_TYPE.game_type_t _gameType;
	String _rawData;
	int _beg = 0;
	int _cur = 0;
	int _end = 0;
	String _cache;
	String _cachePropIdent;
	Vector<String> _cachePropValue = new Vector<String>();
	Game _game = new Game();
	HTree<Move>.HNode<Move> _currentMove = null;
	String _app = null;

	SGF( GAME_TYPEd.game_type_t $type ) {
		this( $type, "libsgf" )
	}

	public SGF( GAME_TYPE.game_type_t $gameType, final String $app ) {
		_app = $app;
		_gameType = $gameType;
	/*
		, _rawData(), _beg( null ), _cur( null ), _end( null ),
		_cache(), _cachePropIdent(), _cachePropValue(), _game(),
		_currentMove( null ), _app( app_ ) {
	*/
	}

	final String[] _errMsg_ = {
		"Unexpected end of file.",
		"Unexpected data bytes found.",
		"Expected GameTree opening sequence.",
		"Expected GameTree closing sequence.",
		"Expected property identifier.",
		"Expected property value opening sequence.",
		"Expected property value closing sequence.",
		"Expected node start marker sequence.",
		"Bad game type.",
		"Bad file format."
	};

	static int non_space( int first, int last ) {
		for ( ; first != last ; ++ first ) {
			if ( ! memchr( _whiteSpace_.data(), *first, _whiteSpace_.size() ) )
				break;
		}
		return ( first );
	}

	void load( HStreamInterface& stream_ ) {
		static int final BUFFER_SIZE( 4096 );
		HChunk buffer( BUFFER_SIZE );
		int nRead( 0 );
		clear();
		while ( ( nRead = static_cast<int>( stream_.read( buffer.raw(), BUFFER_SIZE ) ) ) > 0 )
			_rawData.append( buffer.raw(), nRead );
		_beg = _rawData.begin();
		_cur = _rawData.begin();
		_end = _rawData.end();
		clog << _rawData << endl;
		_cur = non_space( _cur, _end );
		try {
			parse_game_tree();
			while ( _cur != _end )
				parse_game_tree();
		} catch ( final SGFException e ) {
			if ( _cur != _end )
				cerr << "Failed at byte: `" << *_cur << "'" << endl;
			throw;
		}
		return;
	}

	void clear() {
		_beg = _cur = _end = null;
		_currentMove = null;
		_rawData.clear();
		_game.clear();
		return;
	}

	void not_eof() {
		if ( _cur == _end )
			throw SGFException( _errMsg_[ERROR.UNEXPECTED_EOF], static_cast<int>( _cur - _beg ) );
		return;
	}

	void parse_game_tree() {
		not_eof();
		if ( *_cur != TERM.GT_OPEN )
			throw SGFException( _errMsg_[ERROR.GT_OPEN_EXPECTED], static_cast<int>( _cur - _beg ) );
		_cur = non_space( ++ _cur, _end );
		not_eof();
		if ( ! _currentMove )
			_currentMove = _game._tree.create_new_root();
		parse_sequence();
		HTree<Move>.HNode<Move> preVariationMove( _currentMove );
		while ( ( _cur != _end ) && ( *_cur != TERM.GT_CLOSE ) ) {
			_currentMove = &*preVariationMove.add_node();
			parse_game_tree();
			not_eof();
		}
		if ( *_cur != TERM.GT_CLOSE )
			throw SGFException( _errMsg_[ERROR.GT_CLOSE_EXPECTED], static_cast<int>( _cur - _beg ) );
		_cur = non_space( ++ _cur, _end );
		return;
	}

	void parse_sequence() {
		parse_node();
		_cur = non_space( _cur, _end );
		not_eof();
		while ( *_cur == TERM.NODE_MARK ) {
			_currentMove = &*_currentMove.add_node();
			parse_node();
			_cur = non_space( _cur, _end );
			not_eof();
		}
		return;
	}

	void parse_node() {
		if ( *_cur != TERM.NODE_MARK )
			throw SGFException( _errMsg_[ERROR.NODE_MARK_EXPECTED], static_cast<int>( _cur - _beg ) );
		_cur = non_space( ++ _cur, _end );
		not_eof();
		while ( ( *_cur != TERM.GT_CLOSE ) && ( *_cur != TERM.GT_OPEN ) && ( *_cur != TERM.NODE_MARK ) ) {
			parse_property();
			_cur = non_space( _cur, _end );
			not_eof();
		}
		return;
	}

	void parse_property() {
		_cachePropIdent = parse_property_ident();
		if ( _cachePropIdent.is_empty() )
			throw SGFException( _errMsg_[ERROR.PROP_IDENT_EXPECTED], static_cast<int>( _cur - _beg ) );
		_cur = non_space( _cur, _end );
		not_eof();
		_cachePropValue.clear();
		parse_property_value( _cachePropValue );
		while ( *_cur == TERM.PROP_VAL_OPEN )
			parse_property_value( _cachePropValue );
		String final& singleValue( _cachePropValue[0] );
		if ( _cachePropIdent == "GM" ) {
			if ( lexical_cast<int>( singleValue ) != _gameType )
				throw SGFException( _errMsg_[ERROR.BAD_GAME_TYPE], static_cast<int>( _cur - _beg ) );
		} else if ( _cachePropIdent == "FF" ) {
			int ff( lexical_cast<int>( singleValue ) );
			if ( ( ff < 1 ) || ( ff > 4 ) )
				throw SGFException( _errMsg_[ERROR.BAD_FILE_FORMAT], static_cast<int>( _cur - _beg ) );
		} else if ( _cachePropIdent == "PB" )
			_game._blackName = singleValue;
		else if ( _cachePropIdent == "PW" )
			_game._whiteName = singleValue;
		else if ( _cachePropIdent == "BR" )
			_game._blackRank = singleValue;
		else if ( _cachePropIdent == "WR" )
			_game._whiteRank = singleValue;
		else if ( _cachePropIdent == "KM" )
			_game._komi = lexical_cast<double>( singleValue );
		else if ( _cachePropIdent == "HA" )
			_game._handicap = lexical_cast<int>( singleValue );
		else if ( _cachePropIdent == "SZ" )
			_game._gobanSize = lexical_cast<int>( singleValue );
		else if ( _cachePropIdent == "TM" )
			_game._time = lexical_cast<int>( singleValue );
		else if ( _cachePropIdent == "PC" )
			_game._place = singleValue;
		else if ( _cachePropIdent == "RE" ) {
			if ( isdigit( singleValue[2] ) )
				_game._result = lexical_cast<int>( singleValue.raw() + 2 );
			else {
				char r( static_cast<char>( toupper( singleValue[2] ) ) );
				if ( r == 'R' )
					_game._result = Game.RESIGN;
				else if ( r == 'T' )
					_game._result = Game.TIME;
			}
			char player( static_cast<char>( toupper( singleValue[0] ) ) );
			if ( player == 'W' )
				_game._result = -_game._result;
		} else if ( _cachePropIdent == "AB" ) {
			for ( Vector<String>.const_iterator it( _cachePropValue.begin() ), end( _cachePropValue.end() ); it != end; ++ it )
				_game.add_stone( Player.BLACK, Move( *it ) );
		} else if ( _cachePropIdent == "AW" ) {
			for ( Vector<String>.const_iterator it( _cachePropValue.begin() ), end( _cachePropValue.end() ); it != end; ++ it )
				_game.add_stone( Player.WHITE, Move( *it ) );
		} else if ( _cachePropIdent == "B" ) {
			if ( _game._firstToMove == Player.UNSET )
				_game._firstToMove = Player.BLACK;
			(**_currentMove).coord( singleValue );
		} else if ( _cachePropIdent == "W" ) {
			if ( _game._firstToMove == Player.UNSET )
				_game._firstToMove = Player.WHITE;
			(**_currentMove).coord( singleValue );
		} else if ( _cachePropIdent == "C" ) {
			if ( _game._firstToMove != Player.UNSET )
				(**_currentMove)._comment = singleValue;
			else
				_game._comment += singleValue;
		} else
			clog << "property: `" << _cachePropIdent << "' = `" << singleValue << "'" << endl;
		return;
	}

	String parse_property_ident() {
		_cache.clear();
		while ( ( _cur != _end ) && isupper( *_cur ) ) {
			_cache += *_cur;
			++ _cur;
		}
		return ( _cache );
	}

	void parse_property_value( Vector<String> values_ ) {
		not_eof();
		if ( *_cur != TERM.PROP_VAL_OPEN )
			throw SGFException( _errMsg_[ERROR.PROP_VAL_OPEN_EXPECTED], static_cast<int>( _cur - _beg ) );
		_cur = non_space( ++ _cur, _end );
		not_eof();
		_cache.clear();
		bool escaped( false );
		while ( ( _cur != _end ) && ( escaped || ( *_cur != TERM.PROP_VAL_CLOSE ) ) ) {
			escaped = ( *_cur == TERM.ESCAPE );
			if ( ! escaped )
				_cache += *_cur;
			++ _cur;
		}
		not_eof();
		if ( *_cur != TERM.PROP_VAL_CLOSE )
			throw SGFException( _errMsg_[ERROR.PROP_VAL_CLOSE_EXPECTED], static_cast<int>( _cur - _beg ) );
		_cur = non_space( ++ _cur, _end );
		values_.push_back( _cache );
	}

	void save( HStreamInterface& stream_ ) {
		stream_ << "(;GM[" << static_cast<int>( _gameType ) << "]FF[4]AP[" << _app << "]\n"
			<< "SZ[" << _game._gobanSize << "]KM[" << setw( 1 ) << _game._komi << "]TM[" << _game._time << "]\n"
			<< "PB[" << _game._blackName << "]PW[" << _game._whiteName << "]\n"
			<< "BR[" << _game._blackRank << "]WR[" << _game._whiteRank << "]\n";
		if ( ! _game._comment.is_empty() ) {
			_cache = _game._comment;
			_cache.replace( "[", "\\[" ).replace( "]", "\\]" );
			stream_ << "C[" << _cache << "]";
		}
		if ( ! _game._blackPreset.empty() ) {
			stream_ << "AB";
			for ( Game.Vector<Move>.const_iterator it( _game._blackPreset.begin() ), end( _game._blackPreset.end() ); it != end; ++ it )
				stream_ << '[' << it.coord() << ']';
		}
		if ( ! _game._whitePreset.empty() ) {
			stream_ << "AW";
			for ( Game.Vector<Move>.const_iterator it( _game._whitePreset.begin() ), end( _game._whitePreset.end() ); it != end; ++ it )
				stream_ << '[' << it.coord() << ']';
		}
		save_variations( _game._firstToMove, _game._tree.get_root(), stream_ );
		stream_ << ")" << endl;
		return;
	}

	void save_move( Player.player_t of_, HTree<Move>.const_node_t node_, HStreamInterface& stream_ ) {
		stream_ << ';' << ( of_ == Player.BLACK ? 'B' : 'W' ) << '[' << (**node_).coord() << ']';
		if ( ! (**node_)._comment.is_empty() ) {
			_cache = (**node_)._comment;
			_cache.replace( "[", "\\[" ).replace( "]", "\\]" );
			stream_ << "C[" << _cache << "]";
		}
		return;
	}

	void save_variations( Player.player_t from_, HTree<Move>.const_node_t node_, HStreamInterface& stream_ ) {
		int childCount( 0 );
		while ( ( childCount = static_cast<int>( node_.child_count() ) ) == 1 ) {
			node_ = &*node_.begin();
			save_move( from_, node_, stream_ );
			from_ = ( from_ == Player.BLACK ? Player.WHITE : Player.BLACK );
		}
		if ( childCount > 1 ) /* We have variations. */ {
			for ( HTree<Move>.const_iterator it( node_.begin() ), end( node_.end() ); it != end; ++ it ) {
				stream_ << endl << '(';
				save_move( from_, &*it, stream_ );
				save_variations( ( from_ == Player.BLACK ? Player.WHITE : Player.BLACK ), &*it, stream_ );
				stream_ << ')' << endl;
			}
		}
		return;
	}

	void add_stone( Player.player_t player_, int col_, int row_ ) {
		_game.add_stone( player_, col_, row_ );
		return;
	}

}

