import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.Container;
import org.swixml.SwingEngine;
import javax.swing.*;

class HGUIMain extends JPanel implements ActionListener {
	public static final long serialVersionUID = 17l;
	String _emperor;
	public class HWidgets extends SwingEngine {
		/* This is one dirty hack.
		 * To make it work you have to edit org/swixml/SwingEngine.java
		 * and comment out all setAccessible() invocations.
		 */
		public JTextField _name;
		public JTextField _server;
		public JTextField _port;
		public JButton _connect;
		public JPanel _setup;
		public JPanel _main;
		public JLabel _emperor;
		public JPanel _board;
	}
	public HWidgets _widgets;
	public HGUIMain() throws Exception {
		_widgets = new HWidgets();
		_widgets.insert( new InputStreamReader( HGUIMain.class.getResourceAsStream( "res/galaxy.xml" ) ), this );
		_widgets._connect.addActionListener( this );
	}
	void onConnectClick() {
		String errors = "";
		if ( _widgets._name.getText().compareTo( "" ) == 0 )
			errors += "name not set\n";
		else
			_emperor = new String( _widgets._name.getText() );
		if ( _widgets._server.getText().compareTo( "" ) == 0 )
			errors += "server not set\n";
		if ( ( _widgets._port.getText().compareTo( "" ) == 0 )
				|| ( new Integer( _widgets._port.getText() ).intValue() <= 1024 ) )
			errors += "invalid port number (must be over 1024)";
		if( errors.compareTo( "" ) != 0 ) {
			JOptionPane.showMessageDialog( this,
					"Your setup contains following errors:\n" + errors,
					"Galaxy - error ...", JOptionPane.ERROR_MESSAGE );
		} else {
			_widgets._setup.setVisible( false );
			_widgets._setup = null;
			_widgets._main.setVisible( true );
			_widgets._emperor.setText( _emperor );
		}
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == _widgets._connect ) {
			onConnectClick();
		}
	}
}

