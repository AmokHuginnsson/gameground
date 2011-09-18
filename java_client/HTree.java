import java.util.LinkedList;
import java.util.Vector;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

public class HTree<Element> implements TreeModel {
	public class HNode<Element> {
		LinkedList<HNode<Element>> _children = null;
		Element _value = null;
	}
	HNode<Element> _root;
	private Vector<TreeModelListener> treeModelListeners = new Vector<TreeModelListener>();
	private SortedMap<String, HLogicInfo> _logics = null;
	private int _nonPrivateLogics = 0;

	public HTree() {
	}

	//////////////// Fire events //////////////////////////////////////////////

	/**
	 * The only event raised by this model is TreeStructureChanged with the
	 * root as path, i.e. the whole tree has changed.
	 */
	protected void reload() {
		int len = treeModelListeners.size();
		TreeModelEvent e = new TreeModelEvent(this, 
				new Object[] {new HNode<Element>()});
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
		HNode<Element> p = (HNode<Element>)parent;
		HNode<Element> child = null;
		int level = p.getLevel();
		if ( level == 0 ) {
			java.util.Set<java.util.Map.Entry<String,HLogicInfo>> entSet = _logics.entrySet();
			java.util.Map.Entry<String,HLogicInfo> ent = null;
			java.util.Iterator<java.util.Map.Entry<String,HLogicInfo>> it = entSet.iterator();
			int i = 0;
			while ( it.hasNext() ) {
				ent = it.next();
				if ( ent != null ) {
					HLogicInfo li = ent.getValue();
					if ( ! li._private ) {
						if ( i == index ) {
							child = new HNode<Element>( li );
							break;
						}
						++ i;
					}
				}
			}
		} else if ( level == 1 ) {
			java.util.Map.Entry<String,Party> ent = null;
			java.util.Iterator<java.util.Map.Entry<String,Party>> it = p._logic.partyIterator();
			int i = 0;
			while ( it.hasNext() ) {
				ent = it.next();
				if ( ent != null ) {
					if ( i == index ) {
						child = new HNode<Element>( ent.getValue() );
						break;
					}
					++ i;
				}
			}
		}
		return ( child );
	}

	/**
	 * Returns the number of children of parent.
	 */
	public int getChildCount(Object parent) {
		HNode<Element> p = (HNode<Element>)parent;
		int childCount = 0;
		int level = p.getLevel();
		if ( level == 0 )
			childCount = _nonPrivateLogics;
		else if ( level == 1 )
			childCount = p._logic.getPartysCount();
		return childCount;
	}

	/**
	 * Returns the index of child in parent.
	 */
	public int getIndexOfChild(Object parent, Object child) {
		HNode<Element> p = (HNode<Element>)parent;
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
		return ( _root );
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

