import org.apache.commons.cli.*;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Collections;
import java.awt.Container;
import javax.swing.JFrame;
import java.awt.Dimension;
import java.awt.Font;
import javax.swing.JApplet;
import org.swixml.SwingEngine;
import org.swixml.TagLibrary;
import java.net.URL;
import java.util.Map;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class GameGround {
	static public class Setup {
		int _fontSize;
		Font[] _font = new Font[2];
		void prepareFonts( Font $font ) {
			if ( _font[0] == null ) {
				_font[0] = $font;
				_font[1] = $font.deriveFont( (float)($font.getSize() + 4 ) );
			}
		}
		void setFontSize( int $fontSize ) {
			_fontSize = $fontSize;
		}
		Font getFont() {
			return ( _font[_fontSize] );
		}
		void setSound( boolean $enabled ) {
			Sound.setEnabled( $enabled );
		}
	};
	static public class Geometry {
		int _width;
		int _height;
		int _xPos;
		int _yPos;
		Geometry( int $width, int $height, int $xPos, int $yPos ) {
			_width = $width;
			_height = $height;
			_xPos = $xPos;
			_yPos = $yPos;
		}
	}
	public static final long serialVersionUID = 13l;
	public JFrame _frame;
	private final ScheduledExecutorService _scheduler = Executors.newScheduledThreadPool( 1 );
	private SortedMap<String, HLogicInfo> _clientSupportedLogics = java.util.Collections.synchronizedSortedMap( new TreeMap<String, HLogicInfo>() );
	private Map<Object, ScheduledFuture<?>> _tasks = Collections.synchronizedMap( new HashMap<Object, ScheduledFuture<?>>() );
	private HClient _client;
	private String _frameName;
	private String _name;
	private HLogin _loginScreen = null;
	private HWorkArea _workArea = null;
	Ini _ini = new Ini();
	Setup _setup = new Setup();
	Geometry _geometry = null;
	CommandLine _cmd;

	public GameGround() { this( null ); }
	GameGround( String[] $argv ) {
		handleProgramOptions( $argv );
	}

	public void init() {
		try {
			SwingEngine se = new SwingEngine( this );
			org.jdom.Document res = AppletJDOMHelper.loadResource( "/res/gameground.xml", this );
			se.render( res );
			_frame = (javax.swing.JFrame)SwingEngine.getAppFrame();
			_frame.addWindowListener( new java.awt.event.WindowAdapter() {
					public void windowClosing( java.awt.event.WindowEvent e ) {
						if ( _client != null ) {
							_workArea.gracefullShutdown();
						}
						System.out.println( "Bye!" );
					}
			});
			_frameName = _frame.getTitle();
			EagerStaticInitializer.touch( this, "registerLogic" );
			_loginScreen = new HLogin( this );
			_workArea = new HWorkArea( this );
			showLoginScreen();
			resize( res.getRootElement().getAttribute( "size" ).getValue().split( ",", 2 ) );
			System.out.println( "Title: " + _frameName );
			URL url = GameGround.class.getResource( "/class.list" );
			long selfLastModification = url.openConnection().getLastModified();
			URL fresh = new URL( "http://codestation.org/jar/gameground.jar" );
			/* 
			 * Give one minute to upload new GameGround client version to the server.
			 * Additionally give 4 minutes to compensate eventual CodeStation clock skew.
			 */
			long freshLastModification = fresh.openConnection().getLastModified() - 5 * 60 * 1000;

			java.text.SimpleDateFormat timeFormat = new java.text.SimpleDateFormat( "yyyy-MM-dd HH:mm:ss" );
			System.out.println( "freshLastModification: " + timeFormat.format( freshLastModification ) );
			System.out.println( "selfLastModification: " + timeFormat.format( selfLastModification ) );
			if ( freshLastModification > selfLastModification ) {
				javax.swing.JOptionPane.showMessageDialog( _frame,
						"The client you are running is outdated by " + DurationFormat.toString( freshLastModification - selfLastModification ) + ".\n"
						+ "(Latest version is from: " + timeFormat.format( freshLastModification ) + ")\n"
						+ "Please do the following:\n"
						+ "1. Close this client.\n"
						+ "2. Clear all Java related caches.\n"
						+ "3. Try to open this client again.",
						"GameGround - warning ...", javax.swing.JOptionPane.WARNING_MESSAGE );
			}
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	JFrame frame() {
		return ( _frame );
	}
	void showLoginScreen() {
		if ( _client != null ) {
			Chat.clearAll();
			_client.disconnect();
			_client = null;
		}
		showWorkArea( _loginScreen );
	}
	void showWorkArea() {
		showWorkArea( _workArea );
	}
	void showWorkArea( HAbstractWorkArea $workArea ) {
		_frame.setContentPane( $workArea.getGUI() );
		$workArea.reinit();
		_frame.validate();
	}
	public void processMessage( final String $message ) {
		javax.swing.SwingUtilities.invokeLater( new Runnable() {
			public void run() {
				_workArea.processMessage( $message );
			}
		});
	}
	public HLogin.OConnectionConfig getConnectionConfig() {
		return ( _loginScreen.getConnectionConfig() );
	}
	public void destroy() {
		if ( _client != null )
			_client.disconnect();
	}
	void resize( String[] $req ) {
		Dimension preferred = new Dimension( Integer.parseInt( $req[0] ), Integer.parseInt( $req[1] ) );
		_frame.setVisible( true );
		Dimension real = _frame.getContentPane().getSize();
		Dimension withJunk = _frame.getSize();
		if ( _geometry != null ) {
			_frame.setBounds( _geometry._xPos, _geometry._yPos, _geometry._width, _geometry._height );
		} else {
			_frame.setSize( preferred.width + withJunk.width - real.width, preferred.height + withJunk.height - real.height );
		}
		_frame.validate();
	}

	static public void main( final String $argv[] ) {
		javax.swing.SwingUtilities.invokeLater( new Runnable() {
			public void run() {
				eventDispatchThreadMain( $argv );
			}
		});
	}

	static void eventDispatchThreadMain( String $argv[] ) {
		SwingEngine.DEBUG_MODE = true;
		GameGround app = new GameGround( $argv );
		app.init();
		app._frame.setVisible( true );
	}

	public void addGlobalKeyListener( java.awt.Component $component, java.awt.event.KeyListener $who ) {
		$component.addKeyListener( $who );
		if ( $component instanceof java.awt.Container ) {
			java.awt.Component[] components = ((java.awt.Container)$component).getComponents();
			for ( int i = 0; i < components.length ; ++ i ) {
				addGlobalKeyListener( components[i], $who );
			}
		}
	}

	public void registerLogic( String $name, HLogicInfo $info ) {
		System.out.println( "Registering client supported logic: `" + $name + "'." );
		_clientSupportedLogics.put( $name, $info );
		return;
	}

	public HLogicInfo getSupportedLogic( String $name ) {
		return ( _clientSupportedLogics.get( $name ) );
	}

	public void closeParty( HAbstractLogic $logic ) {
		_workArea.closeParty( $logic );
	}
	public HClient getClient() {
		return ( _client );
	}
	public void setClient( HClient $client ) {
		_client = $client;
		if ( _client != null )
			_frame.setTitle( _frameName + " [" + _name + "]" );
		else
			_frame.setTitle( _frameName );
	}

	public void shutdown() {
		System.exit( 0 );
	}
	public String getParameter( String $name ) {
		String val;
		val = _cmd.getOptionValue( $name );
		if ( val == null ) {
			val = _ini.getProperty( $name );
		}
		System.out.println( "Getting parameter: " + $name + ", of value: " + val + "." );
		return ( val );
	}
	void handleProgramOptions( String[] $argv ) {
		_ini.load();
		if ( $argv != null ) {
			Options opts = new Options();
			OptionBuilder.withLongOpt( "help" );
			OptionBuilder.withDescription( "provide this help message and exit" );
			opts.addOption( OptionBuilder.create( 'h' ) );
			OptionBuilder.withLongOpt( "auto-connect" );
			OptionBuilder.withDescription( "automatically connect to the server" );
			opts.addOption( OptionBuilder.create( 'A' ) );
			OptionBuilder.withLongOpt( "login" );
			OptionBuilder.withArgName( "name" );
			OptionBuilder.hasArg();
			OptionBuilder.withDescription( "your preferred nick name" );
			opts.addOption( OptionBuilder.create( 'L' ) );
			OptionBuilder.withLongOpt( "password" );
			OptionBuilder.withArgName( "pass" );
			OptionBuilder.hasArg();
			OptionBuilder.withDescription( "your password" );
			opts.addOption( OptionBuilder.create( 'p' ) );
			OptionBuilder.withLongOpt( "port" );
			OptionBuilder.withArgName( "number" );
			OptionBuilder.hasArg();
			OptionBuilder.withDescription( "port number where GameGround is running" );
			opts.addOption( OptionBuilder.create( 'P' ) );
			OptionBuilder.withLongOpt( "host" );
			OptionBuilder.withArgName( "address" );
			OptionBuilder.hasArg();
			OptionBuilder.withDescription( "Host address to connect to." );
			opts.addOption( OptionBuilder.create( 'H' ) );
			OptionBuilder.withLongOpt( "geometry" );
			OptionBuilder.withArgName( "geom" );
			OptionBuilder.hasArg();
			OptionBuilder.withDescription( "This option specifies the initial size and location of the window." );
			opts.addOption( OptionBuilder.create( 'g' ) );
			Parser p = new PosixParser();
			try {
				System.out.println( "Command line:" );
				for ( String s : $argv )
					System.out.println( s );
				_cmd = p.parse( opts, $argv );
			} catch ( ParseException e ) {
				Con.err( "Application failed to start. Reaseon: " + e.getMessage() );
				System.exit( 1 );
			}
			if ( _cmd.hasOption( "help" ) ) {
				HelpFormatter formatter = new HelpFormatter();
				formatter.printHelp( "GameGround", opts );
				System.exit( 0 );
			}
			if ( _cmd.hasOption( "auto-connect" ) )
				_ini.setProperty( "auto-connect", "true" );
			if ( _cmd.hasOption( "geometry" ) ) {
				Pattern pattern = Pattern.compile( "(\\d+)x(\\d+)\\+(\\d+)\\+(\\d+)" );
				String g = _cmd.getOptionValue( "geometry" );
				Matcher m = pattern.matcher( g );
				if ( ! m.matches() || ( m.groupCount() != 4 ) ) {
					Con.err( "Application failed to start. Reaseon: Invalid geometry specificaton: " + g );
					System.exit( 1 );
				}
				_geometry = new Geometry( Integer.parseInt( m.group( 1 ) ), Integer.parseInt( m.group( 2 ) ), Integer.parseInt( m.group( 3 ) ), Integer.parseInt( m.group( 4 ) ) );
			}
		}
	}
	public Ini ini() {
		return ( _ini );
	}
	public Setup setup() {
		return ( _setup );
	}
	public String getName() {
		return ( _name );
	}
	public void setName( String $name ) {
		_name = $name;
	}
	public void registerTask( Runnable $task, long $delay ) {
		_tasks.put( $task, _scheduler.scheduleAtFixedRate( $task, 0, $delay, TimeUnit.SECONDS ) );
	}
	void flush( Runnable $task ) {
		ScheduledFuture<?> task = _tasks.get( $task );
		_tasks.remove( $task );
		task.cancel( false );
	}
}

