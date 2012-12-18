import java.util.SortedMap;
import java.util.TreeMap;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.awt.Font;
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
import javax.swing.DefaultListModel;

class HBrowser extends HAbstractLogic {
	public static final String LABEL = "browser";
	public class HGUILocal extends HGUIface implements TreeSelectionListener {
		public static final long serialVersionUID = 17l;
		public JTextField _msg;
		public JTextPane _logPad;
		public JTree _games;
		public JList<Player> _people;
		public JButton _join;
		public HGUILocal( String $resource ) {
			super( $resource );
		}
		public void init() {
			super.init();
			_games.getSelectionModel().setSelectionMode( TreeSelectionModel.SINGLE_TREE_SELECTION );
			_games.addTreeSelectionListener( this );
			java.awt.event.MouseListener partysML = new java.awt.event.MouseAdapter() {
				public void mousePressed( MouseEvent e ) {
					if ( e.getClickCount() == 2 ) {
						int button = e.getButton();
						if ( button == MouseEvent.BUTTON1 ) {
							if ( _join.isEnabled() )
								onJoin();
						} else {
							TreePath path = _games.getPathForLocation( e.getX(), e.getY() );
							if ( path != null ) {
								PartysModel.PartysModelNode node = (PartysModel.PartysModelNode)path.getLastPathComponent();
								if ( ( node != null ) && ( node.getLevel() == 1 ) ) {
									onCreate( node._logic._name );
								}
							}
						}
					}
				}
			};
			java.awt.event.MouseListener usersML = new java.awt.event.MouseAdapter() {
				public void mousePressed( MouseEvent e ) {
					int selectedIndex = _people.getSelectedIndex();
					if ( ( selectedIndex >= 0 ) && ! _people.getCellBounds( selectedIndex, selectedIndex ).contains( e.getPoint() ) )
						_people.clearSelection();
					int clicks = e.getClickCount();
					if ( clicks == 2 ) {
						Player selected = _people.getSelectedValue();
						if ( ( selected != null ) && ! selected.toString().equals( _app.getName() ) ) {
							_client.println( "get_account:" + selected.toString() );
						} else {
							System.out.println( selected );
						}
					}
				}
			};
			_games.setModel( _partysModel );
			_games.addMouseListener( partysML );
			_people.addMouseListener( usersML );
			_app.setup().prepareFonts( _logPad.getFont() );
		}
		public JTextPane getLogPad() {
			Font f = _app.setup().getFont();
			if ( f != null )
				_logPad.setFont( f );
			return ( _logPad );
		}
		public void onCreate( String $logic ) {
			GameCreator gc = new GameCreator( _app, _logics, $logic );
			if ( gc.confirmed() ) {
				_client.println( "create:" + gc.getConfiguration() );
			}
		}
		public void onClose() {
		}
		public void onCreate() {
			onCreate( null );
		}
		public void onJoin() {
			PartysModel.PartysModelNode party = (PartysModel.PartysModelNode)_games.getLastSelectedPathComponent();
			_client.println( "join:" + party._party._id );
		}
		public void onAccount() {
			_client.println( "get_account" );
		}
		public void onDisconnect() {
			_client.println( "quit" );
			_client.disconnect();
		}
		public void valueChanged(TreeSelectionEvent e) {
			PartysModel.PartysModelNode node = (PartysModel.PartysModelNode)_games.getLastSelectedPathComponent();
			updatePlayers( node );
			if ( ( node != null ) && ( node.getLevel() == 2 ) ) {
				if ( _us == null )
					_us = _workArea.getPlayer( _app.getName() );
				_join.setEnabled( ! node._party.hasPlayer( _us ) );
			}
		};
		public void updatePlayers( PartysModel.PartysModelNode $node ) {
			DefaultListModel<Player> lm = new DefaultListModel<Player>();
			_people.setModel( lm );
			if ( $node == null )
				return;
			if ( $node.getLevel() == 2 ) {
				java.util.Iterator<Player> it = $node._party.playerIterator();
				while ( it.hasNext() ) {
					lm.addElement( it.next() );
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
	private Player _us = null;
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
	public void reload() {
		TreePath path = _gui._games.getSelectionPath();
		((PartysModel)_gui._games.getModel()).reload();
		if ( path != null )
			_gui._games.setSelectionPath( path );
		else
			_gui._games.setSelectionRow( 0 );
		_gui._msg.requestFocusInWindow();
	}
	public void cleanup() {
		reload();
		_gui.clearLog();
		_us = null;
	}
	public void log( String $message, int $color ) {
		_gui.log( $message, $color );
	}
	public void log( String $message ) {
		_gui.log( $message );
	}
	public void setClient( HClient $client ) {
		_client = $client;
	}
}

