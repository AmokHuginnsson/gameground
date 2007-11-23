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
			_conf.setOwner( this );
			_conf.gobanModel();
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
			_app.setFace( HBrowser.LABEL );
		}
		public void onGobanSize() {
			_client.println( "cmd:setup:goban," + (String)_conf._confGoban.getSelectedItem() );
		}
		public void onKomi() {
			_client.println( "cmd:setup:komi," + _conf._confKomi.getValue() );
		}
		public void onHandicaps() {
			_client.println( "cmd:setup:handicaps," + _conf._confHandicaps.getValue() );
		}
		public void onMainTime() {
			_client.println( "cmd:setup:maintime," + _conf._confMainTime.getValue() );
		}
		public void onByoyomiPeriods() {
			_client.println( "cmd:setup:byoyomiperiods," + _conf._confByoYomiPeriods.getValue() );
		}
		public void onByoyomiTime() {
			_client.println( "cmd:setup:byoyomitime," + _conf._confByoYomiTime.getValue() );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "go";
	public HGUILocal _gui;
	private HClient _client;
//--------------------------------------------//
	public Go( GameGround $applet ) throws Exception {
		super( $applet );
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( "go", Go.class.getDeclaredMethod( "handlerGo", new Class[]{ String.class } ) );
		_handlers.put( "setup", Go.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
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
	void handlerSetup( String $command ) {
		String[] tokens = $command.split( ",", 2 );
		if ( "admin".equals( $command ) ) {
			_gui._conf.setEnabled( true );
		} else {
			int value = Integer.parseInt( tokens[ 1 ] );
			if ( "goban".equals( tokens[ 0 ] ) ) {
				_gui._board.setSize( value );
			} else if ( "komi".equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confKomi, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( "handicaps".equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confHandicaps, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( "maintime".equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confMainTime, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( "byoyomiperiods".equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confByoYomiPeriods, Integer.parseInt( tokens[ 1 ] ) );
			} else if ( "byoyomitime".equals( tokens[ 0 ] ) ) {
				_gui._conf.setValue( _gui._conf._confByoYomiTime, Integer.parseInt( tokens[ 1 ] ) );
			}
		}
	}
	void handlerPlayer( String $command ) {
	}
	public void reinit() {
		_client = _app.getClient();
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

