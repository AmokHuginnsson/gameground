import java.lang.reflect.Method;
import java.util.SortedMap;
import java.util.TreeMap;

public abstract class HAbstractLogic {
	public HGUIface _gui;
	GameGround _app = null;
	SortedMap<String,Method> _handlers;
	HLogicInfo _info;
	String _id = "0";
	public HAbstractLogic( GameGround $app ) {
		_app = $app;
	}
	public void init( HGUIface $ext ) {
		_gui = $ext;
		_gui.init();
		_handlers = java.util.Collections.synchronizedSortedMap( new TreeMap<String,Method>() );
		try {
			_handlers.put( "msg", HAbstractLogic.class.getDeclaredMethod( "handleMessage", new Class[]{ String.class } ) );
			_handlers.put( "err", HAbstractLogic.class.getDeclaredMethod( "handleError", new Class[]{ String.class } ) );
		} catch ( java.lang.NoSuchMethodException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public abstract void reinit();
	public abstract void cleanup();
	public HGUIface getGUI() {
		return ( _gui );
	}
	public HLogicInfo getInfo() {
		return ( _info );
	}
	public void setId( String $id ) {
		_id = $id;
	}
	public void setDefaults( String $defaults ) {
		_info._defaults = $defaults;
	}
	public void handleMessage( String $message ) {
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
		_gui.log( HGUIface.Colors.NORMAL );
	}
	public void handleError( String $message ) {
		javax.swing.JOptionPane.showMessageDialog( _gui,
				"The GameGround server reported error condition:\n" + $message,
				"GameGround - error ...", javax.swing.JOptionPane.ERROR_MESSAGE );
		if ( HBrowser.LABEL.equals( getInfo()._face ) )
			_app.getClient().disconnect();
		else
			_app.setFace( HBrowser.LABEL );
	}
	public void processMessage( String $message ) {
		String[] toks = $message.split( ":", 2 );
		if ( "party".equals( toks[0] ) ) {
			String[] p = $message.split( ",", 2 );
			$message = p[1];
			System.out.println( $message );
		}
		
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
			System.out.println( "Unhandled mnemonic: [" + mnemonic + "], in [" + getInfo()._face + "] while processing message: " + $message );
			System.exit( 0 );
		}
	}
	public void handlerDummy( String $msg ) {
		System.out.println( "Message processed by dummy handler: " + $msg + " in [" + getInfo()._face + "]" );
	}
}
