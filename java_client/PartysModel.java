import java.util.SortedMap;
import java.util.Vector;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

public class PartysModel implements TreeModel {
	public class PartysModelNode {
		public HLogicInfo _logic = null;
		public Party _party = null;
		public Player _player = null;
		PartysModelNode() {
		}
		PartysModelNode( HLogicInfo $logic ) {
			_logic = $logic;
		}
		PartysModelNode( HLogicInfo $logic ) {
			_party = $party;
		}
		PartysModelNode( Player $player ) {
			_player = $player;
		}
		public String toString() {
			return ( _player != null ? _player.toString() : ( _party != null ? _party.toString() : ( _logic != null ? _logic.toString() : "GameGround" ) ) );
		}
	}
	private Vector<TreeModelListener> treeModelListeners = new Vector<TreeModelListener>();
	private SortedMap<String, HLogicInfo> _logics;
	private SortedMap<String, Party> _partys;

	public PartysModel( SortedMap<String, HLogicInfo> $logics, SortedMap<String, Party> $partys ) {
		_logics = $logics;
		_partys = $partys;
	}

	//////////////// Fire events //////////////////////////////////////////////

	/**
	 * The only event raised by this model is TreeStructureChanged with the
	 * root as path, i.e. the whole tree has changed.
	 */
	protected void fireTreeStructureChanged(Person oldRoot) {
		int len = treeModelListeners.size();
		TreeModelEvent e = new TreeModelEvent(this, 
				new Object[] {oldRoot});
		for (TreeModelListener tml : treeModelListeners) {
			tml.treeStructureChanged(e);
		}
	}


	//////////////// TreeModel interface implementation ///////////////////////

	/**
	 * Adds a listener for the TreeModelEvent posted after the tree changes.
	 */
	public void addTreeModelListener(TreeModelListener l) {
		treeModelListeners.addElement(l);
	}

	/**
	 * Returns the child of parent at index index in the parent's child array.
	 */
	public Object getChild(Object parent, int index) {
		PartysModelNode p = (PartysModelNode)parent;
		PartysModelNode child = null;
		if ( ( p._logic == null ) && ( p._party == null ) && ( p._player == null ) ) {
		} else if ( p._logic != null ) {
		} else if ( p._party != null ) {
		}
		return ( child );
	}

	/**
	 * Returns the number of children of parent.
	 */
	public int getChildCount(Object parent) {
		PartysModelNode p = (PartysModelNode)parent;
		int childCount = 0;
		if ( ( p._logic == null ) && ( p._party == null ) && ( p._player == null ) )
			childCount = _logics.count();
		else if ( p._logic != null ) {
		} else if ( p._party != null )
			childCount = p._party.getPlayerCount();
		return childCount;
	}

	/**
	 * Returns the index of child in parent.
	 */
	public int getIndexOfChild(Object parent, Object child) {
		Person p = (Person)parent;
		if (showAncestors) {
			int count = 0;
			Person father = p.getFather();
			if (father != null) {
				count++;
				if (father == child) {
					return 0;
				}
			}
			if (p.getMother() != child) {
				return count;
			}
			return -1;
		}
		return p.getIndexOfChild((Person)child);
	}

	/**
	 * Returns the root of the tree.
	 */
	public Object getRoot() {
		return new PartysModelNode();
	}

	/**
	 * Returns true if node is a leaf.
	 */
	public boolean isLeaf(Object node) {
		PartysModelNode p = (PartysModelNode)parent;
		boolean isLeaf = false;
		if ( ( p._logic == null ) && ( p._party == null ) && ( p._player == null ) ) {
			if ( _logic.count() == 0 )
				isLeaf = true;
		} else if ( p._logic != null ) {
		} else if ( p._party != null ) {
		} else {
			assert p._player != null : "Inconsistient PartysModelNode!";
			isLeaf = true;
		}
		return ( isLeaf );
	}

	/**
	 * Removes a listener previously added with addTreeModelListener().
	 */
	public void removeTreeModelListener(TreeModelListener l) {
		treeModelListeners.removeElement(l);
	}

	/**
	 * Messaged when the user has altered the value for the item
	 * identified by path to newValue.  Not used by this model.
	 */
	public void valueForPathChanged(TreePath path, Object newValue) {
		System.out.println("*** valueForPathChanged : "
				+ path + " --> " + newValue);
	}
}

