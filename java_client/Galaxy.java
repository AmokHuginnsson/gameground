import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import org.swixml.SwingEngine;
import javax.swing.*;

class HGUIMain extends JPanel implements ActionListener {
	public static final long serialVersionUID = 17l;
	public JTextField _edit0;
	public JTextField _edit1;
	public JButton _go;
	public JRadioButton _radio0;
	public JRadioButton _radio1;
	public JRadioButton _radio2;
	JRadioButton _active;
	public HGUIMain( String $label ) throws Exception {
//		new SwingEngine( this ).insert( HGUIMain.class.getResource( "galaxy.xml" ), this );
		new SwingEngine( this ).insert( new InputStreamReader( HGUIMain.class.getResourceAsStream( "galaxy.xml" ) ), this );
		_go.addActionListener( this );
		_radio0.addActionListener( this );
		_radio1.addActionListener( this );
		_radio2.addActionListener( this );
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( ( _active != null ) && ( source == _go ) ) {
			_edit1.setText( _active.getText() );
		} else if ( source.getClass().getName() == "javax.swing.JRadioButton" ) {
			_active = ( JRadioButton ) source;
			_edit0.setText( _active.getText() );
		}
	}
}

public class /* Application or applet name: */ Galaxy extends JApplet {
	public static final long serialVersionUID = 13l;
	private static JFrame _frame;

	public void init() {
		try {
			add( new HGUIMain( "" ) );
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

