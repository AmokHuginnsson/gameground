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

class SetBangPlayer {
	String _name;
	int _score = 0;
	int _last = 0;
	SetBangPlayer( String $name ) { this( $name, 0, 0 ); }
	SetBangPlayer( String $name, int $score, int $last ) {
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

class SetBang extends HAbstractLogic implements Runnable {
	public static final class PROTOCOL {
		public static final String SEP = ":";
		public static final String SEPP = ",";
		public static final String CMD = "cmd";
		public static final String SAY = "say";
		public static final String SETUP = "setup";
		public static final String DECK = "deck";
		public static final String PLAYER = "player";
		public static final String PLAYERQUIT = "player_quit";
	}
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public SetBangTable _table;
		public JTextField _messageInput;
		public JTextPane _logPad;
		public JTable _players;
		public JLabel[] _letters = new JLabel[16];
		boolean _initializedOnce = false;
		final String[] _header = { "Player", "Score", "Last" };
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "setbangtable", SetBangTable.class );
		}
		public void init() {
			super.init();
			clearLog();
			SetBang.this.handlerRound( "" );
			AbstractTableModel model = new AbstractTableModel() {
				public static final long serialVersionUID = 17l;
				public String getColumnName(int col) {
					return ( _header[ col ] );
				}
				public int getRowCount() { return SetBang.this._players.size(); }
				public int getColumnCount() { return _header.length; }
				public Object getValueAt(int row, int col) {
					return SetBang.this._players.get( row ).get( col );
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
			SetBang.this._players.clear();
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public void onMessage() {
			String msg = _messageInput.getText();
			if ( msg.matches( ".*\\S+.*" ) ) {	
				_client.println( "cmd:" + _id + ":say:" + msg );
				_messageInput.setText( "" );
			}
		}
		public void onExit() {
			_app.closeParty( SetBang.this );
		}
	}
//--------------------------------------------//
	public static final int CARDS_ON_TABLE = 12;
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "setbang";
	public HGUILocal _gui;
	private Vector<SetBangPlayer> _players = new Vector<SetBangPlayer>();
//--------------------------------------------//
	public SetBang( GameGround $applet, String $id, String $configuration ) throws Exception {
		super( $applet, $id, $configuration );
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( PROTOCOL.PLAYER, SetBang.class.getDeclaredMethod( "handlerPlayer", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		_handlers.put( PROTOCOL.DECK, SetBang.class.getDeclaredMethod( "handlerDeck", new Class[]{ String.class } ) );
		_app.registerTask( this, 1 );
	}
	void handlerPlayer( String $command ) {
		String[] tokens = $command.split( ",", 3 );
		SetBangPlayer p = null;
		for ( int i = 0; i < _players.size(); ++ i ) {
			if ( _players.get( i ).get( 0 ).compareToIgnoreCase( tokens[ 0 ] ) == 0 ) {
				p = _players.get( i );
				break;
			}
		}
		if ( p == null ) {
			p = new SetBangPlayer( tokens[ 0 ] );
			_players.add( p );
		}
		p.set( tokens[0], Integer.parseInt( tokens[ 1 ] ), Integer.parseInt( tokens[ 2 ] ) );
		((AbstractTableModel)_gui._players.getModel()).fireTableDataChanged();
	}
	void handlerEndRound( String $command ) {
	}
	void handlerRound( String $command ) {
	}
	void handlerDeck( String $command ) {
		String[] cardsStr = $command.split( ",", CARDS_ON_TABLE );
		if ( cardsStr.length != CARDS_ON_TABLE ) {
			Con.err( "Bad deck configuration: " + $command );
			CallStack.print();
			System.exit( 1 );
		}
		int[] cards = new int[CARDS_ON_TABLE];
		for ( int i = 0; i < CARDS_ON_TABLE; ++ i ) {
			if ( ( cardsStr[i].length() > 0 ) && ! cardsStr[i].equals( "n" ) ) {
				cards[i] = Integer.parseInt( cardsStr[i] );
			} else
				cards[i] = -1;
		}
		_gui._table.setCards( cards );
	}
	public void cleanup() {
		_app.flush( this );
	}
	public void run() {
	}
	static HAbstractLogic create( GameGround $app, String $id, String $configuration ) {
		HAbstractLogic logic = null;
		try {
			logic = new SetBang( $app, $id, $configuration );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( logic );
	}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( "set_bang", new HLogicInfo( "set_bang", "setbang", "Set!", new SetBangConfigurator(), SetBang.class.getDeclaredMethod( "create", new Class[] { GameGround.class, String.class, String.class } ) ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

