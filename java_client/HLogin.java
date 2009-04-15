import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.Container;
import java.awt.Color;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.StyleConstants;
import javax.swing.text.Style;
import javax.swing.text.SimpleAttributeSet; 

class HLogin extends HAbstractLogic {
	public static final String LABEL = "login";
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _login;
		public JTextField _password;
		public JTextField _server;
		public JTextField _port;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void reinit() {
			_login.requestFocusInWindow();
			try {
				int port = Integer.parseInt( _app.getParameter("port") );
				if ( port >= 1024 )
					_port.setText( "" + port );
			} catch ( Exception e ) {
			}
			_login.setText( _app.getParameter( "login" ) );
			String host = _app.getParameter( "host" );
			if ( host != null )
				_server.setText( host );
			if ( _app.isApplet() ) {
				_server.setEditable( false );
				_port.setEditable( false );
			}
		}
		public void onConnectClick() {
			HLogin.this.onConnectClick();
		}
		public void onExit() {
			_app.shutdown();
		}
		public Action onEnter = new AbstractAction() {
			public static final long serialVersionUID = 17l;
			public void actionPerformed( ActionEvent $event ) {
				onConnectClick();
			}
		};
		public JTextPane getLogPad() { return ( null ); }
		public void updateTagLib( XUL $xul ) {	}
	}
	public class OConnectionConfig {
		public String _login;
		public String _password;
		public String _host;
		public int _port;
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	HGUILocal _gui;
	OConnectionConfig _connectionConfig;
//--------------------------------------------//
	public HLogin( GameGround $applet ) throws Exception {
		super( $applet );
		_connectionConfig = new OConnectionConfig();
		init( _gui = new HGUILocal( LABEL ) );
		String serverAddress = "";
		try {
			serverAddress = $applet.getCodeBase().getHost();
		} catch ( java.lang.NullPointerException e ) {
		} finally {
			if ( serverAddress.compareTo( "" ) == 0 )
				serverAddress = "127.0.0.1";
		}
		_gui._server.setText( serverAddress );
	}
	void onConnectClick() {
		String errors = "";
		String login = "";
		if ( _gui._login.getText().compareTo( "" ) == 0 )
			errors += "name not set\n";
		else
			login = new String( _gui._login.getText() );
		String password = "";
		if ( _gui._password.getText().compareTo( "" ) == 0 )
			errors += "password not set\n";
		else
			password = new String( _gui._password.getText() );
		String server = "";
		if ( _gui._server.getText().compareTo( "" ) == 0 )
			errors += "server not set\n";
		else
			server = new String( _gui._server.getText() );
		int port = -1;
		try {
			port = new Integer( _gui._port.getText() ).intValue();
		} catch ( NumberFormatException e ) {
		}
		if ( port <= 1024 )
			errors += "invalid port number (must be over 1024)";
		if( errors.compareTo( "" ) != 0 ) {
			JOptionPane.showMessageDialog( _gui,
					"Your setup contains following errors:\n" + errors,
					"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
		} else {
			_connectionConfig._login = login;
			_connectionConfig._password = password;
			_connectionConfig._host = server;
			_connectionConfig._port = port;
			_app.setFace( HBrowser.LABEL );
		}
	}
	public OConnectionConfig getConnectionConfig() {
		return ( _connectionConfig );
	}
	public void reinit() {
	}
	public void cleanup() {}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( LABEL, new HLogin( $app ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

