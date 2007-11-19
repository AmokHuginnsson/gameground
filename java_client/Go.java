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
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.table.AbstractTableModel;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.Style;
import java.util.Comparator;
import org.swixml.SwingEngine;
import javax.swing.table.TableRowSorter;
import javax.swing.RowSorter;

class GoPlayer {
	String _name;
	int _captures = 0;
	GoPlayer( String $name ) { this( $name, 0 ); }
	GoPlayer( String $name, int $captures ) {
		_name = $name;
		_captures = $captures;
	}
	void set( String $name, int $captures ) {
		_name = $name;
		_captures = $captures;
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
				field += _captures;
			break;
		}
		return ( field );
	}
}

class Go extends HAbstractLogic {
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _messageInput;
		public JTextField _wordInput;
		public JTextPane _logPad;
		public Goban _board;
		public GoConfigurator _conf;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "goban", Goban.class );
			$xul.getTaglib().registerTag( "panel", GoConfigurator.class );
		}
		public void reinit() {
			clearLog();
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
				_client.println( "cmd:go:say:" + msg );
				_messageInput.setText( "" );
			}
		}
		public void onExit() {
			GameGround.getInstance().setFace( HBrowser.LABEL );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	private static final boolean registered;
	static {
		registered = registerLogic();
	}
	public static final String LABEL = "go";
	public HGUILocal _gui;
	private HClient _client;
//--------------------------------------------//
	public Go( GameGround $applet ) throws Exception {
		super();
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( "go", Go.class.getDeclaredMethod( "handlerGo", new Class[]{ String.class } ) );
		_handlers.put( "player", Go.class.getDeclaredMethod( "handlerPlayer", new Class[]{ String.class } ) );
		_handlers.put( "player_quit", HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		_info = new HLogicInfo( "go", "go", "Go" );
		GoImages images = new GoImages();
		_gui._board.setGui( this );
		_gui._board.setImages( images );
	}
	void handlerGo( String $command ) {
		processMessage( $command );
	}
	void handlerPlayer( String $command ) {
	}
	static boolean registerLogic() {
		try {
			GameGround.registerLogic( LABEL, new Go( GameGround.getInstance() ) );
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

