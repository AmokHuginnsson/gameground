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
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.tree.TreeSelectionModel;
import java.util.TreeSet;
import java.util.SortedSet;
import javax.swing.DefaultListModel;


class HPlayerSet {
	String _id;
	String _name;
	String _configuration;
	SortedSet<String> _players = java.util.Collections.synchronizedSortedSet( new TreeSet<String>() );

	public HPlayerSet( String $id, String $name ) {
		this( $id, $name, null );
	}
	public HPlayerSet( String $id, String $name, String $configuration ) {
		_id = $id;
		_name = $name;
		_configuration = $configuration;
	}
	public String toString() {
		return ( _name );
	}
	public void addPlayer( String $player ) {
		_players.add( $player );
	}
	public void removePlayer( String $player ) {
		_players.remove( $player );
	}
	public java.util.Iterator<String> peopleIterator() {
		return ( _players.iterator() );
	}
}

class HBrowser extends HAbstractLogic {
	public static final String LABEL = "browser";
	public class HGUILocal extends HGUIface implements TreeSelectionListener {
		public static final long serialVersionUID = 17l;
		public JTextField _msg;
		public JTextPane _logPad;
		public JTree _games;
		public JList _people;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void reinit() {
			_msg.requestFocusInWindow();
			_games.setModel( new DefaultTreeModel( new DefaultMutableTreeNode( new HPlayerSet( "root", "GameGround" ) ) ) );
			_games.getSelectionModel().setSelectionMode( TreeSelectionModel.SINGLE_TREE_SELECTION );
			_games.addTreeSelectionListener( this );
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public void onDisconnect() {
			_client.println( "quit" );
			_client.disconnect();
		}
		public void valueChanged(TreeSelectionEvent e) {
			DefaultMutableTreeNode node = (DefaultMutableTreeNode)_games.getLastSelectedPathComponent();
			updatePlayers( node );
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
		public void updateTagLib( org.swixml.SwingEngine $se ) {	}
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
	private static final boolean registered;
	static {
		registered = registerLogic();
	}
	HClient _client;
	HGUILocal _gui;
//--------------------------------------------//
	public HBrowser( GameGround $applet ) {
		super();
		init( _gui = new HGUILocal( LABEL ) );
		try {
			_handlers.put( "logic", HBrowser.class.getDeclaredMethod( "handleLogic", new Class[]{ String.class } ) );
			_handlers.put( "player", HBrowser.class.getDeclaredMethod( "handlePlayer", new Class[]{ String.class } ) );
			_handlers.put( "player_quit", HBrowser.class.getDeclaredMethod( "handlePlayerQuit", new Class[]{ String.class } ) );
		} catch ( java.lang.NoSuchMethodException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
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
	public void reinit() {
		GameGround gg = GameGround.getInstance();
		if ( gg.getClient() == null ) {
			HLogin l = (HLogin)gg.getLogic( HLogin.LABEL );
			HLogin.OConnectionConfig cc = l.getConnectionConfig();
			_gui.log( "###", HGUILocal.Colors.BLUE );
			_gui.log( " Connecting to server: " + cc._host + " to port " + cc._port + ".\n"  );
			try {
				_client = new HClient();
				_client.connect( cc._host, cc._port );
				_client.start();
				synchronized ( _client ) {
					_client.wait();
				}
				_client.println( "name:" + cc._name );
				_client.println( "get_logics" );
				_client.println( "get_games" );
				_client.println( "get_players" );
				gg.setClient( _client );
			} catch ( Exception e ) {
				JOptionPane.showMessageDialog( _gui,
						"GameGround client was unable to connect to server:\n" + e.getMessage(),
						"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
				gg.setFace( HLogin.LABEL );
				return;
			}
		}
	}
	public void handleLogic( String $message ) {
		System.out.println( "GameGround serves [" + $message + "] logic." );
		String[] tokens = $message.split( ":", 2 );
		HAbstractLogic l = GameGround.getInstance().getLogicBySymbol( tokens[ 0 ] );
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
		if ( node == $node )
			_gui.updatePlayers( node );
	}
	public void handlePlayer( String $message ) {
		DefaultMutableTreeNode node = null;
		String[] tokens = $message.split( ",", 3 );
		handlePlayerQuit( tokens[ 0 ] );
		if ( tokens.length == 1 ) {
			/*
			 * Player is not inside any game.
			 */
			node = (DefaultMutableTreeNode)_gui._games.getModel().getRoot();
		}
		addPlayer( node, tokens[ 0 ] );
		System.out.println( "Another player: [" + tokens[ 0 ] + "]." );
	}
	boolean removePlayer( DefaultMutableTreeNode $node, String $name ) {
		HPlayerSet ps = (HPlayerSet)$node.getUserObject();
		if ( ps._players.remove( $name ) ) {
			DefaultMutableTreeNode node = (DefaultMutableTreeNode)_gui._games.getLastSelectedPathComponent();
			if ( node == $node )
				_gui.updatePlayers( node );
			if ( ps._players.isEmpty() && $node.isLeaf() )
				((DefaultMutableTreeNode)$node.getParent()).remove( $node );
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
}

