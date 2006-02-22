import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JApplet;
import javax.swing.JFrame;

public class /* Application or applet name: */ Galaxy extends JApplet {
	public static final long serialVersionUID = 13l;
	private static JFrame _frame;

	public void init() {
		try {
			add( new HGUIMain( this ) );
			setFocusable( true );
			requestFocus();
		} catch ( Exception e ) {
			e.printStackTrace();
		}
	}

	static public void main( String $argv[] ) {
		_frame = new JFrame();
		JApplet applet = new Galaxy();
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

}

