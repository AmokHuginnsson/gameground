import java.lang.reflect.Method;
import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
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

class HSystemNames {
	static private String[] _systemNamesLatin = {
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
	static private String[] _systemNamesNorse = {
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
	public final static int NORSE = 1;
	public final static int LATIN = 2;
	public static String[] getNames( int $set ) {
		if ( $set == NORSE )
			return ( _systemNamesNorse );
		return ( _systemNamesLatin );
	}
}

class HGalaxy extends HAbstractLogic implements ActionListener, KeyListener {
	enum State {
		NORMAL,
		SELECT,
		INPUT,
		LOCKED
	}
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		/* This is one dirty hack.
		 * To make it work you have to edit org/swixml/SwingEngine.java
		 * and comment out all setAccessible() invocations.
		 */
		public JTextField _fleet;
		public JTextField _messageInput;
		public JTextPane _logPad;
		public JButton _endRound;
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
		public int[] _colorMap;
		public HGUILocal( String $resource ) {
			super( $resource );
			_colorMap = new int[ 20 ];
			_colorMap[ 0 ] = Colors.BRIGHTBLUE;
			_colorMap[ 1 ] = Colors.BRIGHTGREEN;
			_colorMap[ 2 ] = Colors.BRIGHTRED;
			_colorMap[ 3 ] = Colors.BRIGHTCYAN;
			_colorMap[ 4 ] = Colors.BRIGHTMAGENTA;
			_colorMap[ 5 ] = Colors.YELLOW;
			_colorMap[ 6 ] = Colors.BLUE;
			_colorMap[ 7 ] = Colors.GREEN;
			_colorMap[ 8 ] = Colors.RED;
			_colorMap[ 9 ] = Colors.CYAN;
			_colorMap[ 10 ] = Colors.MAGENTA;
			_colorMap[ 11 ] = Colors.BROWN;
			_colorMap[ 12 ] = Colors.LIGHTGRAY;
			_colorMap[ 13 ] = Colors.OTHERGRAY;
			_colorMap[ 14 ] = Colors.DARKGRAY;
			_colorMap[ 15 ] = Colors.WHITE;
		}
		public void updateTagLib( SwingEngine $se ) {
			$se.getTaglib().registerTag( "hboard", HBoard.class );
		}
		public void reinit() { }
		public JTextPane getLogPad() {
			return ( _logPad );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	private static final boolean registered;
	static {
		registered = registerLogic();
	}
	public static final String LABEL = "galaxy";
	private State _state;
	int _round;
	int _color;
	int _systemCount;
	String[] _systemNames;
	PrintWriter _out;
	String _emperor;
	HashMap<Integer,String> _emperors;
	HClient _client;
	java.util.List<HMove> _moves;
	HSystem[] _systems;
 	char[] _symbols = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
		'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
	};
	public HGUILocal _gui;
//--------------------------------------------//
	public HGalaxy( GameGround $applet ) throws Exception {
		super();
		init( _gui = new HGUILocal( LABEL ) );
		_round = 0;
		_systemCount = 0;
		_emperors = new HashMap<Integer,String>();
		_handlers.put( "setup", HGalaxy.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
		_handlers.put( "glx", HGalaxy.class.getDeclaredMethod( "handlerGalaxy", new Class[]{ String.class } ) );
		_handlers.put( "play", HGalaxy.class.getDeclaredMethod( "handlerPlay", new Class[]{ String.class } ) );
		_handlers.put( "player", HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		if ( ( java.util.Calendar.getInstance().get( java.util.Calendar.HOUR_OF_DAY ) % 2 ) == 1 )
			_systemNames = HSystemNames.getNames( HSystemNames.NORSE );
		else
			_systemNames = HSystemNames.getNames( HSystemNames.LATIN );
		_info = new HLogicInfo( "glx", "galaxy", "Galaxy" );
		HImages images = new HImages();
		_gui._board.setGui( this );
		_gui._board.setImages( images );
		String serverAddress = "";
		try {
			serverAddress = $applet.getCodeBase().getHost();
		} catch ( java.lang.NullPointerException e ) {
		} finally {
			if ( serverAddress.compareTo( "" ) == 0 )
				serverAddress = "127.0.0.1";
		}
		_gui._endRound.addActionListener( this );
		$applet.addGlobalKeyListener( $applet, this );
		$applet.addGlobalKeyListener( _gui, this );
		_state = State.LOCKED;
		_moves = java.util.Collections.<HMove>synchronizedList( new java.util.LinkedList<HMove>() );
		//_out.println( "GLX:LOGIN:" + _emperor );
	}
	void handlerGalaxy( String $command ) {
		processMessage( $command );
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
				_gui._board.setSize( new Integer( value ).intValue() );
			} else if ( variable.compareTo( "systems" ) == 0 ) {
				if ( _systems != null ) {
// FIXME					_loop = false;
				} else
					_systems = new HSystem[_systemCount = new Integer( value ).intValue()];
			} else if ( variable.compareTo( "system_coordinates" ) == 0 ) {
				tokens = value.split( ",", 3 );
				index = new Integer( tokens[0] ).intValue();
				coordX = new Integer( tokens[1] ).intValue();
				coordY = new Integer( tokens[2] ).intValue();
				_systems[index] = new HSystem();
				_systems[index]._coordinateX = coordX;
				_systems[index]._coordinateY = coordY;
				if ( index == ( _systemCount - 1 ) )
					_gui._board.setSystems( _systems );
			} else if ( variable.compareTo( "emperor" ) == 0 ) {
				tokens = value.split( ",", 2 );
				index = new Integer( tokens[0] ).intValue();
				_emperors.put( index, tokens[1] );
				if ( _emperors.get( index ).compareTo( _emperor ) == 0 )
					_color = index;
			} else if ( variable.compareTo( "ok" ) == 0 ) {
				setState ( State.NORMAL );
				_gui._emperor.setForeground( _gui.color( _color ) );
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
						_gui.log( " conquered ", HGUIface.Colors.NORMAL );
						int temp = _systems[ sysNo ]._color;
						temp = ( temp >= 0 ) ? temp : HGUIface.Colors.NORMAL;
						_gui.log( _systemNames[ sysNo ], temp );
						value = "(" + _symbols[ sysNo ] + ")";
						_gui.log( value, temp );
						_gui.log( ".\n", HGUIface.Colors.NORMAL );
						_systems[ sysNo ]._color = color;
					} break;
					case ( 'r' ): { /* reinforcements */
						_gui.log( "Reinforcements for ", HGUIface.Colors.NORMAL );
						_gui.log( _systemNames[ sysNo ], color );
						value = "(" + _symbols[ sysNo ] + ")";
						_gui.log( value, color );
						_gui.log( " arrived.\n", HGUIface.Colors.NORMAL );
					} break;
					case ( 'f' ):
					case ( 's' ): { /* resisted attack */
						if ( event == 'f' ) { /* failed to conquer */
							_systems[ sysNo ]._color = color;
							color = _color;
							value = _emperors.get( color );
						}
						int temp = _systems[ sysNo ]._color;
						temp = ( temp >= 0 ) ? temp : HGUIface.Colors.NORMAL;
						_gui.log( _systemNames[ sysNo ], temp );
						variable = "(" + _symbols[ sysNo ] + ")";
						_gui.log( variable, temp );
						_gui.log( " resisted attack from ", HGUIface.Colors.NORMAL );
						_gui.log( value, color );
						_gui.log( ".\n", HGUIface.Colors.NORMAL );
					} break;
					case ( 'i' ): /* info */
					default :
						break;
				}
			} else if ( variable.compareTo( "round" ) == 0 ) {
				_gui.log( "----- ", HGUIface.Colors.WHITE );
				_gui.log( " round: ", HGUIface.Colors.NORMAL );
				_round = new Integer( value ).intValue();
				_gui.log( value + " -----\n", HGUIface.Colors.WHITE );
				_gui.log( HGUIface.Colors.NORMAL );
				setState ( State.NORMAL );
				_gui._round.setText( value );
				_gui._board.repaint();
			}
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void setState( State $state ) {
		_gui._arrival.setText( String.valueOf( _round ) );
		switch ( $state ) {
			case LOCKED:
				_gui._tips.setText( "A waiting for GameGround events ..." );
				break;
			case NORMAL:
				_gui._tips.setText( "Make Your imperial fleet moves ..." );
				break;
			case SELECT:
				_gui._tips.setText( "Select destination for Your fleet ..." );
				break;
			case INPUT:
				_gui._tips.setText( "How many destroyers You wish to send ?" );
				break;
		}
		_state = $state;
	}
	public State getState() {
		return _state;
	}
	void cleanEdit() {
		_gui._fleet.setEditable( false );
		_gui._fleet.setText( "" );
		_gui._board.requestFocus();
	}
	public void keyTyped( KeyEvent $event ) {
		if ( _gui._board._systems != null ) {
			if ( $event.getKeyChar() == KeyEvent.VK_ENTER ) {
				if ( _gui._messageInput.isFocusOwner() ) {
					String message = _gui._messageInput.getText( );
					if ( message.matches( ".*\\S+.*" ) ) {	
						_client._out.println( "GLX:SAY:" + message );
						_gui._messageInput.setText( "" );
					}
				} else {
					if ( _state == State.INPUT ) {
						int fleet = -1;
						try {
							fleet = new Integer( _gui._fleet.getText() ).intValue();
						} catch ( NumberFormatException e ) {
						} finally {
							if ( fleet < 1 ) {
								_gui._tips.setText( "Run! Run! Emperor is mad ..." );
								return;
							} else if ( fleet > _systems[ _gui._board._sourceSystem ]._fleet ) {
								_gui._tips.setText( "Not enough resources!" );
								return;
							}
						}
						HMove move = new HMove();
						move._sourceSystem = _gui._board._sourceSystem;
						move._destinationSystem = _gui._board._destinationSystem;
						move._fleet = fleet;
						_systems[ _gui._board._sourceSystem ]._fleet -= fleet;
						_moves.add( move );
						cleanEdit();
						setState( State.NORMAL );
					}
				}
			} else if ( ( $event.getKeyChar() == KeyEvent.VK_SPACE )
					&& ( ! _gui._messageInput.isFocusOwner() ) ) {
				_gui._endRound.doClick();
			} else if ( $event.getKeyChar() == KeyEvent.VK_ESCAPE ) {
				if ( _gui._messageInput.isFocusOwner() ) {
					_gui._board.requestFocus();
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
	void onEndRound() {
		_gui._board.requestFocus();
		if ( _state == State.INPUT )
			keyTyped( new KeyEvent( _gui._fleet, KeyEvent.KEY_TYPED,
						(long)0, 0, KeyEvent.VK_UNDEFINED, (char)KeyEvent.VK_ENTER ) );
		if ( _state == State.NORMAL ) {
			endRound( _moves );
		}
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == _gui._endRound ) {
			onEndRound();
		}
	}
	int getSystemCount() {
		return _systemCount;
	}
	void endRound( java.util.List<HMove> $moves ) {
		setState( State.LOCKED );
		for ( java.util.ListIterator i = $moves.listIterator(); i.hasNext(); ) {
			HMove move = (HMove)i.next();
			String message = "GLX:play:move=" + move._sourceSystem + "," + move._destinationSystem + "," + move._fleet;
			_out.println( message );
		}
		_out.println( "GLX:play:end_round" );
		$moves.clear();
	}
	static boolean registerLogic() {
		try {
			GameGround.registerLogic( LABEL, new HGalaxy( GameGround.getInstance() ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
	public void reinit() {
		_emperor = ((HLogin)GameGround.getInstance().getLogic( "login" )).getConnectionConfig()._name; /* FIXME */
		_gui._emperor.setText( _emperor );
		_gui.log( "##", 0 );_gui.log( " ##", 1 );_gui.log( " ##", 2 );
		_gui.log( " ##", 3 );_gui.log( " ##", 4 );_gui.log( " ##\n", 5 );
		_gui.log( "##", 6 );_gui.log( " ##", 7 );_gui.log( " ##", 8 );
		_gui.log( " ##", 9 );_gui.log( " ##", 10 );_gui.log( " ##\n", 11 );
		_gui.log( "##", 12 );_gui.log( " ##", 13 );_gui.log( " ##\n", 14 );
	}
}

