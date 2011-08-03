import java.lang.reflect.Method;
import java.util.SortedMap;
import java.util.TreeMap;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.awt.event.ActionEvent;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JTextField;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import org.swixml.XTabbedPane;

class HWorkArea extends HAbstractWorkArea {
	public static final String LABEL = "workarea";
	static boolean once = true;
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public XTabbedPane _tabs = null;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		void init() {
			super.init();
			_browser = new HBrowser( _app );
			_tabs.addTab( "Browser", _browser.getGUI() );
		}
		public void onExit() {
			_app.shutdown();
		}
		public JTextPane getLogPad() { return ( null ); }
		public void updateTagLib( XUL $xul ) { }
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	SortedMap<String, Method> _handlers;
	SortedMap<String, HLogicInfo> _logics;
	HClient _client;
	HGUILocal _gui;
	private HBrowser _browser;
//--------------------------------------------//
	public HWorkArea( GameGround $applet ) throws Exception {
		super( $applet );
		init( _gui = new HGUILocal( LABEL ) );
		_logics = java.util.Collections.synchronizedSortedMap( new TreeMap<String, HLogicInfo>() );
		_handlers = java.util.Collections.synchronizedSortedMap( new TreeMap<String, Method>() );
		try {
			_handlers.put( "err", HWorkArea.class.getDeclaredMethod( "handleError", new Class[]{ String.class } ) );
			_handlers.put( "party", HWorkArea.class.getDeclaredMethod( "handleParty", new Class[]{ String.class } ) );
		} catch ( java.lang.NoSuchMethodException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void reinit() {
		HLogin.OConnectionConfig cc = _app.getConnectionConfig();
		_browser.cleanup();
		_browser.log( "###", HGUILocal.Colors.BLUE );
		try {
			MessageDigest md = MessageDigest.getInstance( "SHA1" );
			md.update( cc._password.getBytes() );
			byte[] d = md.digest();
			cc._password = String.format( "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
					d[0], d[1], d[2], d[3],
					d[4], d[5], d[6], d[7],
					d[8], d[9], d[10], d[11],
					d[12], d[13], d[14], d[15],
					d[16], d[17], d[18], d[19] );
		} catch ( NoSuchAlgorithmException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		_browser.log( " Connecting to server: " + cc._host + " to port " + cc._port + ".\n"  );
		try {
			_client = new HClient( _app );
			_client.connect( cc._host, cc._port );
			System.out.println( "Starting connection thread ..." );
			_client.start();
			_client.waitUntilRunning();
			System.out.println( "Connection thread started." );
			_client.println( "login:" + cc._login + ":" + cc._password );
			_app.setName( cc._login );
			_app.setClient( _client );
			_browser.setClient( _client );
		} catch ( Exception e ) {
			System.out.println( "Connection error: " + e.getMessage() );
			JOptionPane.showMessageDialog( _gui,
					"GameGround client was unable to connect to server:\n" + e.getMessage(),
					"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
			_app.showLoginScreen();
			return;
		}
		_client.println( "get_logics" );
		_client.println( "get_partys" );
		_client.println( "get_players" );
	}
	public void handleError( String $message ) {
		javax.swing.JOptionPane.showMessageDialog( _gui,
				"The GameGround server reported error condition:\n" + $message,
				"GameGround - error ...", javax.swing.JOptionPane.ERROR_MESSAGE );
		_app.showLoginScreen();
	}
	public void handleParty( String $message ) {
		String[] toks = $message.split( ",", 2 );
		if ( "party".equals( toks[0] ) ) {
			String[] p = $message.split( ",", 2 );
			$message = p[1];
			System.out.println( $message );
		}
	}
	public void processMessage( String $message ) {
		
		String[] tokens = $message.split( ":", 2 );
		String mnemonic = tokens[0];
		String argument = tokens.length > 1 ? tokens[1] : null;
		Method handler = _handlers.get( mnemonic );
		if ( handler != null ) {
			try {
				handler.invoke( this, argument );
			} catch ( java.lang.IllegalAccessException e ) {
				e.printStackTrace();
				System.exit( 1 );
			} catch ( java.lang.reflect.InvocationTargetException e ) {
				e.printStackTrace();
				System.exit( 1 );
			}
		} else {
			_browser.processMessage( $message );
		}
	}
	public void handlerDummy( String $msg ) {
		System.out.println( "Message processed by dummy handler: " + $msg + " in [HWorkArea]" );
	}
	public void cleanup() {
		_logics.clear();
	}
	public void closeParty( String $id ) {
	}
}

