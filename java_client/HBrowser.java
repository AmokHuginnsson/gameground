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
			_games.setModel( new DefaultTreeModel( new DefaultMutableTreeNode( "GameGround" ) ) );
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
			if (node == null)
				return;
			JOptionPane.showMessageDialog( this,
					"Games\n" + node,
					"GameGround - error ...", JOptionPane.ERROR_MESSAGE );
		};
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
			((DefaultMutableTreeNode)_gui._games.getModel().getRoot()).add( new DefaultMutableTreeNode( i._name ) );
			((DefaultTreeModel)_gui._games.getModel()).reload();
		}
	}
	public void handlePlayer( String $message ) {
		System.out.println( "Another player: [" + $message + "]." );
	}
}

