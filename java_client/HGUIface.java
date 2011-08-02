import javax.swing.JPanel;
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
	public static final class Colors {
		public static final int BLACK = 0;
		public static final int RED = 1;
		public static final int GREEN = 2;
		public static final int BROWN = 3;
		public static final int BLUE = 4;
		public static final int MAGENTA = 5;
		public static final int CYAN = 6;
		public static final int LIGHTGRAY = 7;
		public static final int DARKGRAY = 8;
		public static final int BRIGHTRED = 9;
		public static final int BRIGHTGREEN = 10;
		public static final int YELLOW = 11;
		public static final int BRIGHTBLUE = 12;
		public static final int BRIGHTMAGENTA = 13;
		public static final int BRIGHTCYAN = 14;
		public static final int WHITE = 15;
		public static final int OTHERGRAY = 16;
		public static final int NORMAL = 7;
		public static final int PALLETE_SIZE = 20;
	}
	public HGUIface( String $resource ) {
		_resource = $resource;
		_attribute = new SimpleAttributeSet();
		_color = Colors.NORMAL;
		_colors = new Color[ Colors.PALLETE_SIZE ];

		_colors[ Colors.BLACK ] = Color.black;
		_colors[ Colors.RED ] = Color.red; /* red */
		_colors[ Colors.GREEN ] = new Color( 0x00, 0xa0, 0x00 ); /* green */
		_colors[ Colors.BROWN ] = new Color( 0xa0, 0xa0, 0x00 ); /* brown? */
		_colors[ Colors.BLUE ] = Color.blue; /* blue */
		_colors[ Colors.MAGENTA ] = new Color( 0xa0, 0x00, 0xa0 ); /* magenta */
		_colors[ Colors.CYAN ] = new Color( 0x00, 0xa0, 0xa0 );; /* cyan */
		_colors[ Colors.LIGHTGRAY ] = Color.lightGray;
		_colors[ Colors.DARKGRAY ] = Color.darkGray;
		_colors[ Colors.BRIGHTRED ] = new Color( 0xff, 0x80, 0x80 ); /* bright red */
		_colors[ Colors.BRIGHTGREEN ] = new Color( 0x80, 0xff, 0x80 ); /* bright green */
		_colors[ Colors.YELLOW ] = Color.yellow; /* yellow */
		_colors[ Colors.BRIGHTBLUE ] = new Color( 0x80, 0x80, 0xff ); /* bright blue */
		_colors[ Colors.BRIGHTMAGENTA ] = Color.magenta; /* bright magenta */
		_colors[ Colors.BRIGHTCYAN ] = Color.cyan; /* bright cyan */
		_colors[ Colors.WHITE ] = Color.white;
		_colors[ Colors.OTHERGRAY ] = Color.gray;
	}
	void init() {
		try {
			String res = "/res/" + _resource + ".xml";
			System.out.println( "Loading resources: " + res );
			XUL xul = new XUL( this );
			updateTagLib( xul );
			xul.insert( AppletJDOMHelper.loadResource( res, this ), this );
			HAbstractConfigurator ac = getConfigurator();
			if ( ac != null )
				xul.mapMembers( ac );
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
	public abstract void updateTagLib( XUL $se );
	public abstract JTextPane getLogPad();
	public Color color( int $color ) { return ( color( $color, null ) ); }
	public Color color( int $color, java.awt.Component $on ) {
		if ( $on == null )
			$on = _logPad;
		Color bg = $on.getBackground();
		int red = 255 - bg.getRed();
		int green = 255 - bg.getGreen();
		int blue = 255 - bg.getBlue();
		int max = 16 * 16;
		if ( ( red * red + green * green + blue * blue ) < ( max + max + max ) ) {
			if ( ( $color == Colors.LIGHTGRAY ) || ( $color == Colors.WHITE ) )
				$color = Colors.BLACK;
			else if ( $color < Colors.DARKGRAY )
				$color += Colors.DARKGRAY;
			else if ( $color >= Colors.DARKGRAY )
				$color -= Colors.DARKGRAY;
		}
		return ( _colors[ $color ] );
	}
	public int lcolor( int $color ) {
		return ( $color );
	}
	void log( String $message, int $color ) {
		StyleConstants.setForeground( _attribute, color( lcolor( $color ) ) );
		add( _logPad, $message, _attribute );
	}
	void log( String $message ) {
		StyleConstants.setForeground( _attribute, color( lcolor( _color ) ) );
		add( _logPad, $message, _attribute );
	}
	void log( int $color ) {
		if ( $color > 15 )
			$color = 15;
		_color = $color;
	}
	public void clearLog() {
		clear( _logPad );
		_log = ( DefaultStyledDocument )_logPad.getStyledDocument();
	}
	public void clear( JTextPane $what ) {
		$what.setStyledDocument( new DefaultStyledDocument() );
	}
	public HAbstractConfigurator getConfigurator() {
		return ( null );
	}
	void add( JTextPane $to, String $what, SimpleAttributeSet $looks ) {
		DefaultStyledDocument txt = (DefaultStyledDocument)$to.getStyledDocument();
		try {
			txt.insertString( txt.getLength(), $what, $looks );
			$to.setCaretPosition( txt.getLength() );
		} catch ( javax.swing.text.BadLocationException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void add( JTextPane $to, String $what ) {
		add( $to, $what, null );
	}
	protected void discardPendingEvents() {
		java.awt.EventQueue q = java.awt.Toolkit.getDefaultToolkit().getSystemEventQueue();
		while ( q.peekEvent() != null ) {
			try {
				q.getNextEvent();
			} catch (InterruptedException e) {
			}
		} 
	}
}
