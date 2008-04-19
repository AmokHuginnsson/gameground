import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.Container;
import java.awt.Color;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JPanel;
import javax.swing.JList;
import javax.swing.JTree;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.StyleConstants;
import javax.swing.text.Style;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.tree.TreeSelectionModel;
import java.util.TreeSet;
import java.util.SortedSet;
import java.util.Vector;
import javax.swing.DefaultListModel;

class HBrowser extends HAbstractLogic {
	public static final String LABEL = "browser";
	public class HGUILocal extends HGUIface implements TreeSelectionListener {
		public static final long serialVersionUID = 17l;
		public JTextField _msg;
		public JTextPane _logPad;
		public JTree _games;
		public JList _people;
		public JButton _join;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void init() {
			super.init();
			_games.getSelectionModel().setSelectionMode( TreeSelectionModel.SINGLE_TREE_SELECTION );
			_games.addTreeSelectionListener( this );
			java.awt.event.MouseListener ml = new java.awt.event.MouseAdapter() {
				public void mousePressed( java.awt.event.MouseEvent e ) {
					if ( e.getClickCount() == 2 ) {
						DefaultMutableTreeNode node = (DefaultMutableTreeNode)_games.getLastSelectedPathComponent();
						if ( ( node != null ) && ( node.getLevel() == 2 ) )
							onJoin();
					}
				}
			};
			_games.addMouseListener( ml );
		}
		public void reinit() {
			_msg.requestFocusInWindow();
			_games.setModel( new DefaultTreeModel( new DefaultMutableTreeNode( new HPlayerSet( "root", "GameGround" ) ) ) );
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public void onCreate() {
			DefaultMutableTreeNode node = null;
			node = (DefaultMutableTreeNode)_games.getModel().getRoot();
			int i = 0, childs = node.getChildCount();
			Vector<HPlayerSet> v = new Vector<HPlayerSet>();
			for ( i = 0; i < childs; ++ i ) {
				DefaultMutableTreeNode gameType = (DefaultMutableTreeNode)node.getChildAt( i );
				HPlayerSet ps = (HPlayerSet)gameType.getUserObject();
				v.add( ps );
			}
			GameCreator gc = new GameCreator( _app, v );
			if ( gc.confirmed() ) {
				_app.setFace( gc.getFace() );
				_client.println( "create:" + gc.getConfiguration() );
			}
		}
		public void onJoin() {
			DefaultMutableTreeNode node = (DefaultMutableTreeNode)_games.getLastSelectedPathComponent();
			HPlayerSet ps = (HPlayerSet)node.getUserObject();
			HAbstractLogic l = _app.getLogicBySymbol( ps._id );
			HLogicInfo i = l.getInfo();
			_app.setFace( i._face );
			_client.println( "join:" + ps._name );
		}
		public void onDisconnect() {
			_client.println( "quit" );
			_client.disconnect();
		}
		public void valueChanged(TreeSelectionEvent e) {
			DefaultMutableTreeNode node = (DefaultMutableTreeNode)_games.getLastSelectedPathComponent();
			updatePlayers( node );
			_join.setEnabled( ( node != null ) && ( node.getLevel() == 2 ) );
		};
		public void updatePlayers( DefaultMutableTreeNode $node ) {
			DefaultListModel lm = new DefaultListModel();
			_people.setModel( lm );
			if ( $node == null )
				return;
			HPlayerSet ps = (HPlayerSet)$node.getUserObject();
			String ent = "";
			java.util.Iterator<String> it = ps.peopleIterator();
			while ( it.hasNext() ) {
				ent = it.next();
				lm.addElement( ent );
			}
			_people.setModel( lm );
		}
		public void updateTagLib( XUL $xul ) { }
		public Action onMessage = new AbstractAction() {
			public static final long serialVersionUID = 17l;
			public void actionPerformed( ActionEvent $event ) {
				String msg = _msg.getText();
				if ( msg.length() > 0 ) {
					_client.println( "msg:" + msg );
					_msg.setText( "" );
				}
			}
		};
	}
//--------------------------------------------//
	public static final long serialVersionUID = 17l;
	HClient _client;
	HGUILocal _gui;
//--------------------------------------------//
	public HBrowser( GameGround $applet ) {
		super( $applet );
		init( _gui = new HGUILocal( LABEL ) );
		_info = new HLogicInfo( "", "browser", "" );
		try {
			_handlers.put( "logic", HBrowser.class.getDeclaredMethod( "handleLogic", new Class[]{ String.class } ) );
			_handlers.put( "game", HBrowser.class.getDeclaredMethod( "handleGame", new Class[]{ String.class } ) );
			_handlers.put( "player", HBrowser.class.getDeclaredMethod( "handlePlayer", new Class[]{ String.class } ) );
			_handlers.put( "player_quit", HBrowser.class.getDeclaredMethod( "handlePlayerQuit", new Class[]{ String.class } ) );
		} catch ( java.lang.NoSuchMethodException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void reinit() {
		if ( _app.getClient() == null ) {
			HLogin l = (HLogin)_app.getLogic( HLogin.LABEL );
			HLogin.OConnectionConfig cc = l.getConnectionConfig();
			_gui.clearLog();
			_gui.log( "###", HGUILocal.Colors.BLUE );
			_gui.log( " Connecting to server: " + cc._host + " to port " + cc._port + ".\n"  );
			try {
				_client = new HClient( _app );
				_client.setLogic( this );
				_client.connect( cc._host, cc._port );
				System.out.println( "Starting connection thread ..." );
				_client.start();
				_client.waitUntilRunning();
				System.out.println( "Connection thread started." );
				_client.println( "name:" + cc._name );
				_app.setName( cc._name );
				_app.setClient( _client );
			} catch ( Exception e ) {
				e.printStackTrace();
				JOptionPane.showMessageDialog( _gui,
						"GameGround client was unable to connect to server:\n" + e.getMessage(),
						"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
				_app.setFace( HLogin.LABEL );
				return;
			}
		} else {
			_client.println( "abandon" );
		}
		_client.println( "get_logics" );
		_client.println( "get_games" );
		_client.println( "get_players" );
	}
	public void handleLogic( String $message ) {
		System.out.println( "GameGround serves [" + $message + "] logic." );
		String[] tokens = $message.split( ":", 2 );
		HAbstractLogic l = _app.getLogicBySymbol( tokens[ 0 ] );
		if ( l != null ) {
			l.setDefaults( tokens[ 1 ] );
			HLogicInfo i = l.getInfo();
			((DefaultMutableTreeNode)_gui._games.getModel().getRoot()).add( new DefaultMutableTreeNode( new HPlayerSet( tokens[ 0 ], i._name, tokens[ 1 ] ) ) );
			((DefaultTreeModel)_gui._games.getModel()).reload();
		}
	}
	void addPlayer( DefaultMutableTreeNode $node, String $name ) {
		((HPlayerSet)$node.getUserObject()).addPlayer( $name );
		DefaultMutableTreeNode node = (DefaultMutableTreeNode)_gui._games.getLastSelectedPathComponent();
		TreePath path = null;
		if ( node != null )
			path = new TreePath( node.getPath() );
		if ( node == $node )
			_gui.updatePlayers( node );
		((DefaultTreeModel)_gui._games.getModel()).reload();
		if ( path != null )
			_gui._games.setSelectionPath( path );
	}
	public void handleGame( String $message ) {
	}
	public void handlePlayer( String $message ) {
		DefaultMutableTreeNode node = null;
		String[] tokens = $message.split( ",", 4 );
		handlePlayerQuit( tokens[ 0 ] );
		node = (DefaultMutableTreeNode)_gui._games.getModel().getRoot();
		if ( tokens.length > 1 ) {
			/*
			 * Player is inside some game.
			 * Lets find game type in the tree.
			 */
			int i = 0, childs = node.getChildCount();
			for ( i = 0; i < childs; ++ i ) {
				DefaultMutableTreeNode gameType = (DefaultMutableTreeNode)node.getChildAt( i );
				HPlayerSet ps = (HPlayerSet)gameType.getUserObject();
				if ( tokens[ 1 ].compareTo( ps._id ) == 0 ) {
					node = gameType;
					break;
				}
			}
			if ( i >= childs ) {
				System.out.println( "Internal logic implementation error." );
				System.exit( 1 );
			}
			/*
			 * Lets look for specyfic game.
			 */
			childs = node.getChildCount();
			DefaultMutableTreeNode game = null;
			for ( i = 0; i < childs; ++ i ) {
				game = (DefaultMutableTreeNode)node.getChildAt( i );
				HPlayerSet ps = (HPlayerSet)game.getUserObject();
				if ( tokens[ 2 ].compareTo( ps._name ) == 0 )
					break;
			}
			if ( game != null ) {
				node = game;
			} else {
				DefaultMutableTreeNode newGame = null;
				node.add( newGame = new DefaultMutableTreeNode( new HPlayerSet( tokens[ 1 ], tokens[ 2 ], tokens[ 3 ] ) ) );
				node = newGame;
			}
		}
		addPlayer( node, tokens[ 0 ] );
		System.out.println( "Another player: [" + tokens[ 0 ] + "]." );
	}
	boolean removePlayer( DefaultMutableTreeNode $node, String $name ) {
		HPlayerSet ps = (HPlayerSet)$node.getUserObject();
		if ( ps._players.remove( $name ) ) {
			DefaultMutableTreeNode node = (DefaultMutableTreeNode)_gui._games.getLastSelectedPathComponent();
			TreePath path = null;
			if ( node != null )
				path = new TreePath( node.getPath() );
			if ( node == $node )
				_gui.updatePlayers( node );
			if ( ps._players.isEmpty() && $node.isLeaf() )
				((DefaultMutableTreeNode)$node.getParent()).remove( $node );
			((DefaultTreeModel)_gui._games.getModel()).reload();
			if ( path != null )
				_gui._games.setSelectionPath( path );
			return ( false );
		} else {
			int childs = $node.getChildCount();
			for ( int i = 0; i < childs; ++ i )
				if ( ! removePlayer( (DefaultMutableTreeNode)$node.getChildAt( i ), $name ) )
					return ( false );
		}
		return ( true );
	}
	public void handlePlayerQuit( String $message ) {
		removePlayer((DefaultMutableTreeNode) _gui._games.getModel().getRoot(), $message );
		System.out.println( "Player: [" + $message + "] removed." );
	}
	public void cleanup() {}
	static boolean registerLogic( GameGround $app ) {
		try {
			$app.registerLogic( LABEL, new HBrowser( $app ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( true );
	}
}

