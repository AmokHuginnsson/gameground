import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.Container;
import org.swixml.SwingEngine;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JEditorPane;

class HGUIMain extends JPanel implements ActionListener {
	public static final long serialVersionUID = 17l;
	HClient _client;
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
		public HBoard _board;
		public JEditorPane _log;
		public HWidgets() {
			super.getTaglib().registerTag( "hboard", HBoard.class );
		}
	}
	public HWidgets _widgets;
	public HGUIMain() throws Exception {
		_widgets = new HWidgets();
		HImages images = new HImages();
		_widgets.insert( new InputStreamReader( getClass().getResourceAsStream( "res/galaxy.xml" ) ), this );
		_widgets._board.setImages( images );
		_widgets._connect.addActionListener( this );
	}
	void onConnectClick() {
		String errors = "";
		String emperor = "";
		if ( _widgets._name.getText().compareTo( "" ) == 0 )
			errors += "name not set\n";
		else
			emperor = new String( _widgets._name.getText() );
		String server = "";
		if ( _widgets._server.getText().compareTo( "" ) == 0 )
			errors += "server not set\n";
		else
			server = new String( _widgets._server.getText() );
		int port = -1;
		try {
			port = new Integer( _widgets._port.getText() ).intValue();
		} catch ( NumberFormatException e ) {
		}
		if ( port <= 1024 )
			errors += "invalid port number (must be over 1024)";
		if( errors.compareTo( "" ) != 0 ) {
			JOptionPane.showMessageDialog( this,
					"Your setup contains following errors:\n" + errors,
					"Galaxy - error ...", JOptionPane.ERROR_MESSAGE );
		} else {
/*			try {
				_client = new HClient( emperor, server, port );
				_client.start();
			} catch ( Exception e ) {
				JOptionPane.showMessageDialog( this,
						"Galaxy client was unable to connect to server:\n" + e.getMessage(),
						"Galaxy - error ...", JOptionPane.ERROR_MESSAGE );
				return;
			}*/
			_widgets._setup.setVisible( false );
			_widgets._setup = null;
			_widgets._main.setVisible( true );
			_widgets._emperor.setText( emperor );
			_log.
		}
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == _widgets._connect ) {
			onConnectClick();
		}
	}
}

