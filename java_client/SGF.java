import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.PrintStream;

class SGFException extends Exception {
	int _code;
	public SGFException( String $message, int $code ) {
		super( $message );
		_code = $code;
	}
	public String toString() {
		return ( super.toString + ": " + _code );
	}
}

public class SGF {
	public enum GAME_TYPE {
		GO( 1 ),
		GOMOKU( 4 )
	}
	public enum ERROR {
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
			enum Player {
				BLACK,
				WHITE,
				UNSET
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
		ArrayList<Move> _blackPreset = new ArrayList<Move>();
		ArrayList<Move> _whitePreset = new ArrayList<move>();
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

		void addStone( Player.player_t player_, int col_, int row_ ) {
			addStone( player_, Move( col_, row_ ) );
			return;
		}

		void addStone( Player.player_t player_, final Move move_ ) {
			if ( player_ == Player.BLACK )
				_blackPreset.push_back( move_ );
			else
				_whitePreset.push_back( move_ );
			return;
		}

		HTree<Move>.HNode<Move> move( HTree<Move>.HNode<Move> node_, Player p, int col_, int row_ ) {
			return ( node_.addNode( Move( col_, row_ ) ) );
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
	GAME_TYPE _gameType;
	String _rawData = "";
	int _cur = 0;
	int _end = 0;
	String _cache;
	String _cachePropIdent;
	ArrayList<String> _cachePropValue = new ArrayList<String>();
	Game _game = new Game();
	HTree<Move>.HNode<Move> _currentMove = null;
	String _app = null;

	SGF( GAME_TYPE $type ) {
		this( $type, "libsgf" );
	}

	public SGF( GAME_TYPE $gameType, final String $app ) {
		_app = $app;
		_gameType = $gameType;
	/*
		_cache(), _cachePropIdent()
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

	int nonSpace( int first, int last ) {
		final String space = " \t\n\r\b\f";
		for ( ; first != last ; ++ first ) {
			if ( space.indexOf( _rawData.charAt( first ) ) == -1 )
				break;
		}
		return ( first );
	}

	void load( BufferedReader $stream ) {
		clear();
		String message = null;
		while ( ( message = $stream.readLine() ) != null )
			_rawData += message;
		_end = _rawData.length();
		_cur = nonSpace( _cur, _end );
		try {
			parseGameTree();
			while ( _cur != _end )
				parseGameTree();
		} catch ( final SGFException e ) {
			if ( _cur != _end )
				System.out.prinntln( "Failed at byte: `" + _rawData.charAt( _cur ) + "'" );
			throw e;
		}
		return;
	}

	void clear() {
		_cur = _end = 0;
		_currentMove = null;
		_rawData.clear();
		_game.clear();
		return;
	}

	void notEof() {
		if ( _cur == _end )
			throw new SGFException( _errMsg_[ERROR.UNEXPECTED_EOF], _cur );
		return;
	}

	void parseGameTree() {
		notEof();
		if ( _rawData.charAt( _cur ) != TERM.GT_OPEN )
			throw new SGFException( _errMsg_[ERROR.GT_OPEN_EXPECTED], _cur );
		_cur = nonSpace( ++ _cur, _end );
		notEof();
		if ( ! _currentMove )
			_currentMove = _game._tree.create_new_root();
		parseSequence();
		HTree<Move>.HNode<Move> preVariationMove = _currentMove;
		while ( ( _cur != _end ) && ( _rawData.charAt( _cur ) != TERM.GT_CLOSE ) ) {
			_currentMove = preVariationMove.addNode();
			parseGameTree();
			notEof();
		}
		if ( _rawData.charAt( _cur ) != TERM.GT_CLOSE )
			throw new SGFException( _errMsg_[ERROR.GT_CLOSE_EXPECTED], _cur );
		_cur = nonSpace( ++ _cur, _end );
		return;
	}

	void parseSequence() {
		parseNode();
		_cur = nonSpace( _cur, _end );
		notEof();
		while ( _rawData.charAt( _cur ) == TERM.NODE_MARK ) {
			_currentMove = _currentMove.addNode();
			parseNode();
			_cur = nonSpace( _cur, _end );
			notEof();
		}
		return;
	}

	void parseNode() {
		if ( _rawData.charAt( _cur ) != TERM.NODE_MARK )
			throw new SGFException( _errMsg_[ERROR.NODE_MARK_EXPECTED], _cur );
		_cur = nonSpace( ++ _cur, _end );
		notEof();
		while ( ( _rawData.charAt( _cur ) != TERM.GT_CLOSE ) && ( _rawData.charAt( _cur ) != TERM.GT_OPEN ) && ( _rawData.charAt( _cur ) != TERM.NODE_MARK ) ) {
			parseProperty();
			_cur = nonSpace( _cur, _end );
			notEof();
		}
		return;
	}

	void parseProperty() {
		_cachePropIdent = parsePropertyIdent();
		if ( _cachePropIdent.is_empty() )
			throw new SGFException( _errMsg_[ERROR.PROP_IDENT_EXPECTED], _cur );
		_cur = nonSpace( _cur, _end );
		notEof();
		_cachePropValue.clear();
		parsePropertyValue( _cachePropValue );
		while ( _rawData.charAt( _cur ) == TERM.PROP_VAL_OPEN )
			parsePropertyValue( _cachePropValue );
		final String singleValue = _cachePropValue[0];
		if ( "GM".equals( _cachePropIdent ) ) {
			if ( lexical_cast<int>( singleValue ) != _gameType )
				throw SGFException( _errMsg_[ERROR.BAD_GAME_TYPE], static_cast<int>( _cur - _beg ) );
		} else if ( "FF".equals( _cachePropIdent ) ) {
			int ff( lexical_cast<int>( singleValue ) );
			if ( ( ff < 1 ) || ( ff > 4 ) )
				throw SGFException( _errMsg_[ERROR.BAD_FILE_FORMAT], static_cast<int>( _cur - _beg ) );
		} else if ( "PB".equals( _cachePropIdent ) )
			_game._blackName = singleValue;
		else if ( "PW".equals( _cachePropIdent ) )
			_game._whiteName = singleValue;
		else if ( "BR".equals( _cachePropIdent ) )
			_game._blackRank = singleValue;
		else if ( "WR".equals( _cachePropIdent ) )
			_game._whiteRank = singleValue;
		else if ( "KM".equals( _cachePropIdent ) )
			_game._komi = lexical_cast<double>( singleValue );
		else if ( "HA".equals( _cachePropIdent ) )
			_game._handicap = lexical_cast<int>( singleValue );
		else if ( "SZ".equals( _cachePropIdent ) )
			_game._gobanSize = lexical_cast<int>( singleValue );
		else if ( "TM".equals( _cachePropIdent ) )
			_game._time = lexical_cast<int>( singleValue );
		else if ( "PC".equals( _cachePropIdent ) )
			_game._place = singleValue;
		else if ( "RE".equals( _cachePropIdent ) ) {
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
		} else if ( "AB".equals( _cachePropIdent ) ) {
			for ( String s : _cachePropValue )
				_game.addStone( Player.BLACK, new Move( s ) );
		} else if ( "AW".equals( _cachePropIdent ) ) {
			for ( String s : _cachePropValue )
				_game.addStone( Player.WHITE, new Move( s ) );
		} else if ( "B".equals( _cachePropIdent ) ) {
			if ( _game._firstToMove == Player.UNSET )
				_game._firstToMove = Player.BLACK;
			_currentMove.coord( singleValue );
		} else if ( "W".equals( _cachePropIdent ) ) {
			if ( _game._firstToMove == Player.UNSET )
				_game._firstToMove = Player.WHITE;
			_currentMove.coord( singleValue );
		} else if ( "C".equals( _cachePropIdent ) ) {
			if ( _game._firstToMove != Player.UNSET )
				_currentMove._comment = singleValue;
			else
				_game._comment += singleValue;
		} else
			System.out.println( "property: `" + _cachePropIdent + "' = `" + singleValue + "'" );
		return;
	}

	String parsePropertyIdent() {
		_cache.clear();
		while ( ( _cur != _end ) && isupper( _rawData.charAt( _cur ) ) ) {
			_cache += _rawData.charAt( _cur );
			++ _cur;
		}
		return ( _cache );
	}

	void parsePropertyValue( ArrayList<String> values_ ) {
		notEof();
		if ( _rawData.charAt( _cur ) != TERM.PROP_VAL_OPEN )
			throw new SGFException( _errMsg_[ERROR.PROP_VAL_OPEN_EXPECTED], _cur );
		_cur = nonSpace( ++ _cur, _end );
		notEof();
		_cache.clear();
		bool escaped( false );
		while ( ( _cur != _end ) && ( escaped || ( _rawData.charAt( _cur ) != TERM.PROP_VAL_CLOSE ) ) ) {
			escaped = ( _rawData.charAt( _cur ) == TERM.ESCAPE );
			if ( ! escaped )
				_cache += _rawData.charAt( _cur );
			++ _cur;
		}
		notEof();
		if ( _rawData.charAt( _cur ) != TERM.PROP_VAL_CLOSE )
			throw new SGFException( _errMsg_[ERROR.PROP_VAL_CLOSE_EXPECTED], _cur );
		_cur = nonSpace( ++ _cur, _end );
		values_.push_back( _cache );
	}

	void save( PrintStream $stream ) {
		$stream.write( "(;GM[" + _gameType + "]FF[4]AP[" + _app + "]\n"
			+ "SZ[" + _game._gobanSize + "]KM[" + setw( 1 ) + _game._komi + "]TM[" + _game._time + "]\n"
			+ "PB[" + _game._blackName + "]PW[" + _game._whiteName + "]\n"
			+ "BR[" + _game._blackRank + "]WR[" + _game._whiteRank + "]\n" );
		if ( ! "".equals( _game._comment ) ) {
			_cache = _game._comment;
			_cache.replace( "[", "\\[" ).replace( "]", "\\]" );
			$stream << "C[" << _cache << "]";
		}
		if ( ! _game._blackPreset.empty() ) {
			$stream << "AB";
			for ( Move m : _game._blackPreset )
				$stream << '[' << m.coord() << ']';
		}
		if ( ! _game._whitePreset.empty() ) {
			$stream << "AW";
			for ( Move m : _game._whitePreset )
				$stream << '[' << m.coord() << ']';
		}
		saveVariations( _game._firstToMove, _game._tree.get_root(), $stream );
		$stream << ")" << endl;
		return;
	}

	void saveMove( Player of_, HTree<Move>.HNode<Move> node_, PrintStream $stream ) {
		$stream << ';' << ( of_ == Player.BLACK ? 'B' : 'W' ) << '[' << (**node_).coord() << ']';
		if ( ! (**node_)._comment.is_empty() ) {
			_cache = (**node_)._comment;
			_cache.replace( "[", "\\[" ).replace( "]", "\\]" );
			$stream << "C[" << _cache << "]";
		}
		return;
	}

	void saveVariations( Player from_, HTree<Move>.HNode<Move> node_, PrintStream $stream ) {
		int childCount( 0 );
		while ( ( childCount = static_cast<int>( node_.child_count() ) ) == 1 ) {
			node_ = &*node_.begin();
			saveMove( from_, node_, $stream );
			from_ = ( from_ == Player.BLACK ? Player.WHITE : Player.BLACK );
		}
		if ( childCount > 1 ) /* We have variations. */ {
			for ( HTree<Move>.HNode<Move> it : node_ ) {
				$stream << endl << '(';
				saveMove( from_, it, $stream );
				saveVariations( ( from_ == Player.BLACK ? Player.WHITE : Player.BLACK ), it, $stream );
				$stream << ')' << endl;
			}
		}
		return;
	}

	void addStone( Player.player_t player_, int col_, int row_ ) {
		_game.addStone( player_, col_, row_ );
		return;
	}

}

