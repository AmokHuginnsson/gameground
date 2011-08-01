import java.awt.event.ActionEvent;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JTextField;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import org.swixml.XTabbedPane;

class WorkArea extends HAbstractLogic {
	public static final String LABEL = "workarea";
	static boolean once = true;
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public XTabbedPane _tabs = null;
		private HBrowser _broser;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void reinit() {
			_broser = new HBrowser( _app );
			_broser.reinit();
			_broser.getGUI().reinit();
				if ( _tabs == null ) {
					System.out.println( ":(" );
				}
			_tabs.addTab( "Browser", _broser.getGUI() );
		}
		public void onExit() {
			_app.shutdown();
		}
		public JTextPane getLogPad() { return ( null ); }
		public void updateTagLib( XUL $xul ) {	}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	HGUILocal _gui;
//--------------------------------------------//
	public WorkArea( GameGround $applet ) throws Exception {
		super( $applet );
		init( _gui = new HGUILocal( LABEL ) );
	}
	public void reinit() {
	}
	public void cleanup() {}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( LABEL, new WorkArea( $app ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

