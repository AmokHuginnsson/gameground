import java.awt.event.ActionEvent;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JTextField;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;

class HLogin extends HAbstractWorkArea {
	public static final String LABEL = "login";
	static boolean once = true;
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _login;
		public JTextField _password;
		public JTextField _server;
		public JTextField _port;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void init() {
			super.init();
			reinit();
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
			String password = _app.getParameter( "password" );
			if ( password != null )
				_password.setText( password );
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
		public void onClose() {
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
		init( _gui = new HGUILocal( LABEL ) );
		_connectionConfig = new OConnectionConfig();
		String serverAddress = "";
		try {
			serverAddress = $applet.getCodeBase().getHost();
		} catch ( java.lang.NullPointerException e ) {
		} finally {
			if ( "".equals( serverAddress ) && ( "".equals( _app.getParameter( "host" ) ) || ( _app.getParameter( "host" ) == null ) ) )
				serverAddress = "127.0.0.1";
		}
		if ( ! "".equals( serverAddress ) )
			_gui._server.setText( serverAddress );
	}
	void onConnectClick() {
		String errors = "";
		String login = "";
		if ( _gui._login.getText().compareTo( "" ) == 0 ) {
			errors += "name not set\n";
		} else {
			login = new String( _gui._login.getText() );
		}
		String password = new String( _gui._password.getText() );
		String server = "";
		if ( _gui._server.getText().compareTo( "" ) == 0 ) {
			errors += "server not set\n";
		} else {
			server = new String( _gui._server.getText() );
		}
		int port = -1;
		try {
			port = new Integer( _gui._port.getText() ).intValue();
		} catch ( NumberFormatException e ) {
		}
		if ( port <= 1024 ) {
			errors += "invalid port number (must be over 1024)\n";
		}
		if ( ( password.length() > 0 ) && ( password.length() < 6 ) ) {
			errors += "if you want to use password it must be at least 6 characters long";
		}
		if( errors.compareTo( "" ) != 0 ) {
			JOptionPane.showMessageDialog( _gui,
					"Your setup contains following errors:\n" + errors,
					"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
		} else {
			_connectionConfig._login = login;
			_connectionConfig._password = password;
			_connectionConfig._host = server;
			_connectionConfig._port = port;
			_app.showWorkArea();
		}
	}
	public OConnectionConfig getConnectionConfig() {
		return ( _connectionConfig );
	}
	public void reinit() {
		if ( "true".equals( _app.getParameter( "auto-connect" ) ) && once ) {
			once = false;
			onConnectClick();
		}
	}
	public void cleanup() {}
}

