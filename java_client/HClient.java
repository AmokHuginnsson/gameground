import java.lang.reflect.Method;
import java.lang.Class;
import java.util.Map;
import java.util.HashMap;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.net.Socket;

class HClient extends Thread {
//--------------------------------------------//
	boolean _loop;
	int _color;
	int _systemCount;
	int _round;
	Map<String,Method> _handlers;
	Map<Integer,String> _emperors;
	String _emperor;
	BufferedReader _in;
	PrintWriter _out;
	Socket _socket;
	HSystem[] _systems;
	HGUIMain _gui;
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
	char[] _symbols = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
		'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
	};
	String[] _systemNames;
//--------------------------------------------//
	public HClient( HGUIMain $gui, String $emperor, String $server, int $port ) throws Exception {
		_round = -1;
		_systemCount = 0;
		_gui = $gui;
		_emperor = $emperor;
		_handlers = new HashMap<String,Method>();
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
	void handlerSetup( String $command ) {
		System.out.println( "handlerSetup: " + $command );
		int index = - 1, coordX = - 1, coordY = - 1;
		String variable;
		String value;
		String[] tokens = new String[3];
		tokens = $command.split ( "=", 2 );
		variable = tokens[0];
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
				_round = 0;
			}
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void handlerPlay( String $command ) {
		System.out.println( "handlerPlay: " + $command );
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
						break;
					}
					case ( 'r' ): { /* reinforcements */
						_gui.log( "Reinforcements for ", HGUIMain.Colors.NORMAL );
						_gui.log( _systemNames[ sysNo ], color );
						value = "(" + _symbols[ sysNo ] + ")";
						_gui.log( value, color );
						_gui.log( " arrived.\n", HGUIMain.Colors.NORMAL );
						break;
					}
					case ( 'f' ): { /* failed to conquer */
						_systems[ sysNo ]._color = color;
						color = _color;
						value = _emperors.get( color );
					}
					case ( 's' ): { /* resisted attack */
						_gui.log( _systemNames[ sysNo ], _systems[ sysNo ]._color );
						variable = "(" + _symbols[ sysNo ] + ")";
						_gui.log( variable, _systems[ sysNo ]._color );
						_gui.log( " resisted attack from ", HGUIMain.Colors.NORMAL );
						_gui.log( value, color );
						_gui.log( ".\n", HGUIMain.Colors.NORMAL );
						break;
					}
					case ( 'i' ): /* info */
					default :{
						break;
					}
				}
			} else if ( variable.compareTo( "round" ) == 0 ) {
				_gui.log( "----- ", HGUIMain.Colors.WHITE );
				_gui.log( " round: ", HGUIMain.Colors.NORMAL );
				_round = new Integer( value ).intValue();
				_gui.log( value + " -----\n", HGUIMain.Colors.WHITE );
				_gui.log( HGUIMain.Colors.NORMAL );
				_gui.setState ( HGUIMain.State.NORMAL );
			}
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void handlerMessage( String $message ) {
		int index = 0, offset = 0;
		int length = $message.length();
		String part;
		while ( index < length ) {
			offset = $message.indexOf( ';', index );
			if ( offset < 0 )
				offset = length;
			part = $message.substring( index, offset );
			if ( part.length() == 0 )
				break;
			if ( part.charAt( 0 ) == '$' ) { /* color */
				try {
					_gui.log( new Integer( part.substring( 1 ) ).intValue() );
				} catch ( NumberFormatException e ) {
					e.printStackTrace();
				}
			}	else { /* text */
				_gui.log( part );
			}
			index = offset + 1;
		}
		_gui.log( "\n" );
		_gui.log( HGUIMain.Colors.NORMAL );
		System.out.println( "handlerMessage:" + $message );
	}
	void processCommand( String $command ) {
		String[] tokens = new String[2];
		tokens = $command.split( ":", 2 );
		String mnemonic = tokens[0];
		String argument = tokens[1];
		Method handler = _handlers.get( mnemonic );
		if( handler != null ) {
			try {
				handler.invoke( this, argument );
			} catch ( java.lang.IllegalAccessException e ) {
			} catch ( java.lang.reflect.InvocationTargetException e ) {
			}
		} else {
			_loop = false;
		}
	}
	public void run() {
		synchronized( this ) {
			if ( _gui._log == null ) {
				try {
					wait();
				} catch ( java.lang.InterruptedException e ) {
					e.printStackTrace();
					System.exit( 1 );
				}
			}
		}
		System.out.println( "Thread started." );
		try {
			String message = "";
			String command;
			String argument;
			String[] tokens = new String[2];
			while ( _loop && ( message = _in.readLine() ) != null ) {
				if ( message.length() > 0 ) {
					tokens = message.split( ":", 2 );
					command = tokens[0];
					argument = tokens[1];
					if ( command.compareTo( "GLX" ) == 0 )
						processCommand( argument );
					else
						break;
					System.out.println( message );
				}
			}
		} catch ( java.io.IOException e ) {
		}
		System.exit( 0 );
	}
	int getSystemCount() {
		return _systemCount;
	}
}

