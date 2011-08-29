import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Vector;
import javax.swing.JTextField;
import javax.swing.JTextPane;

class Chat extends HAbstractLogic {
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
				if ( _online ) {
					_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP + PROTOCOL.SAY + PROTOCOL.SEP + msg );
				} else {
					if ( _lineBuffer.isEmpty() ) {
						_aboutToCreate = true;
						_client.println( "create:chat:" + _interlocutor + "," + _app.getName() );
					}
					_lineBuffer.add( msg );
				}
				_msg.setText( "" );
			}
		}
		public void onClose() {
			_app.closeParty( Chat.this );
			_online = false;
			_id = "0";
			_chats.put( _interlocutor, Chat.this );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	public static final String LABEL = "chat";
	private static final SortedMap<String, Chat> _chats = java.util.Collections.synchronizedSortedMap( new TreeMap<String, Chat>() );
	Vector<String> _lineBuffer = new Vector<String>();
	boolean _online = false;
	boolean _aboutToCreate = false;
	String _interlocutor;
	public HGUILocal _gui;
//--------------------------------------------//
	public Chat( GameGround $applet, String $configuration ) throws Exception {
		super( $applet, "0", $configuration );
		init( _gui = new HGUILocal( LABEL ) );
		_handlers.put( PROTOCOL.PLAYERQUIT, HAbstractLogic.class.getDeclaredMethod( "handlerDummy", new Class[]{ String.class } ) );
		String[] info = $configuration.split( ",", 4 );
		if ( info.length == 4 ) {
			handleMessage( "User: " + ( _interlocutor = info[0] ) );
			handleMessage( "Full name: " + Sec.unescape( info[1] ) );
			handleMessage( "Description:\n" + Sec.unescape( info[2] ) );
		} else
			handleMessage( "User `" + ( _interlocutor = info[0] ) + "' is not registered yet." );
	}
	synchronized public void cleanup() {
	}
	synchronized void setId( String $id ) {
		_id = $id;
		_online = true;
		if ( ! _aboutToCreate )
			Sound.play( "phone-incoming-call" );
		_aboutToCreate = false;
		for ( String line : _lineBuffer ) {
			_client.println( PROTOCOL.CMD + PROTOCOL.SEP + _id + PROTOCOL.SEP + PROTOCOL.SAY + PROTOCOL.SEP + line );
		}
		_lineBuffer.clear();
	}
	synchronized public static Chat showUserInfo( GameGround $app, String $info ) {
		System.out.println( "chat info: " + $info );
		String[] tokens = $info.split( ",", 2 );
		Chat chat = null;
		chat = _chats.get( tokens[0] );
		if ( chat == null ) {
			try {
				chat = new Chat( $app, $info );
			} catch ( Exception e ) {
				e.printStackTrace();
				System.exit( 1 );
			}
			_chats.put( tokens[0], chat );
		}
		return ( chat );
	}
	synchronized static HAbstractLogic create( GameGround $app, String $id, String $configuration ) {
		Chat chat = null;
		String names[] = $configuration.split( ":", 2 );
		String name = names[0].equals( $app.getName() ) ? names[1] : names[0];
		chat = _chats.get( name );
		if ( chat == null )
			chat = showUserInfo( $app, name );
		_chats.remove( name );
		chat.setId( $id );
		return ( chat );
	}
	synchronized static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( "chat", new HLogicInfo( "chat", "chat", "Chat", null, Chat.class.getDeclaredMethod( "create", new Class[] { GameGround.class, String.class, String.class } ), true ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

