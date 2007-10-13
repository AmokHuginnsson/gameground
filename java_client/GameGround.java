import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Collections;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JApplet;
import javax.swing.JFrame;

class CallStack {
	public static void print() {
		System.out.println("Call Stack:");
		StackTraceElement[] ste = (new Throwable()).getStackTrace();
		for (int i=1;i<ste.length;i++){
			System.out.println("Class: "+ste[i].getClassName());
			System.out.println("Method: "+ste[i].getMethodName());
			System.out.println("Line Number: "+ste[i].getLineNumber()+"\n");
		}
	}
}

public class /* Application or applet name: */ GameGround extends JApplet {
	public static final long serialVersionUID = 13l;
	private static JFrame _frame;
	private static SortedMap<String,HAbstractLogic> _logics;
	private static GameGround _instance;

	public void init() {
		try {
			setFace( "login" );
		} catch ( Exception e ) {
			e.printStackTrace();
		}
	}

	public static GameGround getInstance() {
		return ( _instance );
	}

	public void setFace( String $face ) {
		removeAll();
		HAbstractLogic logic = _logics.get( $face );
		if ( logic != null ) {
			add( logic.getGUI() );
			setFocusable( true );
			requestFocus();
		} else {
			System.out.println( "No such logic." );
			CallStack.print();
			System.exit( 1 );
		}
		return;
	}

	static public void main( String $argv[] ) {
		_frame = new JFrame();
		JApplet applet = new GameGround();
		_frame.add( applet );
		applet.init();
		applet.start();
		_frame.setSize( applet.getPreferredSize().width, applet.getPreferredSize().height );
		class HAppTerminator extends WindowAdapter {
			JApplet _applet;
			public HAppTerminator( JApplet $applet ) {
				_applet = $applet;
			}
		}
		_frame.addWindowListener( new HAppTerminator( applet ) { // Handle window close requests
			public void windowClosing( WindowEvent e ) {
				_applet.stop();
				_applet.destroy();
				System.exit( 0 );
			}
		} );
		_frame.setVisible( true );
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

}

