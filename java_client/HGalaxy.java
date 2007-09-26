import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.Container;
import org.swixml.SwingEngine;
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

class HSystemNames {
	private String[] _systemNamesLatin = {
		"Aldebaran",
		"Betelgeuse",
		"Canis Major",
		"Deneb",
		"Eridanus",
		"Fomalhaut",
		"Gemini",
		"Hydra",
		"Izar",
		"Jabhat al Akrab",
		"Kochab",
		"Lupus",
		"Monoceros",
		"Norma",
		"Orion",
		"Procyon",
		"Quantum",
		"Reticulum",
		"Sirius",
		"Taurus",
		"Ursa Minor",
		"Vega",
		"Warrior",
		"Xerkses",
		"Yarn",
		"Zubenelgenubi",
		"Primum",
		"Secundum",
		"Tertium",
		"Quartum",
		"Quintum",
		"Sextum",
		"Septimum",
		"Octavum",
		"Nonum",
		"Decimum"
	};
	private String[] _systemNamesNorse = {
		"Aegir",
		"Balder",
		"C-Frey",
		"D-Tyr",
		"E-Frigg",
		"Freya",
		"Gullveig",
		"Hel",
		"Idun",
		"Jord",
		"Kvasir",
		"Loki",
		"Magni",
		"Njord",
		"Odin",
		"P-Forseti",
		"Q-Hod",
		"Ran",
		"Skadi",
		"Thor",
		"Ull",
		"Ve",
		"W-Vidar",
		"X-Sif",
		"Ymir",
		"Z-Heimdall",
		"Asgard",
		"Vanaheim",
		"Alfheim",
		"Jotunheim",
		"Hrimthursheim",
		"Muspellheim",
		"Midgard",
		"Svartalfheim",
		"Niflheim",
		"Yggdrasil"
	};
}

class HGalaxy extends HAbstractLogic implements ActionListener, KeyListener {
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
	public class HGUIface extends SwingEngine {
		/* This is one dirty hack.
		 * To make it work you have to edit org/swixml/SwingEngine.java
		 * and comment out all setAccessible() invocations.
		 */
		public JTextField _fleet;
		public JTextField _messageInput;
		public JButton _endRound;
		public JPanel _main;
		public JLabel _emperor;
		public JLabel _systemInfo;
		public JLabel _emperorInfo;
		public JLabel _productionInfo;
		public JLabel _fleetInfo;
		public JLabel _round;
		public JLabel _arrival;
		public HBoard _board;
		public JTextPane _log;
		public JTextPane _tips;
		public Color[] _colors;
		public SimpleAttributeSet _attribute;
		public int _color;
		public HGUIface() {
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
	public HGUIface _gui;
	DefaultStyledDocument _log;
//--------------------------------------------//
	public HGalaxy( String $emperor ) throws Exception {
		_round = 0;
		_systemCount = 0;
		_emperor = $emperor;
		_handlers = java.util.Collections.synchronizedSortedMap( new TreeMap<String,Method>() );
		_emperors = new HashMap<Integer,String>();
		_handlers.put( "SETUP", HClient.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
		_handlers.put( "PLAY", HClient.class.getDeclaredMethod( "handlerPlay", new Class[]{ String.class } ) );
		_handlers.put( "MSG", HClient.class.getDeclaredMethod( "handlerMessage", new Class[]{ String.class } ) );
		_socket = new Socket( $server, $port );
		_out = new PrintWriter( _socket.getOutputStream(), true );
		_in = new BufferedReader( new InputStreamReader( _socket.getInputStream() ) );
		_out.println( "GLX:LOGIN:" + _emperor );
		if ( ( java.util.Calendar.getInstance().get( java.util.Calendar.HOUR_OF_DAY ) % 2 ) == 1 )
			_systemNames = _systemNamesNorse;
		else
			_systemNames = _systemNamesLatin;
		_loop = true;
	}
	public HGalaxy( GameGround $applet ) throws Exception {
		super();
		_gui = new HGUIface();
		HImages images = new HImages();
		_widgets.insert( new InputStreamReader( getClass().getResourceAsStream( "res/gameground.xml" ) ), this );
		_widgets._board.setImages( images );
		String serverAddress = "";
		try {
			serverAddress = $applet.getCodeBase().getHost();
		} catch ( java.lang.NullPointerException e ) {
		} finally {
			if ( serverAddress.compareTo( "" ) == 0 )
				serverAddress = "127.0.0.1";
		}
		_widgets._server.setText( serverAddress );
		_widgets._connect.addActionListener( this );
		_widgets._endRound.addActionListener( this );
		$applet.addGlobalKeyListener( $applet, this );
		$applet.addGlobalKeyListener( this, this );
		_state = State.LOCKED;
		_moves = java.util.Collections.<HMove>synchronizedList( new java.util.LinkedList<HMove>() );
	}
	void handlerSetup( String $command ) {
		int index = - 1, coordX = - 1, coordY = - 1;
		String variable;
		String value = "";
		String[] tokens = new String[3];
		tokens = $command.split ( "=", 2 );
		variable = tokens[0];
		if ( java.lang.reflect.Array.getLength( tokens ) > 1 )
			value = tokens[1];
		try {
			if ( variable.compareTo( "board_size" ) == 0 ) {
				_gui._widgets._board.setSize( new Integer( value ).intValue() );
			} else if ( variable.compareTo( "systems" ) == 0 ) {
				if ( _systems != null )
					_loop = false;
				else {
					_systems = new HSystem[_systemCount = new Integer( value ).intValue()];
				}
			} else if ( variable.compareTo( "system_coordinates" ) == 0 ) {
				tokens = value.split( ",", 3 );
				index = new Integer( tokens[0] ).intValue();
				coordX = new Integer( tokens[1] ).intValue();
				coordY = new Integer( tokens[2] ).intValue();
				_systems[index] = new HSystem();
				_systems[index]._coordinateX = coordX;
				_systems[index]._coordinateY = coordY;
				if ( index == ( _systemCount - 1 ) )
					_gui._widgets._board.setSystems( _systems );
			} else if ( variable.compareTo( "emperor" ) == 0 ) {
				tokens = value.split( ",", 2 );
				index = new Integer( tokens[0] ).intValue();
				_emperors.put( index, tokens[1] );
				if ( _emperors.get( index ).compareTo( _emperor ) == 0 )
					_color = index;
			} else if ( variable.compareTo( "ok" ) == 0 ) {
				_gui.setState ( HGUIMain.State.NORMAL );
				_gui._widgets._emperor.setForeground( _gui._widgets._colors[ _color ] );
				_round = 0;
			}
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void handlerPlay( String $command ) {
		char event = 0;
		int sysNo = - 1, color = 0, production = - 1;
		String variable;
		String value;
		String[] tokens = new String[5];
		tokens = $command.split ( "=", 2 );
		variable = tokens[0];
		value = tokens[1];
		try {
			if ( variable.compareTo( "system_info" ) == 0 ) {
				event = value.charAt( 0 );
				tokens = value.split ( ",", 5 );
				sysNo = new Integer( tokens[1] ).intValue();
				production = new Integer( tokens[2] ).intValue();
				if ( production >= 0 )
					_systems[ sysNo ]._production = production;
				_systems[ sysNo ]._fleet = new Integer( tokens[4] ).intValue();
				color = new Integer( tokens[3] ).intValue();
				value = _emperors.get( color );
				switch ( event ) {
					case ( 'c' ): /* conquered */
					case ( 'd' ): { /* defeted */
						_gui.log( value, color );
						_gui.log( " conquered ", HGUIMain.Colors.NORMAL );
						int temp = _systems[ sysNo ]._color;
						temp = ( temp >= 0 ) ? temp : HGUIMain.Colors.NORMAL;
						_gui.log( _systemNames[ sysNo ], temp );
						value = "(" + _symbols[ sysNo ] + ")";
						_gui.log( value, temp );
						_gui.log( ".\n", HGUIMain.Colors.NORMAL );
						_systems[ sysNo ]._color = color;
					} break;
					case ( 'r' ): { /* reinforcements */
						_gui.log( "Reinforcements for ", HGUIMain.Colors.NORMAL );
						_gui.log( _systemNames[ sysNo ], color );
						value = "(" + _symbols[ sysNo ] + ")";
						_gui.log( value, color );
						_gui.log( " arrived.\n", HGUIMain.Colors.NORMAL );
					} break;
					case ( 'f' ):
					case ( 's' ): { /* resisted attack */
						if ( event == 'f' ) { /* failed to conquer */
							_systems[ sysNo ]._color = color;
							color = _color;
							value = _emperors.get( color );
						}
						int temp = _systems[ sysNo ]._color;
						temp = ( temp >= 0 ) ? temp : HGUIMain.Colors.NORMAL;
						_gui.log( _systemNames[ sysNo ], temp );
						variable = "(" + _symbols[ sysNo ] + ")";
						_gui.log( variable, temp );
						_gui.log( " resisted attack from ", HGUIMain.Colors.NORMAL );
						_gui.log( value, color );
						_gui.log( ".\n", HGUIMain.Colors.NORMAL );
					} break;
					case ( 'i' ): /* info */
					default :
						break;
				}
			} else if ( variable.compareTo( "round" ) == 0 ) {
				_gui.log( "----- ", HGUIMain.Colors.WHITE );
				_gui.log( " round: ", HGUIMain.Colors.NORMAL );
				_round = new Integer( value ).intValue();
				_gui.log( value + " -----\n", HGUIMain.Colors.WHITE );
				_gui.log( HGUIMain.Colors.NORMAL );
				_gui.setState ( HGUIMain.State.NORMAL );
				_gui._widgets._round.setText( value );
				_gui._widgets._board.repaint();
			}
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void setState( State $state ) {
		_widgets._arrival.setText( String.valueOf( _client._round ) );
		switch ( $state ) {
			case LOCKED:
				_widgets._tips.setText( "A waiting for GameGround events ..." );
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
			if ( $event.getKeyChar() == KeyEvent.VK_ENTER ) {
				if ( _widgets._messageInput.isFocusOwner() ) {
					String message = _widgets._messageInput.getText( );
					if ( message.matches( ".*\\S+.*" ) ) {	
						_client._out.println( "GLX:SAY:" + message );
						_widgets._messageInput.setText( "" );
					}
				} else {
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
				}
			} else if ( ( $event.getKeyChar() == KeyEvent.VK_SPACE )
					&& ( ! _widgets._messageInput.isFocusOwner() ) ) {
				_widgets._endRound.doClick();
			} else if ( $event.getKeyChar() == KeyEvent.VK_ESCAPE ) {
				if ( _widgets._messageInput.isFocusOwner() ) {
					_widgets._board.requestFocus();
				} else {
					if ( _state == State.INPUT ) {
						cleanEdit();
						setState( State.SELECT );
					} else if ( _state == State.SELECT ) {
						setState( State.NORMAL );
					}
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
			_widgets._log.setCaretPosition( _log.getLength() );
		} catch ( javax.swing.text.BadLocationException e ) {
			e.printStackTrace();
		}
	}
	void log( String $message ) {
		try {
			StyleConstants.setForeground( _widgets._attribute, _widgets._colors[ _widgets._color ] );
			_log.insertString( _log.getLength(), $message, _widgets._attribute );
			_widgets._log.setCaretPosition( _log.getLength() );
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
					"GameGround client was unable to connect to server:\n" + e.getMessage(),
					"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
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
					"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
		} else {
			initBoard( emperor, server, port );
		}
	}
	void onEndRound() {
		_widgets._board.requestFocus();
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

