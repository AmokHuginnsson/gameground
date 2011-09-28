import java.lang.reflect.Method;
import java.util.SortedMap;
import java.util.TreeMap;

public abstract class HAbstractLogic {
	public static class PROTOCOL {
		public static final String SEP = ":";
		public static final String SEPP = ",";
		public static final String CMD = "cmd";
		public static final String SAY = "say";
	}
	public HGUIface _gui;
	GameGround _app = null;
	HClient _client = null;
	SortedMap<String,Method> _handlers;
	String _id = "0";
	String _configuration;
	java.text.SimpleDateFormat _timestampFormat = new java.text.SimpleDateFormat( "[HH:mm:ss] " );

	public HAbstractLogic( GameGround $app ) {
		_app = $app;
		_client = $app.getClient();
	}
	public HAbstractLogic( GameGround $app, String $id, String $configuration ) {
		this( $app );
		_id = $id;
		_configuration = $configuration;
	}
	public void init( HGUIface $ext ) {
		_gui = $ext;
		_gui.init();
		_handlers = java.util.Collections.synchronizedSortedMap( new TreeMap<String,Method>() );
		try {
			_handlers.put( "msg", HAbstractLogic.class.getDeclaredMethod( "handleMessage", new Class[]{ String.class } ) );
			_handlers.put( "say", HAbstractLogic.class.getDeclaredMethod( "handleSay", new Class[]{ String.class } ) );
			_handlers.put( "err", HAbstractLogic.class.getDeclaredMethod( "handleError", new Class[]{ String.class } ) );
		} catch ( java.lang.NoSuchMethodException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public abstract void cleanup();
	public HGUIface getGUI() {
		return ( _gui );
	}
	public String id() {
		return ( _id );
	}
	public void handleSay( String $message ) {
		_gui.log( _timestampFormat.format( java.util.Calendar.getInstance().getTime() ) );
		handleMessage( $message );
	}
	int findColor( String $message, int $offset ) {
		int colorStartIndex = -1;
		int start = $offset;
		int length = $message.length();
		while ( start < length ) {
			start = $message.indexOf( '$', start );
			if ( start < 0 )
				break;
			int color = start + 1;
			while ( ( color < length ) && Character.isDigit( $message.charAt( color ) ) )
				++ color;
			if ( ( color < length ) && ( $message.charAt( color ) == ';' ) ) {
				colorStartIndex = start;
				break;
			}
			start = color;
		}
		return ( colorStartIndex );
	}
	public void handleMessage( String $message ) {
		int index = 0;
		int length = $message.length();
		while ( index < length ) {
			int color = findColor( $message, index );
			if ( color > index )
				_gui.log( $message.substring( index, color ) );
			if ( color >= 0 ) { /* color */
				try {
					int colorEnd = $message.indexOf( ';', color );
					_gui.log( new Integer( $message.substring( color + 1, colorEnd ) ).intValue() );
					index = colorEnd + 1;
				} catch ( NumberFormatException e ) {
					e.printStackTrace();
				}
			}	else { /* text */
				_gui.log( $message.substring( index ) );
				break;
			}
		}
		_gui.log( "\n" );
		_gui.log( _gui.COLOR_NORMAL );
	}
	public void handleError( String $message ) {
		javax.swing.JOptionPane.showMessageDialog( _gui,
				"The GameGround server reported error condition:\n" + $message,
				"GameGround - error ...", javax.swing.JOptionPane.ERROR_MESSAGE );
		assert false : "To be implemented";
	}
	public void processMessage( String $message ) {
		String[] tokens = $message.split( ":", 2 );
		String mnemonic = tokens[0];
		String argument = tokens.length > 1 ? tokens[1] : null;
		Method handler = _handlers.get( mnemonic );
		if ( handler != null ) {
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
			System.out.println( "Unhandled mnemonic: [" + mnemonic + "], in [" + _configuration + "] while processing message: " + $message );
			System.exit( 0 );
		}
	}
	public void handlerDummy( String $msg ) {
		System.out.println( "Message processed by dummy handler: " + $msg + " in [" + _configuration + "]" );
	}
}
