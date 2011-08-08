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
		PartysModelNode() {
		}
		PartysModelNode( HLogicInfo $logic ) {
			_logic = $logic;
		}
		PartysModelNode( Party $party ) {
			_party = $party;
		}
		public String toString() {
			return ( _party != null ? _party.toString() : ( _logic != null ? _logic.toString() : "GameGround" ) );
		}
		public int getLevel() {
			int level = -1;
			if ( ( _logic == null ) && ( _party == null ) )
				level = 0;
			else if ( _logic != null )
				level = 1;
			else if ( _party != null )
				level = 2;
			return ( level );
		}
	}
	private Vector<TreeModelListener> treeModelListeners = new Vector<TreeModelListener>();
	private SortedMap<String, HLogicInfo> _logics;

	public PartysModel( SortedMap<String, HLogicInfo> $logics ) {
		_logics = $logics;
	}

	//////////////// Fire events //////////////////////////////////////////////

	/**
	 * The only event raised by this model is TreeStructureChanged with the
	 * root as path, i.e. the whole tree has changed.
	 */
	protected void reload() {
		int len = treeModelListeners.size();
		TreeModelEvent e = new TreeModelEvent(this, 
				new Object[] {new PartysModelNode()});
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
	public Object getChild( Object parent, int index ) {
		PartysModelNode p = (PartysModelNode)parent;
		PartysModelNode child = null;
		int level = p.getLevel();
		if ( level == 0 ) {
			java.util.Set<java.util.Map.Entry<String,HLogicInfo>> entSet = _logics.entrySet();
			java.util.Map.Entry<String,HLogicInfo> ent = null;
			java.util.Iterator<java.util.Map.Entry<String,HLogicInfo>> it = entSet.iterator();
			int i = 0;
			while ( it.hasNext() ) {
				ent = it.next();
				if ( ent != null ) {
					if ( i == index ) {
						child = new PartysModelNode( ent.getValue() );
						break;
					}
					++ i;
				}
			}
		} else if ( level == 1 ) {
		}
		return ( child );
	}

	/**
	 * Returns the number of children of parent.
	 */
	public int getChildCount(Object parent) {
		PartysModelNode p = (PartysModelNode)parent;
		int childCount = 0;
		int level = p.getLevel();
		if ( level == 0 )
			childCount = _logics.size();
		else if ( level == 1 )
			childCount = p._logic.getPartysCount();
		return childCount;
	}

	/**
	 * Returns the index of child in parent.
	 */
	public int getIndexOfChild(Object parent, Object child) {
		PartysModelNode p = (PartysModelNode)parent;
		int index = -1;
		int childCount = getChildCount( parent );
		for ( int i = 0; i < childCount; ++ i ) {
			if ( getChild( parent, i ) == child ) {
				index = i;
				break;
			}
		}
		return ( index );
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
		return ( getChildCount( node ) == 0 );
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

