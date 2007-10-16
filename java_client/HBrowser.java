import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.Container;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.StyleConstants;
import javax.swing.text.Style;
import javax.swing.text.SimpleAttributeSet; 

class HBrowser extends HAbstractLogic implements ActionListener, KeyListener {
	public static final String LABEL = "browser";
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public JTextField _msg;
		public JTextPane _logPad;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void reinit() {
			_msg.requestFocusInWindow();
			log( "### Connectiong to server ...\n" );
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public void updateTagLib( org.swixml.SwingEngine $se ) {	}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	private static final boolean registered;
	static {
		registered = registerLogic();
	}
	HClient _client;
	HGUILocal _gui;
//--------------------------------------------//
	public HBrowser( GameGround $applet ) throws Exception {
		super();
		init( _gui = new HGUILocal( LABEL ) );
		$applet.addGlobalKeyListener( $applet, this );
		$applet.addGlobalKeyListener( _gui, this );
	}
	public void keyTyped( KeyEvent $event ) {
	}
	public void keyPressed( KeyEvent $event ) {
	}
	public void keyReleased( KeyEvent $event ) {
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
	}
	static boolean registerLogic() {
		try {
			GameGround.registerLogic( LABEL, new HBrowser( GameGround.getInstance() ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

