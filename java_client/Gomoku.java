import java.util.ArrayList;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Comparator;
import java.util.Date;
import javax.swing.table.TableRowSorter;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.DefaultListModel;
import javax.swing.table.AbstractTableModel;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;

class GomokuContestant {
	public JLabel _name;
	public JButton _sit;
	public void clear() {
		_name.setText( "" );
		_sit.setText( Gomoku.HGUILocal.SIT );
	}
}

class GomokuPlayer {
	String _name;
	int _score = 0;
	GomokuPlayer( String $name ) { this( $name, 0 ); }
	GomokuPlayer( String $name, int $score ) {
		_name = $name;
		_score = $score;
	}
	void set( String $name, int $score ) {
		_name = $name;
		_score = $score;
	}
	String getName() {
		return ( _name );
	}
	String get( int $field ) {
		String field = "";
		switch ( $field ) {
			case ( 0 ):
				field = _name;
			break;
			case ( 1 ):
				field += _score;
			break;
		}
		return ( field );
	}
}

class Gomoku extends HAbstractLogic implements Runnable {
	public static final class PROTOCOL {
		public static final String SEP = ":";
		public static final String SEPP = ",";
		public static final String CMD = "cmd";
		public static final String SAY = "say";
		public static final String NAME = "gomoku";
		public static final String PLAY = "play";
		public static final String PASS = "pass";
		public static final String CONTESTANT = "contestant";
		public static final String SIT = "sit";
		public static final String SGF = "sgf";
		public static final String GETUP = "get_up";
		public static final String PUTSTONE = "put_stone";
		public static final String STONE = "stone";
		public static final String TOMOVE = "to_move";
		public static final String PLAYER = "player";
		public static final String SPECTATOR = "spectator";
		public static final String FIVE_IN_A_ROW = "five_in_a_row";
		public static final String PLAYERQUIT = "player_quit";
	}
	public static final int GOBAN_SIZE = 15;
	public static final class STONE extends Goban.STONE {
	}
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public static final String SIT = "Sit!";
		public static final String GETUP = "Get up!";
		public JTextField _messageInput;
		public JTextField _wordInput;
		public JTextPane _logPad;
		public JLabel _blackName;
		public JLabel _blackScore;
		public JButton _blackSit;
		public JLabel _whiteName;
		public JLabel _whiteScore;
		public JButton _whiteSit;
		public JTable _players;
		public JList _spectators;
		public GomokuGoban _board;
		public DummyConfigurator _conf;
		public JLabel _toMove;
		public JLabel _move;
		public String _toolTip;
		public String _passText;
		final String[] _header = { "Player", "Score" };
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "goban", GomokuGoban.class );
		}
		public void init() {
			super.init();
			AbstractTableModel model = new AbstractTableModel() {
				public static final long serialVersionUID = 17l;
				public String getColumnName(int col) {
					return ( _header[ col ] );
				}
				public int getRowCount() { return Gomoku.this._players.size(); }
				public int getColumnCount() { return _header.length; }
				public Object getValueAt(int row, int col) {
					return Gomoku.this._players.get( row ).get( col );
				}
				public boolean isCellEditable(int row, int col)
					{ return false; }
				public void setValueAt(Object value, int row, int col) {
				}
			};
			Comparator<String> comparator = new Comparator<String>() {
				public int compare( String s1, String s2 ) {
					return ( Integer.parseInt( s2 ) - Integer.parseInt( s1 ) );
				}
			};
			TableRowSorter<AbstractTableModel> rs = new TableRowSorter<AbstractTableModel>();
			_players.setModel( model );
			rs.setModel( model );
			rs.setComparator( 1, comparator );
			_players.setRowSorter( rs );
			_players.setShowGrid( false );
			_players.setAutoCreateRowSorter( true );
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public HAbstractConfigurator getConfigurator() {
			return ( _conf );
		}
		public void onMessage() {
			String msg = _messageInput.getText();
			if ( msg.matches( ".*\\S+.*" ) ) {	
				_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP + PROTOCOL.SAY + PROTOCOL.SEP + msg );
				_messageInput.setText( "" );
			}
		}
		public void onExit() {
			_app.closeParty( Gomoku.this );
		}
		public void onBlack() {
			_board.reset();
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( ( _stone == Gomoku.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEPP + (char)STONE.BLACK : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _stone != Gomoku.STONE.NONE );
			_whiteSit.setEnabled( _stone != Gomoku.STONE.NONE );
		}
		public void onWhite() {
			_board.reset();
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( ( _stone == Gomoku.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEPP + (char)STONE.WHITE : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _stone != Gomoku.STONE.NONE );
			_whiteSit.setEnabled( _stone != Gomoku.STONE.NONE );
		}
		public void onLoad() {
			JFileChooser fc = new JFileChooser();
			fc.setAcceptAllFileFilterUsed( false );
			fc.setFileFilter( new FileNameExtensionFilter( "Smart Game Format", "sgf" ) );
			fc.showOpenDialog( this );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "gomoku";
	public HGUILocal _gui;
	private byte _stone = STONE.NONE;
	private byte _toMove = STONE.NONE;
	private long _start = 0;
	private boolean _admin = false;
	private int _move = 0;
	private String _stones = null;
	private ArrayList<GomokuPlayer> _players = new ArrayList<GomokuPlayer>();
	private SortedMap<Byte, GomokuContestant> _contestants = java.util.Collections.synchronizedSortedMap( new TreeMap<Byte, GomokuContestant>() );
//--------------------------------------------//
	public Gomoku( GameGround $applet, String $id, String $configuration ) throws Exception {
		super( $applet, $id, $configuration );
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( PROTOCOL.PLAYER, Gomoku.class.getDeclaredMethod( "handlerPlayer", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.SPECTATOR, Gomoku.class.getDeclaredMethod( "handlerSpectator", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.TOMOVE, Gomoku.class.getDeclaredMethod( "handlerToMove", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.CONTESTANT, Gomoku.class.getDeclaredMethod( "handlerContestant", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, Gomoku.class.getDeclaredMethod( "handlerPlayerQuit", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.FIVE_IN_A_ROW, Gomoku.class.getDeclaredMethod( "handlerFiveInARow", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.SGF, Gomoku.class.getDeclaredMethod( "handlerSGF", new Class[]{ String.class } ) );
		GoImages images = new GoImages();
		_gui._board.setGui( this );
		_gui._board.setImages( images );
		GomokuContestant black = new GomokuContestant();
		black._name = _gui._blackName;
		black._sit = _gui._blackSit;
		_contestants.put( STONE.BLACK, black );
		GomokuContestant white = new GomokuContestant();
		white._name = _gui._whiteName;
		white._sit = _gui._whiteSit;
		_contestants.put( STONE.WHITE, white );
		_gui._board.setSize( GOBAN_SIZE );
		/* Was in init(). */
		_client = _app.getClient();
		_contestants.get( STONE.BLACK ).clear();
		_contestants.get( STONE.WHITE ).clear();
		_stone = STONE.NONE;
		_toMove = STONE.NONE;
		_admin = false;
		_move = 0;
	}
	void handlerContestant( String $command ) {
		String[] tokens = $command.split( ",", 6 );
		byte stone = STONE.NONE;
		GomokuContestant contestant = _contestants.get( ( stone = (byte)tokens[ 0 ].charAt( 0 ) ) );
		contestant._name.setText( tokens[ 1 ] );
		if ( tokens[ 1 ].equals( _app.getName() ) ) {
			_stone = stone;
			contestant._sit.setText( HGUILocal.GETUP );
			contestant._sit.setEnabled( true );
			GomokuContestant foe = _contestants.get( _gui._board.opponent( _stone ) );
			foe._sit.setEnabled( false );
		} else {
			if ( ( stone == _stone ) && ( _stone != STONE.NONE ) ) {
				contestant._sit.setText( HGUILocal.SIT );
				GomokuContestant foe = _contestants.get( _gui._board.opponent( _stone ) );
				foe._sit.setEnabled( "".equals( foe._name.getText() ) );
				_stone = STONE.NONE;
			}
			contestant._sit.setEnabled( "".equals( tokens[ 1 ] ) && ( _stone == STONE.NONE ) );
		}
		_start = new Date().getTime();
	}
	void handlerToMove( String $command ) {
		_toMove = (byte)$command.charAt( 0 );
		++ _move;
		String toMove = STONE.NONE_NAME;
		if ( _toMove == STONE.BLACK )
			toMove = STONE.BLACK_NAME;
		else if ( _toMove == STONE.WHITE )
			toMove = STONE.WHITE_NAME;
		else
			_move = 0;
		_gui._move.setText( "" + _move );
		_gui._toMove.setText( toMove );
	}
	void handlerFiveInARow( String $command ) {
		_toMove = STONE.NONE;
		String[] tokens = $command.split( ",", 4 );
		try {
			int row = Integer.parseInt( tokens[1] );
			int col = Integer.parseInt( tokens[2] );
			int direction = Integer.parseInt( tokens[3] );
			_gui._board.fiveInARow( (byte)tokens[0].charAt( 0 ), row, col, direction );
		} catch ( Exception e ) {
			CallStack.print();
			System.exit( 1 );
		}
	}
	void handlerPlayer( String $command ) {
		String[] tokens = $command.split( ",", 2 );
		GomokuPlayer p = null;
		for ( int i = 0; i < _players.size(); ++ i ) {
			if ( _players.get( i ).get( 0 ).compareToIgnoreCase( tokens[ 0 ] ) == 0 ) {
				p = _players.get( i );
				break;
			}
		}
		if ( p == null ) {
			p = new GomokuPlayer( tokens[ 0 ] );
			_players.add( p );
		}
		p.set( tokens[0], Integer.parseInt( tokens[ 1 ] ) );
		((AbstractTableModel)_gui._players.getModel()).fireTableDataChanged();
	}
	void handlerSGF( String $command ) {
		if ( ( _toMove == STONE.BLACK ) || ( _toMove == STONE.WHITE ) )
			Sound.play( "stone" );
		_gui._board.updateSGF( $command );
		_gui._board.repaint();
	}
	void handlerSpectator( String $command ) {
		DefaultListModel m = (DefaultListModel)_gui._spectators.getModel();
		m.addElement( $command );
	}
	void handlerPlayerQuit( String $command ) {
		DefaultListModel m = (DefaultListModel)_gui._spectators.getModel();
		m.removeElement( $command );
	}
	public boolean isMyMove() {
		return ( ( _stone != STONE.NONE ) && ( _stone == _toMove ) );
	}
	public byte stone() {
		return ( _stone );
	}
	public byte toMove() {
		return ( _toMove );
	}
	public void waitToMove() {
		_toMove = STONE.WAIT;
	}
	public void cleanup() {
		_app.flush( this );
	}
	public void run() {
	}
	static HAbstractLogic create( GameGround $app, String $id, String $configuration ) {
		HAbstractLogic logic = null;
		try {
			logic = new Gomoku( $app, $id, $configuration );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( logic );
	}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( "gomoku", new HLogicInfo( "gomoku", "gomoku", "Gomoku", null, Gomoku.class.getDeclaredMethod( "create", new Class[] { GameGround.class, String.class, String.class } ) ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

