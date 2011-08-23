import javax.swing.JTextField;
import javax.swing.JTextPane;

class Chat extends HAbstractLogic implements Runnable {
	public static final class PROTOCOL {
		public static final String SEP = ":";
		public static final String SEPP = ",";
		public static final String CMD = "cmd";
		public static final String SAY = "say";
		public static final String PLAYER = "player";
		public static final String PLAYERQUIT = "player_quit";
	}
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public static final String SIT = "Sit !";
		public static final String GETUP = "Get up !";
		public JTextField _msg;
		public JTextPane _logPad;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void updateTagLib( XUL $xul ) {
		}
		public void init() {
			super.init();
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public HAbstractConfigurator getConfigurator() {
			return ( null );
		}
		public void onMessage() {
			String msg = _msg.getText();
			if ( msg.matches( ".*\\S+.*" ) ) {	
				_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP + PROTOCOL.SAY + PROTOCOL.SEP + msg );
				_msg.setText( "" );
			}
		}
		public void onClose() {
			_app.closeParty( _id );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "chat";
	public HGUILocal _gui;
//--------------------------------------------//
	public Chat( GameGround $applet, String $id, String $configuration ) throws Exception {
		super( $applet, $id, $configuration );
		init( _gui = new HGUILocal( LABEL ) );
	}
	public void cleanup() {
		_app.flush( this );
	}
	public void run() {
	}
}

