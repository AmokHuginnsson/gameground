import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.PrintStream;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Map;

class SGFException extends Exception {
	public static final long serialVersionUID = 17l;
	int _code = 0;
	public SGFException( String $message, int $code ) {
		super( $message );
		_code = $code;
	}
	public SGFException( String $message ) {
		super( $message );
	}
	public String getMessage() {
		return ( super.getMessage() + ": " + _code );
	}
}

public class SGF {
	public enum GAME_TYPE {
		GO( 1 ),
		GOMOKU( 4 );
		private final int _gameType;
		GAME_TYPE( int $gameType ) {
			_gameType = $gameType;
		}
		int value() {
			return ( _gameType );
		}
	}
	public enum ERROR {
		UNEXPECTED_EOF,
		UNEXPECTED_DATA,
		GT_OPEN_EXPECTED,
		GT_CLOSE_EXPECTED,
		PROP_IDENT_EXPECTED,
		PROP_VAL_OPEN_EXPECTED,
		PROP_VAL_CLOSE_EXPECTED,
		NODE_MARK_EXPECTED,
		BAD_GAME_TYPE,
		BAD_FILE_FORMAT,
		MIXED_NODE,
		DUPLICATED_COORDINATE
	}
	private class TERM {
		static final char GT_OPEN = '(';
		static final char GT_CLOSE = ')';
		static final char PROP_VAL_OPEN = '[';
		static final char PROP_VAL_CLOSE = ']';
		static final char NODE_MARK = ';';
		static final char ESCAPE = '\\';
	}
	enum Player {
		BLACK,
		WHITE,
		UNSET
	}
	public enum Position {
		REMOVE,
		BLACK,
		WHITE,
		TRIANGLE,
		SQUARE,
		CIRCLE,
		MARK
	}
	static class Coord {
		char[] _data = { 0, 0 };
		Coord( int $col, int $row ) {
			_data[0] = (char)( $col + 'a' );
			_data[1] = (char)( $row + 'a' );
		}
		Coord( final String $coord ) {
			set( $coord );
		}
		void set( final String $coord ) {
			if ( $coord.length() >= 2 ) {
				_data[0] = $coord.charAt( 0 );
				_data[1] = $coord.charAt( 1 );
			} else
				_data[0] = _data[1] = 0;
		}
		int col() {
			return ( _data[0] - 'a' );
		}
		int row() {
			return ( _data[1] - 'a' );
		}
		String data() {
			return ( new String( _data ) );
		}
		@Override public boolean equals( Object $coord ) {
			Coord coord = (Coord)$coord;
			return ( ( _data[0] == coord._data[0] ) && ( _data[1] == coord._data[1] ) );
		}
	};
	static class Setup {
		private SortedMap<Position, ArrayList<Coord>> _data = java.util.Collections.synchronizedSortedMap( new TreeMap<Position, ArrayList<Coord>>() );
		private void addPosition( Position $position, Coord $coord ) throws SGFException {
			if ( $position == Position.REMOVE ) {
				for ( Map.Entry<Position, ArrayList<Coord>> e : _data.entrySet() ) {
					if ( e.getKey() == Position.REMOVE )
						continue;
					e.getValue().remove( $coord );
				}
			} else {
				for ( Map.Entry<Position, ArrayList<Coord>> e : _data.entrySet() ) {
					if ( e.getKey() == Position.REMOVE )
						continue;
					ArrayList<Coord> c = e.getValue();
					if ( c.contains( $coord ) )
						throw new SGFException( _errMsg_[ERROR.DUPLICATED_COORDINATE.ordinal()], $coord.row() * 100 + $coord.col() );
				}
			}
			ArrayList<Coord> c = _data.get( $position );
			if ( ( $position != Position.REMOVE ) || ( ! c.contains( $coord ) ) )
				c.add( $coord );
			return;
		}
	}
	static class Move {
		public enum Type {
			INVALID,
			MOVE,
			SETUP
		}
		Type _type = Type.INVALID;
		Coord _coord = null;
		Setup _setup = null;
		String _comment = "";
		Move() { }
		Move( int $col, int $row ) {
			_coord = new Coord( $col, $row );
			_type = Type.MOVE;
		}
		Move( final String $coord ) {
			_coord = new Coord( $coord );
			_type = Type.MOVE;
		}
		void setCoord( Coord $coord ) throws SGFException {
			if ( _type == Type.SETUP )
				throw new SGFException( _errMsg_[ERROR.MIXED_NODE.ordinal()] );
			_coord = $coord;
		}
		void addPosition( Position $position, Coord $coord ) throws SGFException {
			if ( ( $position == Position.REMOVE )
					|| ( $position == Position.BLACK )
					|| ( $position == Position.WHITE ) ) {
				if ( _type == Type.MOVE )
					throw new SGFException( _errMsg_[ERROR.MIXED_NODE.ordinal()] );
				_type = Type.SETUP;
			}
			_setup.addPosition( $position, $coord );
		}
		String coord() {
			return ( _coord.data() );
		}
		int col() {
			return ( _coord.col() );
		}
		int row() {
			return ( _coord.row() );
		}
	}
	static final int RESIGN = 0xffff;
	static final int TIME = 0x7fff;
	String _app = null;
	String _rules = "";
	String _blackName = "";
	String _whiteName = "";
	String _blackRank = "30k";
	String _whiteRank = "30k";
	ArrayList<Move> _blackPreset = new ArrayList<Move>();
	ArrayList<Move> _whitePreset = new ArrayList<Move>();
	HTree<Move> _tree = new HTree<Move>();
	Player _firstToMove = Player.UNSET;
	int _gobanSize = 19;
	int _time = 0;
	int _handicap = 0;
	double _komi = 5.5;
	int _result = 0;
	String _place = "";
	String _comment = "";
	GAME_TYPE _gameType;
	String _rawData = null;
	int _cur = 0;
	int _end = 0;
	String _cache = null;
	String _cachePropIdent = null;
	ArrayList<String> _cachePropValue = new ArrayList<String>();
	HTree<Move>.HNode<Move> _currentMove = null;

	void addStone( Player $player, int $col, int $row ) {
		addStone( $player, new Move( $col, $row ) );
		return;
	}

	void addStone( Player $player, final Move move_ ) {
		if ( $player == Player.BLACK )
			_blackPreset.add( move_ );
		else
			_whitePreset.add( move_ );
		return;
	}

	HTree<Move>.HNode<Move> move( HTree<Move>.HNode<Move> $node, int $col, int $row ) {
		return ( $node.addNode( new Move( $col, $row ) ) );
	}

	void clear() {
		_rawData = "";
		_cur = _end = 0;
		_cache = null;
		_cachePropIdent = null;
		_cachePropValue.clear();
		_blackPreset.clear();
		_whitePreset.clear();
		_currentMove = null;
		_app = "";
		_rules = "";
		_blackName = "";
		_blackRank = "";
		_whiteName = "";
		_whiteRank = "";
		_tree.clear();
		_firstToMove = Player.UNSET;
		_gobanSize = 19;
		_handicap = 0;
		_komi = 5.5;
		_result = 0;
		_place = "";
		_comment = "";
		return;
	}

	SGF( GAME_TYPE $type ) {
		this( $type, "libsgf" );
	}

	public SGF( GAME_TYPE $gameType, final String $app ) {
		_app = $app;
		_gameType = $gameType;
	}

	final static String[] _errMsg_ = {
		"Unexpected end of file.",
		"Unexpected data bytes found.",
		"Expected GameTree opening sequence.",
		"Expected GameTree closing sequence.",
		"Expected property identifier.",
		"Expected property value opening sequence.",
		"Expected property value closing sequence.",
		"Expected node start marker sequence.",
		"Bad game type.",
		"Bad file format.",
		"Cannot mix `move' with `setup' nodes.",
		"Duplicated coordinate in setup."
	};

	int nonSpace( int first, int last ) {
		final String space = " \t\n\r\b\f";
		for ( ; first != last ; ++ first ) {
			if ( space.indexOf( _rawData.charAt( first ) ) == -1 )
				break;
		}
		return ( first );
	}

	void load( BufferedReader $stream ) throws SGFException {
		clear();
		String message = null;
		try {
			while ( ( message = $stream.readLine() ) != null )
				_rawData += ( message + "\n" );
		} catch ( java.io.IOException ioe ) {
			Con.err( "java.io.IOException: " + ioe.getMessage() );
		}
		_end = _rawData.length();
		_cur = nonSpace( _cur, _end );
		try {
			parseGameTree();
			while ( _cur != _end )
				parseGameTree();
		} catch ( final SGFException e ) {
			if ( _cur != _end )
				System.out.println( "Failed at byte: `" + _rawData.charAt( _cur ) + "'" );
			Con.err( "SGFException: " + e.getMessage() );
			throw e;
		}
		return;
	}

	void notEof() throws SGFException {
		if ( _cur == _end )
			throw new SGFException( _errMsg_[ERROR.UNEXPECTED_EOF.ordinal()], _cur );
		return;
	}

	void parseGameTree() throws SGFException {
		notEof();
		if ( _rawData.charAt( _cur ) != TERM.GT_OPEN )
			throw new SGFException( _errMsg_[ERROR.GT_OPEN_EXPECTED.ordinal()], _cur );
		_cur = nonSpace( ++ _cur, _end );
		notEof();
		if ( _currentMove == null )
			_currentMove = _tree.createNewRoot( new Move() );
		parseSequence();
		HTree<Move>.HNode<Move> preVariationMove = _currentMove;
		while ( ( _cur != _end ) && ( _rawData.charAt( _cur ) != TERM.GT_CLOSE ) ) {
			_currentMove = preVariationMove.addNode( new Move() );
			parseGameTree();
			notEof();
		}
		if ( _rawData.charAt( _cur ) != TERM.GT_CLOSE )
			throw new SGFException( _errMsg_[ERROR.GT_CLOSE_EXPECTED.ordinal()], _cur );
		_cur = nonSpace( ++ _cur, _end );
		return;
	}

	void parseSequence() throws SGFException {
		parseNode();
		_cur = nonSpace( _cur, _end );
		notEof();
		while ( _rawData.charAt( _cur ) == TERM.NODE_MARK ) {
			_currentMove = _currentMove.addNode( new Move() );
			parseNode();
			_cur = nonSpace( _cur, _end );
			notEof();
		}
		return;
	}

	void parseNode() throws SGFException {
		if ( _rawData.charAt( _cur ) != TERM.NODE_MARK )
			throw new SGFException( _errMsg_[ERROR.NODE_MARK_EXPECTED.ordinal()], _cur );
		_cur = nonSpace( ++ _cur, _end );
		notEof();
		while ( ( _rawData.charAt( _cur ) != TERM.GT_CLOSE ) && ( _rawData.charAt( _cur ) != TERM.GT_OPEN ) && ( _rawData.charAt( _cur ) != TERM.NODE_MARK ) ) {
			parseProperty();
			_cur = nonSpace( _cur, _end );
			notEof();
		}
		return;
	}

	void parseProperty() throws SGFException {
		_cachePropIdent = parsePropertyIdent();
		if ( "".equals( _cachePropIdent ) )
			throw new SGFException( _errMsg_[ERROR.PROP_IDENT_EXPECTED.ordinal()], _cur );
		_cur = nonSpace( _cur, _end );
		notEof();
		_cachePropValue.clear();
		parsePropertyValue( _cachePropValue );
		while ( _rawData.charAt( _cur ) == TERM.PROP_VAL_OPEN )
			parsePropertyValue( _cachePropValue );
		final String singleValue = _cachePropValue.get( 0 );
		if ( "GM".equals( _cachePropIdent ) ) {
			int val = Integer.parseInt( singleValue );
			if ( val != _gameType.value() )
				throw new SGFException( _errMsg_[ERROR.BAD_GAME_TYPE.ordinal()], _cur );
		} else if ( "FF".equals( _cachePropIdent ) ) {
			int ff = Integer.parseInt( singleValue );
			if ( ( ff < 1 ) || ( ff > 4 ) )
				throw new SGFException( _errMsg_[ERROR.BAD_FILE_FORMAT.ordinal()], _cur );
		} else if ( "AP".equals( _cachePropIdent ) )
			_app = singleValue;
		else if ( "RU".equals( _cachePropIdent ) )
			_rules = singleValue;
		else if ( "PB".equals( _cachePropIdent ) )
			_blackName = singleValue;
		else if ( "PW".equals( _cachePropIdent ) )
			_whiteName = singleValue;
		else if ( "BR".equals( _cachePropIdent ) )
			_blackRank = singleValue;
		else if ( "WR".equals( _cachePropIdent ) )
			_whiteRank = singleValue;
		else if ( "KM".equals( _cachePropIdent ) )
			_komi = Double.parseDouble( singleValue );
		else if ( "HA".equals( _cachePropIdent ) )
			_handicap = Integer.parseInt( singleValue );
		else if ( "SZ".equals( _cachePropIdent ) )
			_gobanSize = Integer.parseInt( singleValue );
		else if ( "TM".equals( _cachePropIdent ) )
			_time = Integer.parseInt( singleValue );
		else if ( "PC".equals( _cachePropIdent ) )
			_place = singleValue;
		else if ( "RE".equals( _cachePropIdent ) ) {
			if ( Character.isDigit( singleValue.charAt( 2 ) ) )
				_result = Integer.parseInt( singleValue.substring( 2 ) );
			else {
				char r = Character.toUpperCase( singleValue.charAt( 2 ) );
				if ( r == 'R' )
					_result = RESIGN;
				else if ( r == 'T' )
					_result = TIME;
			}
			char player = Character.toUpperCase( singleValue.charAt( 0 ) );
			if ( player == 'W' )
				_result = -_result;
		} else if ( "AB".equals( _cachePropIdent ) ) {
			for ( String s : _cachePropValue )
				addStone( Player.BLACK, new Move( s ) );
		} else if ( "AW".equals( _cachePropIdent ) ) {
			for ( String s : _cachePropValue )
				addStone( Player.WHITE, new Move( s ) );
		} else if ( "B".equals( _cachePropIdent ) ) {
			if ( _firstToMove == Player.UNSET )
				_firstToMove = Player.BLACK;
			_currentMove.value().setCoord( new Coord( singleValue ) );
		} else if ( "W".equals( _cachePropIdent ) ) {
			if ( _firstToMove == Player.UNSET )
				_firstToMove = Player.WHITE;
			_currentMove.value().setCoord( new Coord( singleValue ) );
		} else if ( "C".equals( _cachePropIdent ) ) {
			if ( _firstToMove != Player.UNSET )
				_currentMove.value()._comment = singleValue;
			else
				_comment += singleValue;
		} else
			System.out.println( "property: `" + _cachePropIdent + "' = `" + singleValue + "'" );
		return;
	}

	String parsePropertyIdent() {
		_cache = "";
		while ( ( _cur != _end ) && Character.isUpperCase( _rawData.charAt( _cur ) ) ) {
			_cache += _rawData.charAt( _cur );
			++ _cur;
		}
		return ( _cache );
	}

	void parsePropertyValue( ArrayList<String> values_ ) throws SGFException {
		notEof();
		if ( _rawData.charAt( _cur ) != TERM.PROP_VAL_OPEN )
			throw new SGFException( _errMsg_[ERROR.PROP_VAL_OPEN_EXPECTED.ordinal()], _cur );
		_cur = nonSpace( ++ _cur, _end );
		notEof();
		_cache = "";
		boolean escaped = false;
		while ( ( _cur != _end ) && ( escaped || ( _rawData.charAt( _cur ) != TERM.PROP_VAL_CLOSE ) ) ) {
			escaped = ( _rawData.charAt( _cur ) == TERM.ESCAPE );
			if ( ! escaped )
				_cache += _rawData.charAt( _cur );
			++ _cur;
		}
		notEof();
		if ( _rawData.charAt( _cur ) != TERM.PROP_VAL_CLOSE )
			throw new SGFException( _errMsg_[ERROR.PROP_VAL_CLOSE_EXPECTED.ordinal()], _cur );
		_cur = nonSpace( ++ _cur, _end );
		values_.add( _cache );
	}

	void save( PrintStream $stream ) {
		$stream.print( "(;GM[" + _gameType.value() + "]FF[4]AP[" + _app + "]\n"
			+ "SZ[" + _gobanSize + "]KM[" + _komi + "]TM[" + _time + "]\n"
			+ "PB[" + _blackName + "]PW[" + _whiteName + "]\n"
			+ "BR[" + _blackRank + "]WR[" + _whiteRank + "]\n" );
		if ( ! "".equals( _comment ) ) {
			_cache = _comment;
			$stream.print( "C[" + _cache.replace( "[", "\\[" ).replace( "]", "\\]" ) + "]" );
		}
		if ( ! _blackPreset.isEmpty() ) {
			$stream.print( "AB" );
			for ( Move m : _blackPreset )
				$stream.print( "[" + m.coord() + "]" );
		}
		if ( ! _whitePreset.isEmpty() ) {
			$stream.print( "AW" );
			for ( Move m : _whitePreset )
				$stream.print( "[" + m.coord() + "]" );
		}
		if ( _tree.getRoot() != null )
			saveVariations( _firstToMove, _tree.getRoot(), $stream );
		$stream.println( ")" );
		return;
	}

	void saveMove( Player of_, HTree<Move>.HNode<Move> $node, PrintStream $stream ) {
		$stream.print( ";" + ( of_ == Player.BLACK ? "B" : "W" ) + "[" + $node.value().coord() + "]" );
		if ( ! "".equals( $node.value()._comment ) ) {
			_cache = $node.value()._comment;
			$stream.print( "C[" + _cache.replace( "[", "\\[" ).replace( "]", "\\]" ) + "]" );
		}
		return;
	}

	void saveVariations( Player from_, HTree<Move>.HNode<Move> $node, PrintStream $stream ) {
		int childCount = 0;
		while ( ( childCount = $node.getChildCount() ) == 1 ) {
			$node = $node.getChildAt( 0 );
			saveMove( from_, $node, $stream );
			from_ = ( from_ == Player.BLACK ? Player.WHITE : Player.BLACK );
		}
		if ( childCount > 1 ) /* We have variations. */ {
			for ( HTree<Move>.HNode<Move> it : $node ) {
				$stream.print( "\n(" );
				saveMove( from_, it, $stream );
				saveVariations( ( from_ == Player.BLACK ? Player.WHITE : Player.BLACK ), it, $stream );
				$stream.print( ")\n" );
			}
		}
		return;
	}

	public Move lastMove() {
		HTree<Move>.HNode<Move> m = _tree.getRoot();
		while ( ( m != null ) && m.getChildAt( 0 ) != null )
			m = m.getChildAt( 0 );
		return ( m.value() );
	}

}

