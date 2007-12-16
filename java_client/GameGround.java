import org.apache.commons.cli.*;
import javax.swing.JOptionPane;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Collections;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.Container;
import java.awt.Frame;
import java.awt.Dimension;
import java.awt.Insets;
import javax.swing.JApplet;
import org.swixml.SwingEngine;
import org.swixml.TagLibrary;
import java.util.Map;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class /* Application or applet name: */ GameGround extends JApplet {
	public static final long serialVersionUID = 13l;
	private SortedMap<String,HAbstractLogic> _logics = java.util.Collections.synchronizedSortedMap( new TreeMap<String,HAbstractLogic>() );
	public Frame _frame;
	private final ScheduledExecutorService _scheduler = Executors.newScheduledThreadPool( 1 );
	private Map<Object, ScheduledFuture<?>> _tasks = Collections.synchronizedMap( new HashMap<Object, ScheduledFuture<?>>() );
	private HClient _client;
	private String _frameName;
	private String _name;
	boolean _applet = false;
	CommandLine _cmd;

	public GameGround() { this( null ); }
	GameGround( String[] $argv ) {
		if ( $argv == null )
			_applet = true;
		handleArguments( $argv );
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
			}
			EagerStaticInitializer.touch( this, "registerLogic" );
			setFace( HLogin.LABEL );
			resize( res.getRootElement().getAttribute( "size" ).getValue().split( ",", 2 ) );
			setFace( HLogin.LABEL );
			_frameName = _frame.getTitle();
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
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

	public void setFace( String $face ) {
		HAbstractLogic logic = _logics.get( $face );
		if ( logic != null ) {
			HGUIface f = logic.getGUI();
			setContentPane( f );
			validate();
			f.reinit();
			if ( _client != null )
				_client.setLogic( logic );
			logic.reinit();
		} else {
			java.util.Set<java.util.Map.Entry<String,HAbstractLogic>> entSet = _logics.entrySet();
			java.util.Map.Entry<String,HAbstractLogic> ent = null;
			java.util.Iterator<java.util.Map.Entry<String,HAbstractLogic>> it = entSet.iterator();
			while ( it.hasNext() ) {
				ent = it.next();
				System.out.println( "logic: " + ent.getKey() );
			}
			System.out.println( "No such logic: " + $face + "." );
			CallStack.print();
			System.exit( 1 );
		}
		return;
	}

	public HAbstractLogic getLogicBySymbol( String $symbol ) {
		java.util.Set<java.util.Map.Entry<String,HAbstractLogic>> entSet = _logics.entrySet();
		java.util.Map.Entry<String,HAbstractLogic> ent = null;
		java.util.Iterator<java.util.Map.Entry<String,HAbstractLogic>> it = entSet.iterator();
		while ( it.hasNext() ) {
			ent = it.next();
			if ( ent != null ) {
				HAbstractLogic al = ent.getValue();
				HLogicInfo info = al.getInfo();
				if ( ( info != null ) && ( info._symbol.compareTo( $symbol ) == 0 ) )
					return ( al );
			}
		}
		return ( null );
	}

	static public void main( String $argv[] ) {
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

	public void registerLogic( String $name, HAbstractLogic $logic ) {
		_logics.put( $name, $logic );
		return;
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

	public HAbstractLogic getLogic( String $name ) {
		return ( _logics.get( $name ) );
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
		if ( isApplet() )
			return ( super.getParameter( $name ) );
		else {
			return ( _cmd.getOptionValue( $name ) );
		}
	}
	void handleArguments( String[] $argv ) {
		if ( $argv != null ) {
			Options opts = new Options();
			opts.addOption( OptionBuilder.withLongOpt( "help" ).withDescription( "provide this help message and exit" ).create( 'h' ) );
			opts.addOption( OptionBuilder.withLongOpt( "login" ).withArgName( "name" ).hasArg().withDescription( "your preferred nick name" ).create( 'L' ) );
			opts.addOption( OptionBuilder.withLongOpt( "port" ).withArgName( "number" ).hasArg().withDescription( "port number where GameGround is running" ).create( 'P' ) );
			opts.addOption( OptionBuilder.withLongOpt( "host" ).withArgName( "address" ).hasArg().withDescription( "Host address to connect to." ).create( 'H' ) );
			Parser p = new PosixParser();
			try {
				_cmd = p.parse( opts, $argv );
			} catch ( ParseException e ) {
				System.out.println( "Application failed to start. Reaseon: " + e.getMessage() );
				System.exit( 1 );
			}
			if ( _cmd.hasOption( "help" ) ) {
				HelpFormatter formatter = new HelpFormatter();
				formatter.printHelp( "GameGround", opts );
				System.exit( 0 );
			}
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

