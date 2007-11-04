import java.lang.reflect.Method;
import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import javax.swing.Action;
import javax.swing.AbstractAction;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.Container;
import org.swixml.SwingEngine;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.text.Style;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Vector;
import java.io.PrintWriter;
import javax.swing.table.AbstractTableModel;

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
	void set( int $score, int $last ) {
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

class Boggle extends HAbstractLogic {
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _messageInput;
		public JTextPane _logPad;
		public JTable _players;
		public BoggleConfigurator _conf;
		final String[] _header = { "Player", "Score", "Last" };
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "panel", BoggleConfigurator.class );
		}
		public void reinit() {
			clearLog();
			_players.setModel( new AbstractTableModel() {
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
			} );
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public HAbstractConfigurator getConfigurator() {
			return ( _conf );
		}
		public Action onMessage = new AbstractAction() {
			public static final long serialVersionUID = 17l;
			public void actionPerformed( ActionEvent $event ) {
				String msg = _messageInput.getText();
				if ( msg.matches( ".*\\S+.*" ) ) {	
					_client.println( "cmd:bgl:say:" + msg );
					_gui._messageInput.setText( "" );
				}
			}
		};
		public void onExit() {
			_client.println( "abandon" );
			GameGround.getInstance().setFace( HBrowser.LABEL );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	private static final boolean registered;
	static {
		registered = registerLogic();
	}
	public static final String LABEL = "boggle";
	public HGUILocal _gui;
	private HClient _client;
	private Vector<BogglePlayer> _players = new Vector<BogglePlayer>();
//--------------------------------------------//
	public Boggle( GameGround $applet ) throws Exception {
		super();
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( "setup", Boggle.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
		_handlers.put( "bgl", Boggle.class.getDeclaredMethod( "handlerBoggle", new Class[]{ String.class } ) );
		_handlers.put( "play", Boggle.class.getDeclaredMethod( "handlerPlay", new Class[]{ String.class } ) );
		_handlers.put( "player", Boggle.class.getDeclaredMethod( "handlerPlayer", new Class[]{ String.class } ) );
		_handlers.put( "player_quit", Boggle.class.getDeclaredMethod( "handlerPlayerQuit", new Class[]{ String.class } ) );
		_info = new HLogicInfo( "bgl", "boggle", "Boggle" );
	}
	void handlerBoggle( String $command ) {
		processMessage( $command );
	}
	void handlerSetup( String $command ) {
	}
	void handlerPlay( String $command ) {
	}
	void handlerPlayer( String $command ) {
		String[] tokens = $command.split( ",", 3 );
		BogglePlayer p = null;
		for ( int i = 0; i < _players.size(); ++ i ) {
			if ( _players.get( i ).get( 0 ) == tokens[ 0 ] ) {
				p = _players.get( i );
				break;
			}
		}
		if ( p == null ) {
			p = new BogglePlayer( tokens[ 0 ] );
			_players.add( p );
		}
		p.set( Integer.parseInt( tokens[ 1 ] ), Integer.parseInt( tokens[ 2 ] ) );
	}
	void handlerPlayerQuit( String $command ) {
		int idx = -1;
		for ( int i = 0; i < _players.size(); ++ i ) {
			if ( _players.get( i ).get( 0 ) == $command ) {
				idx = i;
				break;
			}
		}
		if ( idx >= 0 )
			_players.remove( idx );
	}
	static boolean registerLogic() {
		try {
			GameGround.registerLogic( LABEL, new Boggle( GameGround.getInstance() ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
	public void reinit() {
		GameGround gg = GameGround.getInstance();
		_client = gg.getClient();
	}
}

