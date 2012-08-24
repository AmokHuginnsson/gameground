import java.lang.reflect.Method;
import java.util.SortedMap;
import java.util.TreeMap;
import java.awt.event.ActionEvent;
import java.awt.Color;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JTextField;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;

class LogicParty {
	public HLogicInfo _logicInfo;
	public Party _party;
	LogicParty( HLogicInfo $logicInfo, Party $party ) {
		_logicInfo = $logicInfo;
		_party = $party;
	}
}

class HWorkArea extends HAbstractWorkArea {
	public static final String LABEL = "workarea";
	static boolean once = true;
	public class HGUILocal extends HGUIface {
		public static final long serialVersionUID = 17l;
		public TabbedPaneX _tabs = null;
		int _previouslySelectedTab = 0;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		void init() {
			super.init();
			_browser = new HBrowser( _app, HWorkArea.this, new PartysModel( _logics ), _logics, _players );
			_tabs.addChangeListener( new ChangeListener() {
				public void stateChanged( ChangeEvent evt ) {
					int selectedTab = _tabs.getSelectedIndex();
					if ( selectedTab != _previouslySelectedTab ) {
						_tabs.setBackgroundAt( _previouslySelectedTab = selectedTab, null );
					}
				}
			});
			_tabs.addTab( "Browser", _browser.getGUI() );
		}
		public void onClose() {
			_app.shutdown();
		}
		public JTextPane getLogPad() { return ( null ); }
		public void updateTagLib( XUL $xul ) {
			$xul.getTaglib().registerTag( "tabbedpanex", TabbedPaneX.class );
		}
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	static Color COLOR_NOTIFY = Color.CYAN.brighter().brighter().brighter().brighter().brighter();
	private static final String CLIENT_VERSION = "2";
	private SortedMap<String, Method> _handlers = java.util.Collections.synchronizedSortedMap( new TreeMap<String, Method>() );
	private SortedMap<String, HLogicInfo> _logics = java.util.Collections.synchronizedSortedMap( new TreeMap<String, HLogicInfo>() );
	private SortedMap<String, Player> _players = java.util.Collections.synchronizedSortedMap( new TreeMap<String, Player>() );
	HClient _client;
	HGUILocal _gui;
	private HBrowser _browser;
//--------------------------------------------//
	public HWorkArea( GameGround $applet ) throws Exception {
		super( $applet );
		init( _gui = new HGUILocal( LABEL ) );
		try {
			_handlers.put( "err", HWorkArea.class.getDeclaredMethod( "handleError", new Class[]{ String.class } ) );
			_handlers.put( "warn", HWorkArea.class.getDeclaredMethod( "handleWarning", new Class[]{ String.class } ) );
			_handlers.put( "party", HWorkArea.class.getDeclaredMethod( "handleParty", new Class[]{ String.class } ) );
			_handlers.put( "logic", HWorkArea.class.getDeclaredMethod( "handleLogic", new Class[]{ String.class } ) );
			_handlers.put( "account", HWorkArea.class.getDeclaredMethod( "handleAccount", new Class[]{ String.class } ) );
			_handlers.put( "party_info", HWorkArea.class.getDeclaredMethod( "handlePartyInfo", new Class[]{ String.class } ) );
			_handlers.put( "party_close", HWorkArea.class.getDeclaredMethod( "handlePartyClose", new Class[]{ String.class } ) );
			_handlers.put( "player", HWorkArea.class.getDeclaredMethod( "handlePlayer", new Class[]{ String.class } ) );
			_handlers.put( "player_quit", HWorkArea.class.getDeclaredMethod( "handlePlayerQuit", new Class[]{ String.class } ) );
		} catch ( java.lang.NoSuchMethodException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void reinit() {
		HLogin.OConnectionConfig cc = _app.getConnectionConfig();
		cleanup();
		_browser.cleanup();
		_browser.log( "###", HGUILocal.Colors.BLUE );
		cc._password = Crypt.SHA1( cc._password );
		_browser.log( " Connecting to server: " + cc._host + " to port " + cc._port + ".\n"  );
		try {
			_client = new HClient( _app );
			_client.connect( cc._host, cc._port );
			System.out.println( "Starting connection thread ..." );
			_client.start();
			_client.waitUntilRunning();
			System.out.println( "Connection thread started." );
			_client.println( "login:" + CLIENT_VERSION + ":" + cc._login + ":" + cc._password );
			_app.setName( cc._login );
			_app.setClient( _client );
			_browser.setClient( _client );
		} catch ( Exception e ) {
			Con.err( "Connection error: " + e.getMessage() );
			JOptionPane.showMessageDialog( _gui,
					"GameGround client was unable to connect to server:\n" + e.getMessage(),
					"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
			_app.showLoginScreen();
			return;
		}
		_client.println( "get_logics" );
		_client.println( "get_partys" );
		_client.println( "get_players" );
		Sound.play( "service-login" );
	}
	public void handleError( String $message ) {
		javax.swing.JOptionPane.showMessageDialog( _gui,
				"The GameGround server reported error condition:\n" + $message,
				"GameGround - error ...", javax.swing.JOptionPane.ERROR_MESSAGE );
		_app.showLoginScreen();
	}
	public void handleWarning( String $message ) {
		javax.swing.JOptionPane.showMessageDialog( _gui,
				"The GameGround server issued a warning:\n" + $message,
				"GameGround - warning ...", javax.swing.JOptionPane.WARNING_MESSAGE );
	}
	public void handleParty( String $message ) {
		String[] toks = $message.split( ",", 2 );
		LogicParty lp = getPartyById( toks[0] );
		if ( lp._party != null ) {
			if ( lp._party._party.getGUI() != _gui._tabs.getSelectedComponent() ) {
				int idx = _gui._tabs.indexOfTab( lp._party.toString() );
				showNotification( idx );
			}
			lp._party._party.processMessage( toks[1] );
		}
	}
	public synchronized void processMessage( String $message ) {
		String[] tokens = $message.split( ":", 2 );
		String mnemonic = tokens[0];
		String argument = tokens.length > 1 ? tokens[1] : null;
		Method handler = _handlers.get( mnemonic );
		if ( handler != null ) {
			try {
				handler.invoke( this, argument );
			} catch ( java.lang.IllegalAccessException e ) {
				e.printStackTrace();
				System.exit( 1 );
			} catch ( java.lang.reflect.InvocationTargetException e ) {
				e.printStackTrace();
				System.exit( 1 );
			}
		} else {
			if ( _browser.getGUI() != _gui._tabs.getSelectedComponent() ) {
				int idx = _gui._tabs.indexOfTab( "Browser" );
				showNotification( idx );
			}
			_browser.processMessage( $message );
		}
	}
	void showNotification( int $idx ) {
		if ( _gui._tabs.getBackgroundAt( $idx ) != COLOR_NOTIFY ) {
			_gui._tabs.setBackgroundAt( $idx, COLOR_NOTIFY );
			Sound.play( "message-new-instant" );
		}
	}
	public void handlerDummy( String $msg ) {
		System.out.println( "Message processed by dummy handler: " + $msg + " in [HWorkArea]" );
	}
	public synchronized void cleanup() {
		_players.clear();
		for ( int i = 1, COUNT = _gui._tabs.getTabCount(); i < COUNT; ++ i )
			_gui._tabs.removeTabAt( 1 );
		for ( HLogicInfo l : _logics.values() ) {
			l.clear();
		}
		_logics.clear();
	}
	public synchronized void closeParty( HAbstractLogic $logic ) {
		String id = $logic.id();
		System.out.println( "Abandoning party: " + id );
		LogicParty lp = getPartyById( id );
		if ( lp != null ) {
			_client.println( "abandon:" + id );
			lp._party.removePlayer( _players.get( _app.getName() ) );
			lp._party._party = null;
		}
		_gui._tabs.setSelectedComponent( _browser.getGUI() );
		_gui._tabs.remove( $logic.getGUI() );
		_browser.reload();
	}

	public LogicParty getPartyById( String $id ) {
		java.util.Set<java.util.Map.Entry<String,HLogicInfo>> entSet = _logics.entrySet();
		java.util.Map.Entry<String,HLogicInfo> ent = null;
		java.util.Iterator<java.util.Map.Entry<String,HLogicInfo>> it = entSet.iterator();
		LogicParty lp = null;
		while ( it.hasNext() ) {
			ent = it.next();
			if ( ent != null ) {
				HLogicInfo info = ent.getValue();
				if ( info != null ) {
					Party p = info.getParty( $id );
					if ( p != null ) {
						lp = new LogicParty( info, p );
						break;
					}
				}
			}
		}
		return ( lp );
	}
	public void handleLogic( String $message ) {
		System.out.println( "GameGround serves [" + $message + "] logic." );
		String[] tokens = $message.split( ":", 2 );
		HLogicInfo l = _app.getSupportedLogic( tokens[ 0 ] );
		if ( l != null ) {
			System.out.println( "Client serves [" + $message + "] logic." );
			if ( tokens.length > 1 )
				l.setDefaults( tokens[1] );
			_logics.put( tokens[0], l );
			_browser.reload();
		}
	}
	public void handlePartyInfo( String $message ) {
		System.out.println( "New party: [" + $message + "]." );
		String[] tokens = $message.split( ",", 3 );
		HLogicInfo l = _logics.get( tokens[1] );
		l.addParty( tokens[0], new Party( tokens[0], tokens[2] ) );
		_browser.reload();
	}
	public void handlePartyClose( String $message ) {
		System.out.println( "Party [" + $message + "] closed." );
		LogicParty lp = getPartyById( $message );
		if ( lp != null ) {
			if ( lp._party._party != null ) {
				if ( _gui._tabs.getSelectedComponent() == lp._party._party.getGUI() )
					_gui._tabs.setSelectedComponent( _browser.getGUI() );
				_gui._tabs.remove( lp._party._party.getGUI() );
				lp._party._party = null;
			}
			lp._logicInfo.dropParty( $message );
		}
		_browser.reload();
	}
	public void handlePlayer( String $message ) {
		System.out.println( "Another player: [" + $message + "]." );
		String[] tokens = $message.split( ",", 2 );
		String name = tokens[0];
		Player p = _players.get( name );
		if ( p != null ) {
			for ( HLogicInfo l : _logics.values() )
				l.dropPlayer( p );
		} else {
			_players.put( name, p = new Player( name ) );
		}
		if ( tokens.length > 1 ) {
			String[] partys = tokens[1].split( "," );
			for ( String id : partys ) {
				LogicParty lp = getPartyById( id );
				if ( lp != null ) {
					lp._party.addPlayer( p );
					if ( ( lp._party._party == null ) && ( name.equals( _app.getName() ) ) ) {
						System.out.println( "Adding new local party: [" + lp._party + "]." );
						lp._party._party = lp._logicInfo.create( _app, id, lp._party._configuration );
						HGUIface gui = lp._party._party.getGUI();
						_gui._tabs.addTab( lp._party.toString(), gui );
						_gui._tabs.setSelectedComponent( gui );
						gui.onShow();
					}
				}
			}
		}
		_browser.reload();
	}
	public void handlePlayerQuit( String $message ) {
		Player p = _players.get( $message );
		if ( p != null ) {
			for ( HLogicInfo l : _logics.values() )
				l.dropPlayer( p );
			_players.remove( $message );
		}
		_browser.reload();
		System.out.println( "Player: [" + $message + "] removed." );
	}
	public void handleAccount( String $message ) {
		String[] tokens = $message.split( ",", 2 );
		if ( tokens[0].equals( _app.getName() ) ) {
			Account acc = new Account( _app, $message );
			if ( acc.confirmed() ) {
				// System.out.println( "account:" + acc.getConfiguration() );
				_client.println( "account:" + acc.getConfiguration() );
			}
		} else {
			Chat chat = Chat.showUserInfo( _app, $message );
			java.awt.Component c = chat.getGUI();
			_gui._tabs.addTab( tokens[0], c );
			_gui._tabs.setSelectedComponent( c );
		}
	}
	public Player getPlayer( String $name ) {
		return ( _players.get( $name ) );
	}
	public void gracefullShutdown() {
		((HBrowser.HGUILocal)_browser.getGUI()).onDisconnect();
	}
}

