import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Vector;
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
				if ( online() ) {
					_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP + PROTOCOL.SAY + PROTOCOL.SEP + msg );
				} else {
					if ( _lineBuffer.isEmpty() ) {
						_client.println( "create:chat:" + _interlocutor + "," + _app.getName() );
					}
					_lineBuffer.add( msg );
				}
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
	private static final SortedMap<String, Chat> _userInfos = java.util.Collections.synchronizedSortedMap( new TreeMap<String, Chat>() );
	Vector<String> _lineBuffer = new Vector<String>();
	String _interlocutor;
	public HGUILocal _gui;
//--------------------------------------------//
	public Chat( GameGround $applet, String $configuration ) throws Exception {
		super( $applet, "0", $configuration );
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		String[] info = $configuration.split( ",", 4 );
		handleMessage( "User: " + ( _interlocutor = info[0] ) );
		handleMessage( "Full name: " + Sec.unescape( info[1] ) );
		handleMessage( "Description:\n" + Sec.unescape( info[2] ) );
	}
	boolean online() {
		return ( ! "0".equals( _id ) );
	}
	public void cleanup() {
		_app.flush( this );
	}
	public void run() {
	}
	void setId( String $id ) {
		_id = $id;
		for ( String line : _lineBuffer ) {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP + PROTOCOL.SAY + PROTOCOL.SEP + line );
		}
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
		Chat chat = null;
		String names[] = $configuration.split( ":", 2 );
		String name = names[0].equals( $app.getName() ) ? names[1] : names[0];
		chat = _userInfos.get( name );
		assert chat != null : "chat should already exist";
		_userInfos.remove( name );
		chat.setId( $id );
		return ( chat );
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

