import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Date;
import java.text.SimpleDateFormat;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.JSlider;
import javax.swing.DefaultListModel;
import javax.swing.event.ChangeEvent;
import java.awt.KeyboardFocusManager;
import java.awt.KeyEventDispatcher;
import java.awt.event.KeyEvent;
import java.awt.Component;
import javax.swing.SwingUtilities;

class GoPlayer {
	public JLabel _name;
	public JLabel _score;
	public JLabel _captures;
	public JLabel _timeLeftLabel;
	public JLabel _byoyomi;
	public JButton _sit;
	public int _timeLeft = 0;
	public boolean _sitting = false;
	public static final String EMPTY = " ";
	public void clear( int $komi ) {
		_sitting = false;
		_name.setText( EMPTY );
		_score.setText( "" + $komi );
		_captures.setText( "" );
		_timeLeftLabel.setText( "" );
		_byoyomi.setText( "" );
		_sit.setText( Go.HGUILocal.SIT );
	}
	public void clearScore( int $komi ) {
		_score.setText( "" + $komi );
		_captures.setText( "" + 0 );
	}
	void capture( int $captures ) {
		int current = Integer.parseInt( _captures.getText() );
		_captures.setText( "" + ( current + $captures ) );
		current = Integer.parseInt( _score.getText() );
		_score.setText( "" + ( current + $captures ) );
	}
	void score( int $score ) {
		int current = Integer.parseInt( _score.getText() );
		_score.setText( "" + ( current + $score ) );
	}
}

class Go extends HAbstractLogic implements Runnable {
	public static final class PROTOCOL extends HAbstractLogic.PROTOCOL {
		public static final String NAME = "go";
		public static final String PLAY = "play";
		public static final String NEWGAME = "newgame";
		public static final String PASS = "pass";
		public static final String UNDO = "undo";
		public static final String SETUP = "setup";
		public static final String ADMIN = "admin";
		public static final String KOMI = "komi";
		public static final String HANDICAPS = "handicaps";
		public static final String CONTESTANT = "contestant";
		public static final String MAINTIME = "maintime";
		public static final String GOBAN = "goban";
		public static final String SGF = "sgf";
		public static final String SIT = "sit";
		public static final String GETUP = "get_up";
		public static final String BYOYOMITIME = "byoyomitime";
		public static final String BYOYOMIPERIODS = "byoyomiperiods";
		public static final String PUTSTONE = "put_stone";
		public static final String SELECT = "select";
		public static final String STONE = "stone";
		public static final String TOMOVE = "to_move";
		public static final String PLAYER = "player";
		public static final String PLAYERQUIT = "player_quit";
		public static final String DEAD = "dead";
		public static final String ACCEPT = "accept";
	}
	public static final class GOBAN_SIZE {
		public static final int NORMAL = 19;
	}
	public static final class STONE extends Goban.STONE {
		public static final byte MARK = 'm';
		public static final byte DEAD_BLACK = 's';
		public static final byte DEAD_WHITE = 't';
		public static final byte TERITORY_BLACK = 'p';
		public static final byte TERITORY_WHITE = 'q';
		public static final byte DAME = 'x';
	}
	public class HGUILocal extends HGUIface implements javax.swing.event.ChangeListener, GobanHolderInterface {
		public static final long serialVersionUID = 17l;
		public static final String SIT = "Sit !";
		public static final String GETUP = "Get up !";
		public static final String RESIGN = "Resign !";
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
		public JButton _newgame;
		public JButton _load;
		public JButton _pass;
		public JButton _undo;
		public JList<String> _visitors;
		public JSlider _jumpToMove;
		public GoGoban _board;
		public SGFTree _sgftree;
		public GoConfigurator _conf = new GoConfigurator();
		public JLabel _toMove;
		public JLabel _move;
		public String _toolTip;
		public String _passText;
		boolean _internalUpdate = false;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "goban", GoGoban.class );
			$xul.getTaglib().registerTag( "datespinner", DateSpinner.class );
			$xul.getTaglib().registerTag( "sgftree", SGFTree.class );
		}
		public void mapMembers( XUL $se ) {
			$se.mapMembers( _conf );
			_conf.init();
		}
		public void init() {
			super.init();
			_toolTip = _pass.getToolTipText();
			_passText = _pass.getText();
			_conf.setOwner( this );
			_conf.gobanModel();
			((DefaultListModel<String>)_visitors.getModel()).clear();
			setEnabledControls( false );
			_pass.setText( _passText );
			_pass.setToolTipText( _toolTip );
			_jumpToMove.addChangeListener( this );
			KeyEventDispatcher ked = new KeyEventDispatcher() {
				@Override
				public boolean dispatchKeyEvent( KeyEvent e ) {
					Object src = e.getSource();
					boolean isComp = src instanceof Component;
					if ( isComp && SwingUtilities.isDescendingFrom( (Component)src, HGUILocal.this ) && ( e.getID() == KeyEvent.KEY_PRESSED ) && ! ( src instanceof JTextField ) ) {
						switch ( e.getKeyCode() ) {
							case ( KeyEvent.VK_LEFT ): {
								onGoToPrevious();
							} break;
							case ( KeyEvent.VK_RIGHT ): {
								onGoToNext();
							} break;
							case ( KeyEvent.VK_UP ): {
								_board.choosePreviousPath();
							} break;
							case ( KeyEvent.VK_DOWN ): {
								_board.chooseNextPath();
							} break;
							case ( KeyEvent.VK_HOME ): {
								onGoToFirst();
							} break;
							case ( KeyEvent.VK_END ): {
								onGoToLast();
							} break;
						}
					}
					return false;
				}
			};
			KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher( ked );
			_sgftree.requestFocusInWindow();
		}
		public void stateChanged(ChangeEvent e) {
			if ( ! _internalUpdate )
				_board.placeStones( _jumpToMove.getValue() );
		}
		public JTextPane getLogPad() {
			_logPad.setFont( _app.setup().getFont() );
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
		public void onClose() {
			_app.closeParty( Go.this );
		}
		public void onGobanSize() {
			if ( _conf.eventsIgnored() )
				return;
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.GOBAN + PROTOCOL.SEPP
					+ (String)_conf._confGoban.getSelectedItem() );
		}
		public void onKomi() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.KOMI + PROTOCOL.SEPP
					+ _conf._confKomi.getValue() );
		}
		public void onHandicaps() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.HANDICAPS + PROTOCOL.SEPP
					+ _conf._confHandicaps.getValue() );
		}
		public void onMainTime() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.MAINTIME + PROTOCOL.SEPP
					+ _conf._confMainTime.getValueInt() );
		}
		public void onByoyomiPeriods() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.BYOYOMIPERIODS + PROTOCOL.SEPP
					+ _conf._confByoYomiPeriods.getValue() );
		}
		public void onByoyomiTime() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.SETUP + PROTOCOL.SEP
					+ PROTOCOL.BYOYOMITIME + PROTOCOL.SEPP
					+ _conf._confByoYomiTime.getValueInt() );
		}
		public void onBlack() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ ( ( _playerColor == Go.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEP + (char)STONE.BLACK : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _playerColor != Go.STONE.NONE );
			_whiteSit.setEnabled( _playerColor != Go.STONE.NONE );
		}
		public void onWhite() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ ( ( _playerColor == Go.STONE.NONE ) ? PROTOCOL.SIT + PROTOCOL.SEP + (char)STONE.WHITE : PROTOCOL.GETUP ) );
			_blackSit.setEnabled( _playerColor != Go.STONE.NONE );
			_whiteSit.setEnabled( _playerColor != Go.STONE.NONE );
		}
		public void onPass() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP
					+ ( Go.this._toMove != STONE.MARK ? PROTOCOL.PASS : PROTOCOL.ACCEPT ) );
			_pass.setEnabled( false );
		}
		public void onNew() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP + PROTOCOL.NEWGAME );
		}
		public void onUndo() {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP
					+ PROTOCOL.PLAY + PROTOCOL.SEP + PROTOCOL.UNDO );
		}
		public void onLoad() {
			_board.load();
		}
		public void onSave() {
			_board.save();
		}
		public void onGoToFirst() {
			_board.goToFirst();
		}
		public void onGoToPrevious() {
			_board.goToPrevious();
		}
		public void onGoToNext() {
			_board.goToNext();
		}
		public void onGoToLast() {
			_board.goToLast();
		}
		public void setMoveSlider( int $viewMove, int $lastMove ) {
			_internalUpdate = true;
			_jumpToMove.setMaximum( $lastMove );
			_jumpToMove.setValue( $viewMove );
			_gui._sgftree._valid = false;
			_internalUpdate = false;
		}
		public void jumpToMove( HTree<SGF.Move>.HNode<SGF.Move> $node ) {
			_gui._board.selectBranch( $node );
		}
		public HTree<SGF.Move>.HNode<SGF.Move> viewMove() {
			return ( _gui._board._viewMove );
		}
		public HTree<SGF.Move>.HNode<SGF.Move> currentMove() {
			return ( _gui._board._currentMove );
		}
		public void updateSetup() {
			_conf.selectGobanSize( "" + _board._sgf._gobanSize );
			_board.setSize( _board._sgf._gobanSize );
			_conf.setValue( _conf._confKomi, (int)_board._sgf._komi );
			_conf.setValue( _conf._confHandicaps, _board._sgf._handicap );
			_conf.setValue( _conf._confMainTime, _board._sgf._time );
			GoPlayer contestant = _contestants.get( STONE.BLACK );
			contestant._name.setText( _board._sgf._blackName + "[" + _board._sgf._blackRank + "]" );
			contestant = _contestants.get( STONE.WHITE );
			contestant._name.setText( _board._sgf._whiteName + "[" + _board._sgf._whiteRank + "]" );
		}
		void setEnabledControls( boolean $enabled ) {
			_conf.setEnabled( $enabled );
			_newgame.setEnabled( $enabled );
			_load.setEnabled( $enabled );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "go";
	public HGUILocal _gui;
	private byte _playerColor = STONE.NONE;
	private byte _toMove = STONE.NONE;
	private long _start = 0;
	private boolean _admin = false;
	private int _move = 0;
	private String _stones = null;
	private SortedMap<Byte, GoPlayer> _contestants = java.util.Collections.synchronizedSortedMap( new TreeMap<Byte, GoPlayer>() );
//--------------------------------------------//
	public Go( GameGround $applet, String $id, String $configuration ) throws Exception {
		super( $applet, $id, $configuration );
		init( _gui = new HGUILocal( LABEL ) );
		_gui._conf.setDefaults( $configuration );
		_handlers.put( PROTOCOL.SETUP, Go.class.getDeclaredMethod( "handlerSetup", new Class<?>[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYER, Go.class.getDeclaredMethod( "handlerPlayer", new Class<?>[]{ String.class } ) );
		_handlers.put( PROTOCOL.TOMOVE, Go.class.getDeclaredMethod( "handlerToMove", new Class<?>[]{ String.class } ) );
		_handlers.put( PROTOCOL.STONE, Go.class.getDeclaredMethod( "handlerStone", new Class<?>[]{ String.class } ) );
		_handlers.put( PROTOCOL.CONTESTANT, Go.class.getDeclaredMethod( "handlerContestant", new Class<?>[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, Go.class.getDeclaredMethod( "handlerPlayerQuit", new Class<?>[]{ String.class } ) );
		_handlers.put( PROTOCOL.SGF, Go.class.getDeclaredMethod( "handlerSGF", new Class<?>[]{ String.class } ) );
		_handlers.put( PROTOCOL.SELECT, Go.class.getDeclaredMethod( "handlerSelect", new Class<?>[]{ String.class } ) );
		GoImages images = new GoImages();
		_gui._board.setLogic( this );
		_gui._board.setImages( images );
		_gui._sgftree.setLogic( this );
		_gui._sgftree.setImages( images );
		_gui._sgftree.setSGF( _gui._board.getSGF() );
		GoPlayer black = new GoPlayer();
		black._name = _gui._blackName;
		black._captures = _gui._blackCaptures;
		black._score = _gui._blackScore;
		black._timeLeftLabel = _gui._blackTimeLeft;
		black._byoyomi = _gui._blackByoYomiLeft;
		black._sit = _gui._blackSit;
		_contestants.put( STONE.BLACK, black );
		GoPlayer white = new GoPlayer();
		white._name = _gui._whiteName;
		white._captures = _gui._whiteCaptures;
		white._score = _gui._whiteScore;
		white._timeLeftLabel = _gui._whiteTimeLeft;
		white._byoyomi = _gui._whiteByoYomiLeft;
		white._sit = _gui._whiteSit;
		_contestants.put( STONE.WHITE, white );
		/* Was in init(). */
		_contestants.get( STONE.BLACK ).clear( 0 );
		_contestants.get( STONE.WHITE ).clear( ((Integer)_gui._conf._confKomi.getValue()).intValue() );
		_playerColor = STONE.NONE;
		_toMove = STONE.NONE;
		_admin = false;
		_move = 0;
		_app.registerTask( this, 1 );
	}
	void handlerSetup( String $command ) {
		String[] tokens = $command.split( ",", 2 );
		if ( PROTOCOL.ADMIN.equals( $command ) ) {
			_admin = true;
			if ( ! ongoingMatch() )
				_gui.setEnabledControls( true );
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
	boolean ongoingMatch() {
		return ( isPlayerSitting( STONE.BLACK ) && isPlayerSitting( STONE.WHITE ) );
	}
	boolean isPlayerSitting( byte $stone ) {
		GoPlayer p = _contestants.get( $stone );
		return ( p._sitting );
	}
	public boolean isAdmin() {
		return ( _admin );
	}
	void handlerContestant( String $command ) {
		String[] tokens = $command.split( ",", 6 );
		byte stone = STONE.NONE;
		GoPlayer contestant = _contestants.get( ( stone = (byte)tokens[ 0 ].charAt( 0 ) ) );
		contestant._sitting = ! "".equals( tokens[ 1 ] );
		contestant._name.setText( contestant._sitting ? tokens[ 1 ] : GoPlayer.EMPTY );
		contestant._captures.setText( tokens[ 2 ] );
		contestant._score.setText( tokens[ 3 ] );
		contestant._timeLeft = Integer.parseInt( tokens[ 4 ] );
		Date d = new Date( contestant._timeLeft * 1000 );
		contestant._timeLeftLabel.setText( new SimpleDateFormat( "mm:ss" ).format( d ) );
		if ( Integer.parseInt( tokens[ 5 ] ) >= 0 )
			contestant._byoyomi.setText( tokens[ 5 ] );
		if ( tokens[ 1 ].equals( _app.getName() ) ) {
			_playerColor = stone;
			contestant._sit.setEnabled( true );
			GoPlayer foe = _contestants.get( Goban.opponent( _playerColor ) );
			foe._sit.setEnabled( false );
		} else {
			if ( ( stone == _playerColor ) && ( _playerColor != STONE.NONE ) ) {
				contestant._sit.setText( HGUILocal.SIT );
				GoPlayer foe = _contestants.get( Goban.opponent( _playerColor ) );
				foe._sit.setEnabled( ! foe._sitting );
				_playerColor = STONE.NONE;
				if ( _gui._toolTip != null ) {
					_gui._pass.setText( _gui._passText );
					_gui._pass.setToolTipText( _gui._toolTip );
				}
			}
			contestant._sit.setEnabled( "".equals( tokens[ 1 ] ) && ( _playerColor == STONE.NONE ) );
		}
		_start = new Date().getTime();
		if ( _playerColor != STONE.NONE ) {
			if ( isPlayerSitting( Goban.opponent( _playerColor ) ) )
				_contestants.get( _playerColor )._sit.setText( HGUILocal.RESIGN );
		}
		if ( ! ongoingMatch() ) {
			if ( _playerColor != STONE.NONE )
				_contestants.get( _playerColor )._sit.setText( _playerColor != STONE.NONE ? HGUILocal.GETUP : HGUILocal.SIT );
			if ( _admin )
				_gui.setEnabledControls( true );
		} else
			_gui.setEnabledControls( false );
	}
	void captures( byte $stone, int $captures ) {
		GoPlayer contestant = _contestants.get( $stone );
		if ( contestant != null ) {
			contestant.capture( $captures );
		}
	}
	void score( byte $stone, int $score ) {
		GoPlayer contestant = _contestants.get( $stone );
		if ( contestant != null ) {
			contestant.score( $score );
		}
	}
	void clearScore() {
		GoPlayer contestant = _contestants.get( STONE.BLACK );
		if ( contestant != null ) {
			contestant.clearScore( 0 );
		}
		contestant = _contestants.get( STONE.WHITE );
		if ( contestant != null ) {
			contestant.clearScore( ((Integer)_gui._conf._confKomi.getValue()).intValue() );
		}
	}
	void handlerSGF( String $command ) {
		if ( ( _toMove == STONE.BLACK ) || ( _toMove == STONE.WHITE ) )
			Sound.play( "stone" );
		_gui._board.updateSGF( $command.replace( "\\n", "\n" ) );
		_gui._board.repaint();
		_gui._sgftree.repaint();
	}
	void handlerSelect( String $command ) {
		if ( ! _admin )
			_gui._board.select( $command );
	}
	void handlerStone( String $command ) {
	}
	void handlerToMove( String $command ) {
		_toMove = (byte)$command.charAt( 0 );
		if ( ( _toMove == STONE.MARK ) && ( _gui._passText.equals( _gui._pass.getText() ) ) )
			handlerMark();
		else if ( _toMove != STONE.MARK ) {
			_gui._pass.setEnabled( ( _toMove == _playerColor ) && ( _playerColor != STONE.NONE ) );
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
		_gui._toMove.setText( STONE.NONE_NAME );
		_gui._pass.setText( "Accept" );
		_gui._pass.setEnabled( true );
		_gui.setToolTipText( "You are accepting opponents markings, not your own." );
	}
	void handlerPlayer( String $command ) {
		DefaultListModel<String> m = (DefaultListModel<String>)_gui._visitors.getModel();
		m.addElement( $command );
	}
	void handlerPlayerQuit( String $command ) {
		DefaultListModel<String> m = (DefaultListModel<String>)_gui._visitors.getModel();
		m.removeElement( $command );
	}
	public boolean isMyMove() {
		return ( ( ( _playerColor == STONE.NONE ) && _admin ) || ( ( _playerColor != STONE.NONE ) && ( _playerColor == _toMove ) ) );
	}
	public byte playerColor() {
		return ( _playerColor );
	}
	public byte stoneDead() {
		return ( _playerColor == STONE.BLACK ? STONE.DEAD_BLACK : STONE.DEAD_WHITE );
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
		if ( ( _toMove == STONE.BLACK ) || ( _toMove == STONE.WHITE ) ) {
			long now = new Date().getTime();
			GoPlayer p = _contestants.get( _toMove );
			long left = p._timeLeft * 1000 + _start - now;
			if ( left >= 0 ) {
				Date d = new Date( left );
				p._timeLeftLabel.setText( new SimpleDateFormat( "mm:ss" ).format( d ) );
			}
		}
	}
	static HAbstractLogic create( GameGround $app, String $id, String $configuration ) {
		HAbstractLogic logic = null;
		try {
			logic = new Go( $app, $id, $configuration );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( logic );
	}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( "go", new HLogicInfo( "go", "go", "Go", null, Go.class.getDeclaredMethod( "create", new Class<?>[] { GameGround.class, String.class, String.class } ) ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

