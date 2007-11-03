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
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.text.Style;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.io.PrintWriter;

class Boggle extends HAbstractLogic {
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _messageInput;
		public JTextPane _logPad;
		public BoggleConfigurator _conf;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "panel", BoggleConfigurator.class );
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
//--------------------------------------------//
	public Boggle( GameGround $applet ) throws Exception {
		super();
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( "setup", Boggle.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
		_handlers.put( "bgl", Boggle.class.getDeclaredMethod( "handlerBoggle", new Class[]{ String.class } ) );
		_handlers.put( "play", Boggle.class.getDeclaredMethod( "handlerPlay", new Class[]{ String.class } ) );
		_handlers.put( "player", HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		_handlers.put( "player_quit", HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		_info = new HLogicInfo( "bgl", "boggle", "Boggle" );
	}
	void handlerBoggle( String $command ) {
		processMessage( $command );
	}
	void handlerSetup( String $command ) {
	}
	void handlerPlay( String $command ) {
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

