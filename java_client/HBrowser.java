import java.util.SortedMap;
import java.util.TreeMap;
import java.awt.event.ActionEvent;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JList;
import javax.swing.JTree;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JOptionPane;
import javax.swing.JTextPane;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.tree.TreeSelectionModel;
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
						PartysModel.PartysModelNode node = (PartysModel.PartysModelNode)_games.getLastSelectedPathComponent();
						if ( ( node != null ) && ( node.getLevel() == 2 ) )
							onJoin();
					}
				}
			};
			_games.setModel( _partysModel );
			_games.addMouseListener( ml );
			_msg.requestFocusInWindow();
		}
		public JTextPane getLogPad() {
			return ( _logPad );
		}
		public void onCreate() {
			GameCreator gc = new GameCreator( _app, _logics );
			if ( gc.confirmed() ) {
				_client.println( "create:" + gc.getConfiguration() );
			}
		}
		public void onJoin() {
		/*
			DefaultMutableTreeNode node = (DefaultMutableTreeNode)_games.getLastSelectedPathComponent();
			HPlayerSet ps = (HPlayerSet)node.getUserObject();
			HAbstractLogic l = _app.getLogicBySymbol( ps._id );
			HLogicInfo i = l.getInfo();
			_app.setFace( i._face );
			_client.println( "join:" + ps._login );
		*/
		}
		public void onDisconnect() {
			_client.println( "quit" );
			_client.disconnect();
		}
		public void valueChanged(TreeSelectionEvent e) {
			PartysModel.PartysModelNode node = (PartysModel.PartysModelNode)_games.getLastSelectedPathComponent();
			updatePlayers( node );
			_join.setEnabled( ( node != null ) && ( node.getLevel() == 2 ) );
		};
		public void updatePlayers( PartysModel.PartysModelNode $node ) {
			DefaultListModel lm = new DefaultListModel();
			_people.setModel( lm );
			if ( $node == null )
				return;
			if ( $node.getLevel() == 2 ) {
				String ent = "";
				java.util.Iterator<Player> it = $node._party.playerIterator();
				while ( it.hasNext() ) {
					ent = it.next().toString();
					lm.addElement( ent );
				}
			} else {
				java.util.Set<java.util.Map.Entry<String,Player>> entSet = _players.entrySet();
				java.util.Map.Entry<String,Player> ent = null;
				java.util.Iterator<java.util.Map.Entry<String,Player>> it = entSet.iterator();
				while ( it.hasNext() ) {
					ent = it.next();
					if ( ent != null ) {
						lm.addElement( ent.getValue() );
					}
				}
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
	HGUILocal _gui;
	HWorkArea _workArea;
	PartysModel _partysModel;
	private SortedMap<String, HLogicInfo> _logics = null;
	private SortedMap<String, Player> _players = null;
//--------------------------------------------//
	public HBrowser( GameGround $applet, HWorkArea $workArea, PartysModel $partysModel, SortedMap<String, HLogicInfo> $logics, SortedMap<String, Player> $players ) {
		super( $applet );
		_workArea = $workArea;
		_partysModel = $partysModel;
		_logics = $logics;
		_players = $players;
		init( _gui = new HGUILocal( LABEL ) );
	}
	public void init() { }
	public void reload() {
		((PartysModel)_gui._games.getModel()).reload();
	}
	public void cleanup() {
		reload();
		_gui.clearLog();
	}
	public void log( String $message, int $color ) {
		_gui.log( $message, $color );
	}
	public void log( String $message ) {
		_gui.log( $message );
	}
}

