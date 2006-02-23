import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
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

class HGUIMain extends JPanel implements ActionListener, KeyListener {
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
		public JButton _endRound;
		public JPanel _setup;
		public JPanel _main;
		public JLabel _emperor;
		public JLabel _systemInfo;
		public JLabel _emperorInfo;
		public JLabel _productionInfo;
		public JLabel _fleetInfo;
		public HBoard _board;
		public JTextPane _log;
		public JTextPane _tips;
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
	private State _state;
	HClient _client;
	java.util.List<HMove> _moves;
	public HWidgets _widgets;
	DefaultStyledDocument _log;
//--------------------------------------------//
	public HGUIMain( Galaxy $applet ) throws Exception {
		super();
		_widgets = new HWidgets();
		HImages images = new HImages();
		_widgets.insert( new InputStreamReader( getClass().getResourceAsStream( "res/galaxy.xml" ) ), this );
		_widgets._board.setImages( images );
		_widgets._connect.addActionListener( this );
		_widgets._endRound.addActionListener( this );
		$applet.addGlobalKeyListener( $applet, this );
		$applet.addGlobalKeyListener( this, this );
		_state = State.LOCKED;
		_moves = java.util.Collections.<HMove>synchronizedList( new java.util.LinkedList<HMove>() );
	}
	void setState( State $state ) {
		switch ( $state ) {
			case LOCKED:
				_widgets._tips.setText( "A waiting for Galaxy events ..." );
				break;
			case NORMAL:
				_widgets._tips.setText( "Make Your imperial fleet moves ..." );
				break;
			case SELECT:
				_widgets._tips.setText( "Select destination for Your fleet ..." );
				break;
			case INPUT:
				_widgets._tips.setText( "How many destroyers You wish to send ?" );
				break;
		}
		_state = $state;
	}
	public State getState() {
		return _state;
	}
	void cleanEdit() {
		_widgets._fleet.setEditable( false );
		_widgets._fleet.setText( "" );
		_widgets._board.requestFocus();
	}
	public void keyTyped( KeyEvent $event ) {
		if ( _widgets._board._systems != null ) {
			System.out.println( $event.getKeyChar() );
			if ( $event.getKeyChar() == KeyEvent.VK_ENTER ) {
				if ( _state == State.INPUT ) {
					int fleet = -1;
					try {
						fleet = new Integer( _widgets._fleet.getText() ).intValue();
					} catch ( NumberFormatException e ) {
					} finally {
						if ( fleet < 1 ) {
							_widgets._tips.setText( "Run! Run! Emperor is mad ..." );
							return;
						} else if ( fleet > _client._systems[ _widgets._board._sourceSystem ]._fleet ) {
							_widgets._tips.setText( "Not enough resources!" );
							return;
						}
					}
					HMove move = new HMove();
					move._sourceSystem = _widgets._board._sourceSystem;
					move._destinationSystem = _widgets._board._destinationSystem;
					move._fleet = fleet;
					_client._systems[ _widgets._board._sourceSystem ]._fleet -= fleet;
					_moves.add( move );
					cleanEdit();
					setState( State.NORMAL );
				}
			} else if ( $event.getKeyChar() == KeyEvent.VK_SPACE ) {
				_widgets._endRound.doClick();
			} else if ( $event.getKeyChar() == KeyEvent.VK_ESCAPE ) {
				if ( _state == State.INPUT ) {
					cleanEdit();
					setState( State.SELECT );
				} else if ( _state == State.SELECT ) {
					setState( State.NORMAL );
				}
			}
		}
	}
	public void keyPressed( KeyEvent $event ) {
	}
	public void keyReleased( KeyEvent $event ) {
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
	void onEndRound() {
		if ( _state == State.INPUT )
			keyTyped( new KeyEvent( _widgets._fleet, KeyEvent.KEY_TYPED,
						(long)0, 0, KeyEvent.VK_UNDEFINED, (char)KeyEvent.VK_ENTER ) );
		if ( _state == State.NORMAL ) {
			_client.endRound( _moves );
		}
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == _widgets._connect ) {
			onConnectClick();
		} else if ( source == _widgets._endRound ) {
			onEndRound();
		}
	}
}

