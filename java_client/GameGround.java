import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Collections;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.Container;
import java.awt.Frame;
import javax.swing.JApplet;
import org.swixml.SwingEngine;
import org.swixml.TagLibrary;

public class /* Application or applet name: */ GameGround extends JApplet {
	public static final long serialVersionUID = 13l;
	private static SortedMap<String,HAbstractLogic> _logics = java.util.Collections.synchronizedSortedMap( new TreeMap<String,HAbstractLogic>() );
	private static GameGround _instance;
	public Frame _frame;
	private HClient _client;

	public void init() {
		try {
			SwingEngine se = new SwingEngine( this );
			if ( _instance == null ) {
				_instance = this;
				TagLibrary tl = se.getTaglib();
				tl.unregisterTag( "frame" );
				tl.registerTag( "frame", JApplet.class );
				se.insert( AppletJDOMHelper.loadResource( "/res/gameground.xml", this ), this );
				_frame = getParentFrame();
			} else {
				se.render( AppletJDOMHelper.loadResource( "/res/gameground.xml", this ) );
				_frame = SwingEngine.getAppFrame();
				((javax.swing.JFrame)_frame).setContentPane( this );
			}
			_frame.setVisible( true );
			EagerStaticInitializer.touch( this );
			setFace( HLogin.LABEL );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
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
			logic.reinit();
			if ( _client != null )
				_client.setLogic( logic );
		} else {
			java.util.Set<java.util.Map.Entry<String,HAbstractLogic>> entSet = _logics.entrySet();
			java.util.Map.Entry<String,HAbstractLogic> ent = null;
			for ( java.util.Iterator<java.util.Map.Entry<String,HAbstractLogic>> it = entSet.iterator(); it.hasNext(); ent = it.next() )
				System.out.println( "logic: " + ent.getKey() );
			System.out.println( "No such logic: " + $face + "." );
			CallStack.print();
			System.exit( 1 );
		}
		return;
	}

	static public void main( String $argv[] ) {
		SwingEngine.DEBUG_MODE = true;
		_instance = new GameGround();
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

}

