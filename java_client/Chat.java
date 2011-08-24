import java.util.SortedMap;
import java.util.TreeMap;
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
	private static final SortedMap<String, HAbstractLogic> _userInfos = java.util.Collections.synchronizedSortedMap( new TreeMap<String, HAbstractLogic>() );
	public HGUILocal _gui;
//--------------------------------------------//
	public Chat( GameGround $applet, String $configuration ) throws Exception {
		super( $applet, "0", $configuration );
		init( _gui = new HGUILocal( LABEL ) );
		String[] info = $configuration.split( ",", 4 );
		handleMessage( "User: " + info[0] );
		handleMessage( "Full name: " + Sec.unescape( info[1] ) );
		handleMessage( "Description:\n" + Sec.unescape( info[2] ) );
	}
	public void cleanup() {
		_app.flush( this );
	}
	public void run() {
	}
	void setId( String $id ) {
		_id = $id;
	}
	public static Chat showUserInfo( GameGround $app, String $info ) {
		String[] tokens = $info.split( ",", 2 );
		Chat chat = null;
		try {
			chat = new Chat( $app, $info );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		_userInfos.put( tokens[0], chat );
		return ( chat );
	}
	static HAbstractLogic create( GameGround $app, String $id, String $configuration ) {
		HAbstractLogic logic = null;
		String names[] = $configuration.split( ":", 2 );
		String name = names[0].equals( $app.getName() ) ? names[1] : names[0];
		logic = _userInfos.get( name );
		_userInfos.remove( name );
		return ( logic );
	}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( "chat", new HLogicInfo( "chat", "chat", "Chat", null, Chat.class.getDeclaredMethod( "create", new Class[] { GameGround.class, String.class, String.class } ) ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

