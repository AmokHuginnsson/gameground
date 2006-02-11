import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.Container;
import org.swixml.SwingEngine;
import javax.swing.*;

public class /* Application or applet name: */ Galaxy extends JApplet {
	public static final long serialVersionUID = 13l;
	private static JFrame _frame;

	public void init() {
		try {
			add( new HGUIMain() );
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
		_frame.setSize( 400, 200 );
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
}

