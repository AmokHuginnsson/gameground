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
	SortedMap<String,Method> _handlers;
	String _name;
	BufferedReader _in;
	PrintWriter _out;
	Socket _socket;
//--------------------------------------------//
	public HClient() throws Exception {
//		_handlers = java.util.Collections.synchronizedSortedMap( new TreeMap<String,Method>() );
//		_handlers.put( "MSG", HClient.class.getDeclaredMethod( "handlerMessage", new Class[]{ String.class } ) );
		_loop = true;
		System.out.println( "HClient" );
	}
	void connect( String $server, int $port ) throws Exception {
		_socket = new Socket( $server, $port );
		_out = new PrintWriter( _socket.getOutputStream(), true );
		_in = new BufferedReader( new InputStreamReader( _socket.getInputStream() ) );
		_out.println( "name:" + _name );
		System.out.println( "connect" );
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
			try {
				wait();
			} catch ( java.lang.InterruptedException e ) {
				e.printStackTrace();
				System.exit( 1 );
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
}

