import java.lang.reflect.Method;
import java.lang.Class;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Collections;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.net.Socket;

class HClient extends Thread {
//--------------------------------------------//
	boolean _loop;
	HLogic _logic;
	SortedMap<String,Method> _handlers;
	HashMap<Integer,String> _emperors;
	String _name;
	BufferedReader _in;
	PrintWriter _out;
	Socket _socket;
	HSystem[] _systems;
	char[] _symbols = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
		'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
	};
	String[] _systemNames;
//--------------------------------------------//
	public static void registerLogic( HLogic $logic ) {
		return;
	}
	public HClient() throws Exception {
		_gui = $gui;
		_handlers = java.util.Collections.synchronizedSortedMap( new TreeMap<String,Method>() );
		_handlers.put( "MSG", HClient.class.getDeclaredMethod( "handlerMessage", new Class[]{ String.class } ) );
		_loop = true;
	}
	void connect( String $server, String $port ) {
		_socket = new Socket( $server, $port );
		_out = new PrintWriter( _socket.getOutputStream(), true );
		_in = new BufferedReader( new InputStreamReader( _socket.getInputStream() ) );
		_out.println( "name:" + _name );
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
				e.printStackTrace();
				System.exit( 1 );
			} catch ( java.lang.reflect.InvocationTargetException e ) {
				e.printStackTrace();
				System.exit( 1 );
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
					/* System.out.println( message ); */
				}
			}
		} catch ( java.io.IOException e ) {
		}
		System.exit( 0 );
	}
	int getSystemCount() {
		return _systemCount;
	}
	void endRound( java.util.List<HMove> $moves ) {
		_gui.setState( HGUIMain.State.LOCKED );
		for ( java.util.ListIterator i = $moves.listIterator(); i.hasNext(); ) {
			HMove move = (HMove)i.next();
			String message = "GLX:PLAY:move=" + move._sourceSystem + "," + move._destinationSystem + "," + move._fleet;
			_out.println( message );
		}
		_out.println( "GLX:PLAY:end_round" );
		$moves.clear();
	}
}

