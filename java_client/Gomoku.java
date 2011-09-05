import java.util.SortedMap;
import java.util.TreeMap;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.DefaultListModel;
import java.util.Date;

class GomokuPlayer {
	public JLabel _name;
	public JButton _sit;
	public void clear() {
		_name.setText( "" );
		_sit.setText( Gomoku.HGUILocal.SIT );
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
		public static final String SETUP = "setup";
		public static final String ADMIN = "admin";
		public static final String CONTESTANT = "contestant";
		public static final String SIT = "sit";
		public static final String GETUP = "get_up";
		public static final String PUTSTONE = "put_stone";
		public static final String STONES = "stones";
		public static final String STONE = "stone";
		public static final String TOMOVE = "to_move";
		public static final String PLAYER = "player";
		public static final String PLAYERQUIT = "player_quit";
	}
	public static final int GOBAN_SIZE = 15;
	public static final class STONE extends Goban.STONE {
		public static final byte WIN_BLACK = 'p';
		public static final byte WIN_WHITE = 'q';
		public static final byte INVALID = 'N';
	}
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public static final String SIT = "Sit !";
		public static final String GETUP = "Get up !";
		public JTextField _messageInput;
		public JTextField _wordInput;
		public JTextPane _logPad;
		public JLabel _blackName;
		public JLabel _blackScore;
		public JButton _blackSit;
		public JLabel _whiteName;
		public JLabel _whiteScore;
		public JButton _whiteSit;
		public JTable _champions;
		public JList _visitors;
		public GomokuGoban _board;
		public DummyConfigurator _conf;
		public JLabel _toMove;
		public JLabel _move;
		public String _toolTip;
		public String _passText;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "goban", GomokuGoban.class );
			$xul.getTaglib().registerTag( "setup", DummyConfigurator.class );
		}
		public void init() {
			super.init();
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
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( ( _stone == Gomoku.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEPP + STONE.BLACK : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _stone != Gomoku.STONE.NONE );
			_whiteSit.setEnabled( _stone != Gomoku.STONE.NONE );
		}
		public void onWhite() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( ( _stone == Gomoku.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEPP + STONE.WHITE : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _stone != Gomoku.STONE.NONE );
			_whiteSit.setEnabled( _stone != Gomoku.STONE.NONE );
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
	private SortedMap<Byte, GomokuPlayer> _contestants = java.util.Collections.synchronizedSortedMap( new TreeMap<Byte, GomokuPlayer>() );
//--------------------------------------------//
	public Gomoku( GameGround $applet, String $id, String $configuration ) throws Exception {
		super( $applet, $id, $configuration );
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( PROTOCOL.STONES, Gomoku.class.getDeclaredMethod( "handlerStones", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYER, Gomoku.class.getDeclaredMethod( "handlerPlayer", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.TOMOVE, Gomoku.class.getDeclaredMethod( "handlerToMove", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.CONTESTANT, Gomoku.class.getDeclaredMethod( "handlerContestant", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, Gomoku.class.getDeclaredMethod( "handlerPlayerQuit", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.ADMIN, HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.SETUP, HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		GoImages images = new GoImages();
		//_gui._board.setGui( this );
		_gui._board.setImages( images );
		GomokuPlayer black = new GomokuPlayer();
		black._name = _gui._blackName;
		black._sit = _gui._blackSit;
		_contestants.put( STONE.BLACK, black );
		GomokuPlayer white = new GomokuPlayer();
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
		_app.registerTask( this, 1 );
	}
	void handlerStones( String $command ) {
		_gui._board.setStones( ( _stones = $command ).getBytes() );
		_gui._board.repaint();
	}
	void handlerContestant( String $command ) {
		String[] tokens = $command.split( ",", 6 );
		byte stone = STONE.NONE;
		GomokuPlayer contestant = _contestants.get( ( stone = (byte)tokens[ 0 ].charAt( 0 ) ) );
		contestant._name.setText( tokens[ 1 ] );
		if ( tokens[ 1 ].equals( _app.getName() ) ) {
			_stone = stone;
			contestant._sit.setText( HGUILocal.GETUP );
			contestant._sit.setEnabled( true );
			GomokuPlayer foe = _contestants.get( _gui._board.opponent( _stone ) );
			foe._sit.setEnabled( false );
		} else {
			if ( ( stone == _stone ) && ( _stone != STONE.NONE ) ) {
				contestant._sit.setText( HGUILocal.SIT );
				GomokuPlayer foe = _contestants.get( _gui._board.opponent( _stone ) );
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
	void handlerPlayer( String $command ) {
		DefaultListModel m = (DefaultListModel)_gui._visitors.getModel();
		m.addElement( $command );
	}
	void handlerPlayerQuit( String $command ) {
		DefaultListModel m = (DefaultListModel)_gui._visitors.getModel();
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

