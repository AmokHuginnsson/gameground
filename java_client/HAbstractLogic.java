import java.lang.reflect.Method;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Collections;
import javax.swing.JPanel;
import org.swixml.SwingEngine;
import org.swixml.TagLibrary;
import javax.swing.JTextPane;
import java.awt.Color;
import javax.swing.text.SimpleAttributeSet; 
import javax.swing.text.StyleConstants;
import javax.swing.text.DefaultStyledDocument;

abstract class HGUIface extends JPanel {
	public static final long serialVersionUID = 17l;
	DefaultStyledDocument _log;
	public JTextPane _logPad;
	public Color[] _colors;
	public SimpleAttributeSet _attribute;
	public int _color;
	public String _resource;
	public static class Colors {
		public static int NORMAL = 12;
		public static int WHITE = 15;
	}
	public HGUIface( String $resource ) {
		_resource = $resource;
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
	public abstract void updateTagLib( SwingEngine $se );
	public abstract void reinit();
	public abstract JTextPane getLogPad();
	public void init() {
		try {
			String res = "/res/" + _resource + ".xml";
			System.out.println( "Loading resources: " + res );
			SwingEngine se = new SwingEngine( this );
			updateTagLib( se );
			se.insert( AppletJDOMHelper.loadResource( res, this ), this );
		} catch ( java.lang.Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		if ( getLogPad() != null ) {
			_logPad = getLogPad();
			_log = ( DefaultStyledDocument )_logPad.getStyledDocument();
		} else
			System.out.println( "No logPad for this face." );
	}
	void log( String $message, int $color ) {
		try {
			if ( $color > 15 )
				$color = 15;
			StyleConstants.setForeground( _attribute, _colors[ $color ] );
			_log.insertString( _log.getLength(), $message, _attribute );
			_logPad.setCaretPosition( _log.getLength() );
		} catch ( javax.swing.text.BadLocationException e ) {
			e.printStackTrace();
		}
	}
	void log( String $message ) {
		try {
			StyleConstants.setForeground( _attribute, _colors[ _color ] );
			_log.insertString( _log.getLength(), $message, _attribute );
			_logPad.setCaretPosition( _log.getLength() );
		} catch ( javax.swing.text.BadLocationException e ) {
			e.printStackTrace();
		}
	}
	void log( int $color ) {
		if ( $color > 15 )
			$color = 15;
		_color = $color;
	}
}

public abstract class HAbstractLogic {
	public HGUIface _gui;
	SortedMap<String,Method> _handlers;

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
	public HGUIface getGUI() {
		return ( _gui );
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
		GameGround gg = GameGround.getInstance();
		javax.swing.JOptionPane.showMessageDialog( _gui,
				"The GameGround server reported error condition:\n" + $message,
				"GameGround - error ...", javax.swing.JOptionPane.ERROR_MESSAGE );
		gg.setFace( HLogin.LABEL );
		gg.getClient().disconnect();
	}
	public void processMessage( String $message ) {
		String tokens[] = new String[2];
		tokens = $message.split( ":", 2 );
		String mnemonic = tokens[0];
		String argument = tokens[1];
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
			System.out.println( "Unhandled mnemonic: [" + mnemonic + "], then processing message: " + $message );
			System.exit( 0 );
		}
	}
}
