import java.util.Vector;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.table.AbstractTableModel;
import java.util.Comparator;
import javax.swing.table.TableRowSorter;
import java.util.Date;
import java.text.SimpleDateFormat;

class BogglePlayer {
	String _name;
	int _score = 0;
	int _last = 0;
	BogglePlayer( String $name ) { this( $name, 0, 0 ); }
	BogglePlayer( String $name, int $score, int $last ) {
		_name = $name;
		_score = $score;
		_last = $last;
	}
	void set( String $name, int $score, int $last ) {
		_name = $name;
		_score = $score;
		_last = $last;
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
			case ( 2 ):
				field += _last;
			break;
		}
		return ( field );
	}
}

class Boggle extends HAbstractLogic implements Runnable {
	public static final class PROTOCOL {
		public static final String SEP = ":";
		public static final String SEPP = ",";
		public static final String CMD = "cmd";
		public static final String SAY = "say";
		public static final String LONGEST = "longest";
		public static final String ROUND = "round";
		public static final String SETUP = "setup";
		public static final String DECK = "deck";
		public static final String SCORED = "scored";
		public static final String ENDROUND = "end_round";
		public static final String PLAYER = "player";
		public static final String PLAYERQUIT = "player_quit";
	}
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _messageInput;
		public JTextField _wordInput;
		public JTextPane _wordsSent;
		public JTextPane _wordsScored;
		public JTextPane _wordsLongest;
		public JTextPane _logPad;
		public JTable _players;
		public JLabel _timeLeftLabel;
		public JLabel _letter00;
		public JLabel _letter01;
		public JLabel _letter02;
		public JLabel _letter03;
		public JLabel _letter10;
		public JLabel _letter11;
		public JLabel _letter12;
		public JLabel _letter13;
		public JLabel _letter20;
		public JLabel _letter21;
		public JLabel _letter22;
		public JLabel _letter23;
		public JLabel _letter30;
		public JLabel _letter31;
		public JLabel _letter32;
		public JLabel _letter33;
		public JLabel[] _letters = new JLabel[16];
		public BoggleConfigurator _conf;
		final String[] _header = { "Player", "Score", "Last" };
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "panel", BoggleConfigurator.class );
		}
		public void init() {
			super.init();
			_letters[ 0 ] = _letter00;
			_letters[ 1 ] = _letter01;
			_letters[ 2 ] = _letter02;
			_letters[ 3 ] = _letter03;
			_letters[ 4 ] = _letter10;
			_letters[ 5 ] = _letter11;
			_letters[ 6 ] = _letter12;
			_letters[ 7 ] = _letter13;
			_letters[ 8 ] = _letter20;
			_letters[ 9 ] = _letter21;
			_letters[ 10 ] = _letter22;
			_letters[ 11 ] = _letter23;
			_letters[ 12 ] = _letter30;
			_letters[ 13 ] = _letter31;
			_letters[ 14 ] = _letter32;
			_letters[ 15 ] = _letter33;
			for ( int i = 0; i < 16; ++ i )
				_letters[ i ].setText( "GAMEGROUNDBOGGLE".substring( i, i + 1 ).toUpperCase() );
			clearLog();
			Boggle.this.handlerRound( "" );
			AbstractTableModel model = new AbstractTableModel() {
				public static final long serialVersionUID = 17l;
				public String getColumnName(int col) {
					return ( _header[ col ] );
				}
				public int getRowCount() { return Boggle.this._players.size(); }
				public int getColumnCount() { return _header.length; }
				public Object getValueAt(int row, int col) {
					return Boggle.this._players.get( row ).get( col );
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
			rs.setComparator( 2, comparator );
			_players.setRowSorter( rs );
			_players.setShowGrid( false );
			_players.setAutoCreateRowSorter( true );
			Boggle.this._players.clear();
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
				_client.println( "cmd:" + _id + ":say:" + msg );
				_messageInput.setText( "" );
			}
		}
		public void onWord() {
			String msg = _wordInput.getText();
			if ( msg.matches( ".*\\S+.*" ) ) {	
				_client.println( "cmd:" + _id + ":play:" + msg );
				_wordInput.setText( "" );
				add( _wordsSent, msg + "\n" );
			}
		}
		public void onExit() {
			_app.closeParty( _id );
		}
	}
	enum State { INIT, PLAY, WAIT }
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "boggle";
	public HGUILocal _gui;
	private long _start = 0;
	private long _timeLeft = 0;
	private long _roundTime = 0;
	private long _pauseTime = 0;
	private State _state = State.INIT;
	private Vector<BogglePlayer> _players = new Vector<BogglePlayer>();
//--------------------------------------------//
	public Boggle( GameGround $applet, HLogicInfo $info ) throws Exception {
		super( $applet );
		_info = $info;
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( PROTOCOL.PLAYER, Boggle.class.getDeclaredMethod( "handlerPlayer", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		_handlers.put( "party_info", HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.DECK, Boggle.class.getDeclaredMethod( "handlerDeck", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.SCORED, Boggle.class.getDeclaredMethod( "handlerScored", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.LONGEST, Boggle.class.getDeclaredMethod( "handlerLongest", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.ROUND, Boggle.class.getDeclaredMethod( "handlerRound", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.ENDROUND, Boggle.class.getDeclaredMethod( "handlerEndRound", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.SETUP, Boggle.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
	}
	void handlerPlayer( String $command ) {
		String[] tokens = $command.split( ",", 3 );
		BogglePlayer p = null;
		for ( int i = 0; i < _players.size(); ++ i ) {
			if ( _players.get( i ).get( 0 ).compareToIgnoreCase( tokens[ 0 ] ) == 0 ) {
				p = _players.get( i );
				break;
			}
		}
		if ( p == null ) {
			p = new BogglePlayer( tokens[ 0 ] );
			_players.add( p );
		}
		p.set( tokens[0], Integer.parseInt( tokens[ 1 ] ), Integer.parseInt( tokens[ 2 ] ) );
		((AbstractTableModel)_gui._players.getModel()).fireTableDataChanged();
	}
	void handlerScored( String $command ) {
		_gui.add( _gui._wordsScored, $command + "\n" );
	}
	void handlerEndRound( String $command ) {
		_start = new Date().getTime();
		_timeLeft = _pauseTime;
		_state = State.WAIT;
	}
	void handlerRound( String $command ) {
		_gui.clear( _gui._wordsSent );
		_gui.clear( _gui._wordsScored );
		_gui.clear( _gui._wordsLongest );
	}
	void handlerLongest( String $command ) {
		_gui.add( _gui._wordsLongest, $command + "\n" );
	}
	void handlerDeck( String $command ) {
		if ( $command.length() < 16 ) {
			System.out.println( "Bad deck configuration: " + $command );
			CallStack.print();
			System.exit( 1 );
		}
		for ( int i = 0; i < 16; ++ i )
			_gui._letters[ i ].setText( $command.substring( i, i + 1 ).toUpperCase() );
		_start = new Date().getTime();
		_timeLeft = _roundTime;
		_state = State.PLAY;
	}
	void handlerSetup( String $command ) {
		String[] tokens = $command.split( ",", 2 );
		 _roundTime = Integer.parseInt( tokens[ 0 ] );
		 _pauseTime = Integer.parseInt( tokens[ 1 ] );
	}
	public void cleanup() {
		_app.flush( this );
	}
	public void run() {
		if ( ( _state == State.PLAY ) || ( _state == State.WAIT ) ) {
			long now = new Date().getTime();
			long left = _timeLeft * 1000 + _start - now;
			if ( left >= 0 ) {
				Date d = new Date( left );
				_gui._timeLeftLabel.setText( new SimpleDateFormat( "mm:ss" ).format( d ) );
			}
		}
	}
	public void init() {
		_client = _app.getClient();
		_state = State.INIT;
		_app.registerTask( this, 1 );
	}
	static HAbstractLogic create( GameGround $app, HLogicInfo $info ) {
		HAbstractLogic logic = null;
		try {
			logic = new Boggle( $app, $info );
		} catch ( Exception e ) {
		}
		return ( logic );
	}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( "bgl", new HLogicInfo( "bgl", "boggle", "Boggle", Boggle.class.getDeclaredMethod( "create", new Class[] { GameGround.class, HLogicInfo.class } ) ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

