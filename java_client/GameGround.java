import org.apache.commons.cli.*;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Collections;
import java.awt.Container;
import java.awt.Frame;
import java.awt.Dimension;
import javax.swing.JApplet;
import org.swixml.SwingEngine;
import org.swixml.TagLibrary;
import java.util.Map;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.Properties;
import java.io.FileReader;

public class /* Application or applet name: */ GameGround extends JApplet {
	public static final long serialVersionUID = 13l;
	public Frame _frame;
	private final ScheduledExecutorService _scheduler = Executors.newScheduledThreadPool( 1 );
	private SortedMap<String, HLogicInfo> _clientSupportedLogics = java.util.Collections.synchronizedSortedMap( new TreeMap<String, HLogicInfo>() );
	private Map<Object, ScheduledFuture<?>> _tasks = Collections.synchronizedMap( new HashMap<Object, ScheduledFuture<?>>() );
	private HClient _client;
	private String _frameName;
	private String _name;
	private HLogin _loginScreen = null;
	private HWorkArea _workArea = null;
	boolean _applet = false;
	Properties _ini;
	CommandLine _cmd;

	public GameGround() { this( null ); }
	GameGround( String[] $argv ) {
		if ( $argv == null )
			_applet = true;
		handleProgramOptions( $argv );
	}

	public void init() {
		try {
			SwingEngine se = new SwingEngine( this );
			org.jdom.Document res = AppletJDOMHelper.loadResource( "/res/gameground.xml", this );
			if ( _applet == true ) {
				TagLibrary tl = se.getTaglib();
				tl.unregisterTag( "frame" );
				tl.registerTag( "frame", JApplet.class );
				se.insert( res, this );
				_frame = getParentFrame();
			} else {
				se.render( res );
				_frame = SwingEngine.getAppFrame();
				((javax.swing.JFrame)_frame).setContentPane( this );
				_frame.addWindowListener( new java.awt.event.WindowAdapter() {
						public void windowClosing( java.awt.event.WindowEvent e ) {
							if ( _client != null ) {
								_workArea.gracefullShutdown();
							}
							System.out.println( "Bye!" );
						}
				});
			}
			_frameName = _frame.getTitle();
			EagerStaticInitializer.touch( this, "registerLogic" );
			_loginScreen = new HLogin( this );
			_workArea = new HWorkArea( this );
			showLoginScreen();
			resize( res.getRootElement().getAttribute( "size" ).getValue().split( ",", 2 ) );
			System.out.println( "Title: " + _frameName );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void showLoginScreen() {
		if ( _client != null ) {
			_client.disconnect();
			_client = null;
		}
		showWorkArea( _loginScreen );
	}
	void showWorkArea() {
		showWorkArea( _workArea );
	}
	void showWorkArea( HAbstractWorkArea $workArea ) {
		setContentPane( $workArea.getGUI() );
		$workArea.reinit();
		validate();
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
		Dimension preferred = new Dimension( new Integer( $req[0] ).intValue(), new Integer( $req[1] ).intValue() );
		_frame.setVisible( true );
		Dimension real = getContentPane().getSize();
		Dimension withJunk = _frame.getSize();
		_frame.setSize( preferred.width + withJunk.width - real.width, preferred.height + withJunk.height - real.height );
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
		app.start();
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

	private Frame getParentFrame() {
		Container cont = this;
		while ( ( cont != null ) && !( cont instanceof Frame ) ) {
			cont = cont.getParent();
		}
		if ( cont != null )
			return (Frame)cont;
		return null;
	} 

	public void closeParty( String $id ) {
		_workArea.closeParty( $id );
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
		if ( _applet ) {
			_frame.setVisible( false );
		} else {
			System.exit( 0 );
		}
	}
	public boolean isApplet() {
		return ( _applet );
	}
	public String getParameter( String $name ) {
		String val;
		if ( isApplet() )
			val = super.getParameter( $name );
		else 
			val = _cmd.getOptionValue( $name );
		if ( ( _ini != null ) && ( val == null ) )
			val = _ini.getProperty( $name );
		System.out.println( "Getting parameter: " + $name + ", of value: " + val + "." );
		return ( val );
	}
	void handleProgramOptions( String[] $argv ) {
		try {
			/* System.getProperties().list( System.out ); */
			_ini = new Properties();
			_ini.load( new FileReader( System.getenv( "HOME" ) + "/etc/conf/gameground-clientrc" ) );
		} catch ( java.io.FileNotFoundException e ) {
			Con.err( e.getMessage() );
		} catch ( java.io.IOException e ) {
			Con.err( "FATAL ERROR: " + e.getMessage() );
			e.printStackTrace();
			System.exit( 1 );
		} catch ( java.security.AccessControlException e ) {
			Con.err( "Insufficient privileges to guess home directory: " + e.getMessage() );
		}
		if ( $argv != null ) {
			Options opts = new Options();
			opts.addOption( OptionBuilder.withLongOpt( "help" ).withDescription( "provide this help message and exit" ).create( 'h' ) );
			opts.addOption( OptionBuilder.withLongOpt( "auto-connect" ).withDescription( "automatically connect to the server" ).create( 'A' ) );
			opts.addOption( OptionBuilder.withLongOpt( "login" ).withArgName( "name" ).hasArg().withDescription( "your preferred nick name" ).create( 'L' ) );
			opts.addOption( OptionBuilder.withLongOpt( "port" ).withArgName( "number" ).hasArg().withDescription( "port number where GameGround is running" ).create( 'P' ) );
			opts.addOption( OptionBuilder.withLongOpt( "host" ).withArgName( "address" ).hasArg().withDescription( "Host address to connect to." ).create( 'H' ) );
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
			if ( _cmd.hasOption( "auto-connect" ) && ( _ini != null ) )
				_ini.setProperty( "auto-connect", "true" );
		}
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

