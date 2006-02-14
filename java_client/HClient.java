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
	Map<String,Method> _handlers;
	String _emperor;
	BufferedReader _in;
	PrintWriter _out;
	Socket _socket;
//--------------------------------------------//
	HClient( String $emperor, String $server, int $port ) throws Exception {
		_emperor = $emperor;
		_handlers = new HashMap<String,Method>();
		_handlers.put( "SETUP", HClient.class.getDeclaredMethod( "handlerSetup", new Class[]{ String.class } ) );
		_handlers.put( "PLAY", HClient.class.getDeclaredMethod( "handlerPlay", new Class[]{ String.class } ) );
		_handlers.put( "MSG", HClient.class.getDeclaredMethod( "handlerMessage", new Class[]{ String.class } ) );
		_socket = new Socket( $server, $port );
		_out = new PrintWriter( _socket.getOutputStream(), true );
		_in = new BufferedReader( new InputStreamReader( _socket.getInputStream() ) );
		_out.println( "GLX:LOGIN:" + _emperor );
		_loop = true;
	}
	void handlerSetup( String $command ) {
		System.out.println( "handlerSetup: " + $command );
	}
	void handlerPlay( String $command ) {
		System.out.println( "handlerPlay: " + $command );
	}
	void handlerMessage( String $message ) {
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
	}
}

