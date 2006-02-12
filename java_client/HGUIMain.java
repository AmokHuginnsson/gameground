import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.Container;
import org.swixml.SwingEngine;
import javax.swing.*;

class HGUIMain extends JPanel implements ActionListener {
	public static final long serialVersionUID = 17l;
	public class HWidgets extends SwingEngine {
		/* This is one dirty hack.
		 * To make it work you have to edit org/swixml/SwingEngine.java
		 * and comment out all setAccessible() invocations.
		 */
		public JTextField _name;
		public JTextField _server;
		public JTextField _port;
		public JButton _connect;
	}
	public HWidgets _widgets;
	public HGUIMain() throws Exception {
		_widgets = new HWidgets();
		_widgets.insert( new InputStreamReader( HGUIMain.class.getResourceAsStream( "galaxy.xml" ) ), this );
		_widgets._connect.addActionListener( this );
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == _widgets._connect ) {
			_widgets._name.setText( "AAA" );
		}
	}
}

