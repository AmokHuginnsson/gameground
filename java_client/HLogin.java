import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.Container;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.StyleConstants;
import javax.swing.text.Style;
import javax.swing.text.SimpleAttributeSet; 

class HLogin extends HAbstractLogic implements ActionListener, KeyListener {
	public static final String LABEL = "login";
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _name;
		public JTextField _server;
		public JTextField _port;
		public JButton _connect;
		public JPanel _login;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void reinit() {
			_name.requestFocusInWindow();
		}
		public JTextPane getLogPad() { return ( null ); }
		public void updateTagLib( org.swixml.SwingEngine $se ) {	}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	private static final boolean registered;
	static {
		registered = registerLogic();
	}
	HClient _client;
	HGUILocal _gui;
//--------------------------------------------//
	public HLogin( GameGround $applet ) throws Exception {
		super();
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
		_gui._connect.addActionListener( this );
		$applet.addGlobalKeyListener( $applet, this );
		$applet.addGlobalKeyListener( _gui, this );
	}
	public void keyTyped( KeyEvent $event ) {
	}
	public void keyPressed( KeyEvent $event ) {
	}
	public void keyReleased( KeyEvent $event ) {
	}
	void onConnectClick() {
		String errors = "";
		String login = "";
		if ( _gui._name.getText().compareTo( "" ) == 0 )
			errors += "name not set\n";
		else
			login = new String( _gui._name.getText() );
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
/* FIXME			try {
				_client = new HClient( this, login, $server, $port );
				_client.start();
			} catch ( Exception e ) {
				JOptionPane.showMessageDialog( this,
						"GameGround client was unable to connect to server:\n" + e.getMessage(),
						"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
				return;
			} */
			GameGround.getInstance().setFace( HBrowser.LABEL );
		}
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == _gui._connect ) {
			onConnectClick();
		}
	}
	static boolean registerLogic() {
		try {
			GameGround.registerLogic( LABEL, new HLogin( GameGround.getInstance() ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

