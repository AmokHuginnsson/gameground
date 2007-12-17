import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.lang.reflect.Method;
import java.net.URL;
import java.util.HashMap;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Vector;
import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JList;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.table.AbstractTableModel;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.Style;
import java.util.Comparator;
import org.swixml.SwingEngine;
import javax.swing.table.TableRowSorter;
import javax.swing.RowSorter;
import javax.swing.DefaultListModel;
import java.util.Collections;
import java.util.Map;
import java.util.Date;
import java.text.SimpleDateFormat;
import java.util.Calendar;

class GoPlayer {
	public JLabel _name;
	public JLabel _score;
	public JLabel _captures;
	public JLabel _timeLeftLabel;
	public JLabel _byoyomi;
	public JButton _sit;
	public int _timeLeft = 0;
	public void clear() {
		_name.setText( "" );
		_score.setText( "" );
		_captures.setText( "" );
		_timeLeftLabel.setText( "" );
		_byoyomi.setText( "" );
		_sit.setText( Go.HGUILocal.SIT );
	}
}

class Go extends HAbstractLogic implements Runnable {
	public static final class PROTOCOL {
		public static final String SEP = ":";
		public static final String SEPP = ",";
		public static final String CMD = "cmd";
		public static final String SAY = "say";
		public static final String NAME = "go";
		public static final String PLAY = "play";
		public static final String PASS = "pass";
		public static final String SETUP = "setup";
		public static final String ADMIN = "admin";
		public static final String KOMI = "komi";
		public static final String HANDICAPS = "handicaps";
		public static final String CONTESTANT = "contestant";
		public static final String MAINTIME = "maintime";
		public static final String GOBAN = "goban";
		public static final String SIT = "sit";
		public static final String GETUP = "get_up";
		public static final String BYOYOMITIME = "byoyomitime";
		public static final String BYOYOMIPERIODS = "byoyomiperiods";
		public static final String PUTSTONE = "put_stone";
		public static final String STONES = "stones";
		public static final String STONE = "stone";
		public static final String TOMOVE = "to_move";
		public static final String PLAYER = "player";
		public static final String PLAYERQUIT = "player_quit";
		public static final String DEAD = "dead";
		public static final String ACCEPT = "accept";
		public static final String PREFIX = PROTOCOL.CMD + PROTOCOL.SEP + PROTOCOL.NAME + PROTOCOL.SEP;
	}
	public static final class GOBAN_SIZE {
		public static final int NORMAL = 19;
	}
	public static final class STONE {
		public static final char BLACK = 'b';
		public static final char WHITE = 'w';
		public static final char NONE	= ' ';
		public static final char MARK = 'm';
		public static final char DEAD_BLACK = 's';
		public static final char DEAD_WHITE = 't';
		public static final char TERITORY_BLACK = 'p';
		public static final char TERITORY_WHITE = 'q';
		public static final char DAME = 'x';
		public static final char INVALID = 'N';
		public static final char WAIT = 'Z';
		public static final String NONE_NAME = "None";
		public static final String BLACK_NAME = "Black";
		public static final String WHITE_NAME = "White";
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
		public JLabel _blackCaptures;
		public JLabel _blackTimeLeft;
		public JLabel _blackByoYomiLeft;
		public JButton _blackSit;
		public JLabel _whiteName;
		public JLabel _whiteScore;
		public JLabel _whiteCaptures;
		public JLabel _whiteTimeLeft;
		public JLabel _whiteByoYomiLeft;
		public JButton _whiteSit;
		public JButton _pass;
		public JList _visitors;
		public Goban _board;
		public GoConfigurator _conf;
		public JLabel _toMove;
		public JLabel _move;
		public String _toolTip;
		public String _passText;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "goban", Goban.class );
			$xul.getTaglib().registerTag( "panel", GoConfigurator.class );
		}
		public void init() {
			super.init();
			_toolTip = _pass.getToolTipText();
			_passText = _pass.getText();
		}
		public void reinit() {
			clearLog();
			_conf.setOwner( this );
			_conf.gobanModel();
			((DefaultListModel)_visitors.getModel()).clear();
			_conf.setEnabled( false );
			_pass.setText( _passText );
			_pass.setToolTipText( _toolTip );
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
				_client.println( PROTOCOL.PREFIX + PROTOCOL.SAY + PROTOCOL.SEP + msg );
				_messageInput.setText( "" );
			}
		}
		public void onExit() {
			_app.setFace( HBrowser.LABEL );
		}
		public void onGobanSize() {
			if ( _conf.eventsIgnored() )
				return;
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.GOBAN + PROTOCOL.SEPP
					+ (String)_conf._confGoban.getSelectedItem() );
		}
		public void onKomi() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.KOMI + PROTOCOL.SEPP
					+ _conf._confKomi.getValue() );
		}
		public void onHandicaps() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.HANDICAPS + PROTOCOL.SEPP
					+ _conf._confHandicaps.getValue() );
		}
		public void onMainTime() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.MAINTIME + PROTOCOL.SEPP
					+ _conf._confMainTime.getValue() );
		}
		public void onByoyomiPeriods() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.BYOYOMIPERIODS + PROTOCOL.SEPP
					+ _conf._confByoYomiPeriods.getValue() );
		}
		public void onByoyomiTime() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.BYOYOMITIME + PROTOCOL.SEPP
					+ _conf._confByoYomiTime.getValue() );
		}
		public void onBlack() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( ( _stone == Go.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEPP + STONE.BLACK : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _stone != Go.STONE.NONE );
			_whiteSit.setEnabled( _stone != Go.STONE.NONE );
		}
		public void onWhite() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( ( _stone == Go.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEPP + STONE.WHITE : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _stone != Go.STONE.NONE );
			_whiteSit.setEnabled( _stone != Go.STONE.NONE );
		}
		public void onPass() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( Go.this._toMove != STONE.MARK ? PROTOCOL.PASS : PROTOCOL.ACCEPT ) );
			_pass.setEnabled( false );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "go";
	public HGUILocal _gui;
	public HClient _client;
	private char _stone = STONE.NONE;
	private char _toMove = STONE.NONE;
	private long _start = 0;
	private boolean _admin = false;
	private int _move = 0;
	private String _stones = null;
	private SortedMap<Character,GoPlayer> _contestants = java.util.Collections.synchronizedSortedMap( new TreeMap<Character,GoPlayer>() );
//--------------------------------------------//
	public Go( GameGround $applet ) throws Exception {
		super( $applet );
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( PROTOCOL.NAME, Go.class.getDeclaredMethod( "handlerGo", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.SETUP, Go.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.STONES, Go.class.getDeclaredMethod( "handlerStones", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYER, Go.class.getDeclaredMethod( "handlerPlayer", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.TOMOVE, Go.class.getDeclaredMethod( "handlerToMove", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.STONE, Go.class.getDeclaredMethod( "handlerStone", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.CONTESTANT, Go.class.getDeclaredMethod( "handlerContestant", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, Go.class.getDeclaredMethod( "handlerPlayerQuit", new Class[]{ String.class } ) );
		_info = new HLogicInfo( "go", "go", "Go" );
		GoImages images = new GoImages();
		_gui._board.setGui( this );
		_gui._board.setImages( images );
		GoPlayer black = new GoPlayer();
		black._name = _gui._blackName;
		black._captures = _gui._blackCaptures;
		black._score = _gui._blackScore;
		black._timeLeftLabel = _gui._blackTimeLeft;
		black._byoyomi = _gui._blackByoYomiLeft;
		black._sit = _gui._blackSit;
		_contestants.put( new Character( STONE.BLACK ), black );
		GoPlayer white = new GoPlayer();
		white._name = _gui._whiteName;
		white._captures = _gui._whiteCaptures;
		white._score = _gui._whiteScore;
		white._timeLeftLabel = _gui._whiteTimeLeft;
		white._byoyomi = _gui._whiteByoYomiLeft;
		white._sit = _gui._whiteSit;
		_contestants.put( new Character( STONE.WHITE ), white );
	}
	void handlerGo( String $command ) {
		processMessage( $command );
	}
	void handlerSetup( String $command ) {
		String[] tokens = $command.split( ",", 2 );
		if ( PROTOCOL.ADMIN.equals( $command ) ) {
			_gui._conf.setEnabled( _admin = true );
		} else {
			int value = Integer.parseInt( tokens[ 1 ] );
			if ( PROTOCOL.GOBAN.equals( tokens[ 0 ] ) ) {
				_gui._conf.selectGobanSize( tokens[ 1 ] );
				_gui._board.setSize( value );
			} else if ( PROTOCOL.KOMI.equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confKomi, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( PROTOCOL.HANDICAPS.equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confHandicaps, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( PROTOCOL.MAINTIME.equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confMainTime, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( PROTOCOL.BYOYOMIPERIODS.equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confByoYomiPeriods, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( PROTOCOL.BYOYOMITIME.equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confByoYomiTime, Integer.parseInt( tokens[ 1 ] ) );
			}
		}
	}
	void handlerStones( String $command ) {
		_gui._board.setStones( ( _stones = $command ).getBytes() );
		_gui._board.repaint();
	}
	void handlerContestant( String $command ) {
		String[] tokens = $command.split( ",", 6 );
		char stone = STONE.NONE;
		GoPlayer contestant = _contestants.get( new Character( stone = tokens[ 0 ].charAt( 0 ) ) );
		contestant._name.setText( tokens[ 1 ] );
		contestant._captures.setText( tokens[ 2 ] );
		contestant._score.setText( tokens[ 3 ] );
		contestant._timeLeft = Integer.parseInt( tokens[ 4 ] );
		Date d = new Date( contestant._timeLeft * 1000 );
		contestant._timeLeftLabel.setText( new SimpleDateFormat( "mm:ss" ).format( d ) );
		if ( Integer.parseInt( tokens[ 5 ] ) >= 0 )
			contestant._byoyomi.setText( tokens[ 5 ] );
		if ( tokens[ 1 ].equals( _app.getName() ) ) {
			_gui._board.setStone( _stone = stone );
			contestant._sit.setText( HGUILocal.GETUP );
			contestant._sit.setEnabled( true );
			GoPlayer foe = _contestants.get( _gui._board.oponent( _stone ) );
			foe._sit.setEnabled( false );
		} else {
			if ( ( stone == _stone ) && ( _stone != STONE.NONE ) ) {
				contestant._sit.setText( HGUILocal.SIT );
				GoPlayer foe = _contestants.get( _gui._board.oponent( _stone ) );
				foe._sit.setEnabled( "".equals( foe._name.getText() ) );
				_gui._board.setStone( _stone = STONE.NONE );
				if ( _gui._toolTip != null ) {
					_gui._pass.setText( _gui._passText );
					_gui._pass.setToolTipText( _gui._toolTip );
				}
			}
			contestant._sit.setEnabled( "".equals( tokens[ 1 ] ) && ( _stone == STONE.NONE ) );
		}
	}
	void handlerStone( String $command ) {
	}
	void handlerToMove( String $command ) {
		_toMove = $command.charAt( 0 );
		if ( ( _toMove == STONE.MARK ) && ( _gui._passText.equals( _gui._pass.getText() ) ) )
			handlerMark();
		else if ( _toMove != STONE.MARK ) {
			_gui._pass.setEnabled( ( _toMove == _stone ) && ( _stone != STONE.NONE ) );
			_gui._conf.setEnabled( _admin && ( _toMove == STONE.NONE ) );
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
			_start = new Date().getTime();
		}
	}
	void handlerMark() {
		_gui._board.setStones( _stones.getBytes(), true );
		_gui._toMove.setText( STONE.NONE_NAME );
		_gui._pass.setText( "Accept" );
		_gui._pass.setEnabled( true );
		_gui.setToolTipText( "You are accepting opponents markings, not your own." );
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
	public char stone() {
		return ( _stone );
	}
	public char stoneDead() {
		return ( _stone == STONE.BLACK ? STONE.DEAD_BLACK : STONE.DEAD_WHITE );
	}
	public char toMove() {
		return ( _toMove );
	}
	public void waitToMove() {
		_toMove = STONE.WAIT;
	}
	public void reinit() {
		_client = _app.getClient();
		_contestants.get( new Character( STONE.BLACK ) ).clear();
		_contestants.get( new Character( STONE.WHITE ) ).clear();
		_stone = STONE.NONE;
		_toMove = STONE.NONE;
		_admin = false;
		_move = 0;
		_app.registerTask( this, 1 );
	}
	public void cleanup() {
		_app.flush( this );
	}
	public void run() {
		if ( ( _toMove == STONE.BLACK ) || ( _toMove == STONE.WHITE ) ) {
			long now = new Date().getTime();
			GoPlayer p = _contestants.get( _toMove );
			Date d = new Date( p._timeLeft * 1000 + _start - now );
			p._timeLeftLabel.setText( new SimpleDateFormat( "mm:ss" ).format( d ) );
		}
	}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( LABEL, new Go( $app ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

