import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.Container;
import org.swixml.SwingEngine;
import java.awt.Color;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.StyleConstants;
import javax.swing.text.Style;
import javax.swing.text.SimpleAttributeSet; 

class HGUIMain extends JPanel implements ActionListener {
	enum State {
		NORMAL,
		SELECT,
		INPUT,
		LOCKED
	}
	public static class Colors {
		public static int NORMAL = 12;
		public static int WHITE = 15;
	}
	public class HWidgets extends SwingEngine {
		/* This is one dirty hack.
		 * To make it work you have to edit org/swixml/SwingEngine.java
		 * and comment out all setAccessible() invocations.
		 */
		public JTextField _name;
		public JTextField _server;
		public JTextField _port;
		public JTextField _fleet;
		public JButton _connect;
		public JPanel _setup;
		public JPanel _main;
		public JLabel _emperor;
		public JLabel _systemInfo;
		public JLabel _emperorInfo;
		public JLabel _productionInfo;
		public JLabel _fleetInfo;
		public HBoard _board;
		public JTextPane _log;
		public Color[] _colors;
		public SimpleAttributeSet _attribute;
		public int _color;
		public HWidgets() {
			super.getTaglib().registerTag( "hboard", HBoard.class );
			_attribute = new SimpleAttributeSet();
			_color = Colors.NORMAL;
			_colors = new Color[ 16 ];
			_colors[ 0 ] = new Color( 0x80, 0x80, 0xff ); /* bright blue */
			_colors[ 1 ] = new Color( 0x80, 0xff, 0x80 ); /* bright green */
			_colors[ 2 ] = new Color( 0xff, 0x80, 0x80 ); /* bright red */
			_colors[ 3 ] = Color.cyan; /* bright cyan */
			_colors[ 4 ] = Color.magenta; /* bright magenta */
			_colors[ 5 ] = Color.yellow; /* yellow */
			_colors[ 6 ] = Color.blue; /* blue */
			_colors[ 7 ] = new Color( 0x00, 0xa0, 0x00 ); /* green */
			_colors[ 8 ] = Color.red; /* red */
			_colors[ 9 ] = new Color( 0x00, 0xa0, 0xa0 );; /* cyan */
			_colors[ 10 ] = new Color( 0xa0, 0x00, 0xa0 ); /* magenta */
			_colors[ 11 ] = new Color( 0xa0, 0xa0, 0x00 ); /* brown? */
			_colors[ 12 ] = Color.lightGray;
			_colors[ 13 ] = Color.gray;
			_colors[ 14 ] = Color.darkGray;
			_colors[ 15 ] = Color.white;
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	State _state;
	HClient _client;
	public HWidgets _widgets;
	DefaultStyledDocument _log;
//--------------------------------------------//
	public HGUIMain() throws Exception {
		_widgets = new HWidgets();
		HImages images = new HImages();
		_widgets.insert( new InputStreamReader( getClass().getResourceAsStream( "res/galaxy.xml" ) ), this );
		_widgets._board.setImages( images );
		_widgets._connect.addActionListener( this );
		_state = State.LOCKED;
	}
	void setState( State $state ) {
		_state = $state;
	}
	void log( String $message, int $color ) {
		try {
			if ( $color > 15 )
				$color = 15;
			StyleConstants.setForeground( _widgets._attribute, _widgets._colors[ $color ] );
			_log.insertString( _log.getLength(), $message, _widgets._attribute );
		} catch ( javax.swing.text.BadLocationException e ) {
			e.printStackTrace();
		}
	}
	void log( String $message ) {
		try {
			StyleConstants.setForeground( _widgets._attribute, _widgets._colors[ _widgets._color ] );
			_log.insertString( _log.getLength(), $message, _widgets._attribute );
		} catch ( javax.swing.text.BadLocationException e ) {
			e.printStackTrace();
		}
	}
	void log( int $color ) {
		if ( $color > 15 )
			$color = 15;
		_widgets._color = $color;
	}
	void initBoard( String $emperor, String $server, int $port ) {
		try {
			_client = new HClient( this, $emperor, $server, $port );
			_client.start();
		} catch ( Exception e ) {
			JOptionPane.showMessageDialog( this,
					"Galaxy client was unable to connect to server:\n" + e.getMessage(),
					"Galaxy - error ...", JOptionPane.ERROR_MESSAGE );
			return;
		}
		_widgets._setup.setVisible( false );
		_widgets._setup = null;
		_widgets._main.setVisible( true );
		_widgets._emperor.setText( $emperor );
		_widgets._board.setGui( this );
		_log = ( DefaultStyledDocument )_widgets._log.getStyledDocument();
//		log( "##", 0 );log( " ##", 1 );log( " ##", 2 );log( " ##", 3 );log( " ##", 4 );log( " ##\n", 5 );
//		log( "##", 6 );log( " ##", 7 );log( " ##", 8 );log( " ##", 9 );log( " ##", 10 );log( " ##\n", 11 );
//		log( "##", 12 );log( " ##", 13 );log( " ##\n", 14 );
		synchronized ( _client ) {
			_client.notify();
		}
		_widgets._emperor.setForeground( _widgets._colors[ _client._color ] );
	}
	void onConnectClick() {
		String errors = "";
		String emperor = "";
		if ( _widgets._name.getText().compareTo( "" ) == 0 )
			errors += "name not set\n";
		else
			emperor = new String( _widgets._name.getText() );
		String server = "";
		if ( _widgets._server.getText().compareTo( "" ) == 0 )
			errors += "server not set\n";
		else
			server = new String( _widgets._server.getText() );
		int port = -1;
		try {
			port = new Integer( _widgets._port.getText() ).intValue();
		} catch ( NumberFormatException e ) {
		}
		if ( port <= 1024 )
			errors += "invalid port number (must be over 1024)";
		if( errors.compareTo( "" ) != 0 ) {
			JOptionPane.showMessageDialog( this,
					"Your setup contains following errors:\n" + errors,
					"Galaxy - error ...", JOptionPane.ERROR_MESSAGE );
		} else {
			initBoard( emperor, server, port );
		}
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == _widgets._connect ) {
			onConnectClick();
		}
	}
}

