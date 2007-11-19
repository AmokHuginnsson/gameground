import org.apache.commons.cli.*;
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

public class /* Application or applet name: */ GameGround extends JApplet {
	public static final long serialVersionUID = 13l;
	private static SortedMap<String,HAbstractLogic> _logics = java.util.Collections.synchronizedSortedMap( new TreeMap<String,HAbstractLogic>() );
	private static GameGround _instance;

	public Frame _frame;
	private HClient _client;
	boolean _applet = false;
	CommandLine _cmd;

	public GameGround() { this( null ); }
	GameGround( String[] $argv ) {
		handleArguments( $argv );
	}

	public void init() {
		try {
			SwingEngine se = new SwingEngine( this );
			org.jdom.Document res = AppletJDOMHelper.loadResource( "/res/gameground.xml", this );
			if ( _instance == null ) {
				_instance = this;
				TagLibrary tl = se.getTaglib();
				tl.unregisterTag( "frame" );
				tl.registerTag( "frame", JApplet.class );
				se.insert( res, this );
				_frame = getParentFrame();
				_applet = true;
			} else {
				se.render( res );
				_frame = SwingEngine.getAppFrame();
				((javax.swing.JFrame)_frame).setContentPane( this );
			}
			EagerStaticInitializer.touch( this );
			setFace( Go.LABEL );
			resize( res.getRootElement().getAttribute( "size" ).getValue().split( ",", 2 ) );
			setFace( Go.LABEL );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	void resize( String[] $req ) {
		Dimension preferred = new Dimension( new Integer( $req[0] ).intValue(), new Integer( $req[1] ).intValue() );
		_frame.setVisible( true );
		Dimension real = getContentPane().getSize();
		Dimension withJunk = _frame.getSize();
		_frame.setSize( preferred.width + withJunk.width - real.width, preferred.height + withJunk.height - real.height );
		_frame.validate();
	}
	public static GameGround getInstance() {
		return ( _instance );
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
		_instance = new GameGround( $argv );
		_instance.init();
		_instance.start();
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

	public static void registerLogic( String $name, HAbstractLogic $logic ) {
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
}

