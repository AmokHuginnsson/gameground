import java.lang.Iterable;
import java.util.Iterator;
import java.util.ArrayList;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

public class HTree<Element> implements TreeModel {
	public class HNode<Element> implements Iterable<HNode<Element>> {
		ArrayList<HNode<Element>> _children = null;
		Element _value = null;
		public HNode() {
			_value = null;
		}
		public HNode( Element $value ) {
			_value = $value;
		}
		HNode<Element> getChildAt( int $index ) {
			return ( _children != null ? _children.get( $index ) : null );
		}
		int getChildCount() {
			return ( _children != null ? _children.size() : 0 );
		}
		public Iterator<HNode<Element>> iterator() {
			return ( _children != null ? _children.iterator() : null );
		}
		public HNode<Element> addNode() {
			return ( addNode( null ) );
		}
		public HNode<Element> addNode( Element $value ) {
			if ( _children == null )
				_children = new ArrayList<HNode<Element>>();
			HNode<Element> node = null;
			_children.add( node = new HNode<Element>( $value ) );
			return ( node );
		}
		public Element value() {
			return ( _value );
		}
	}
	HNode<Element> _root;
	private ArrayList<TreeModelListener> treeModelListeners = new ArrayList<TreeModelListener>();

	public HTree() {
	}
	public HNode<Element> createNewRoot() {
		return ( _root = new HNode<Element>() );
	}

	/**
	 * Returns the root of the tree.
	 */
	public HNode<Element> getRoot() {
		return ( _root );
	}
	public void clear() {
		_root = null;
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
			tml.treeStructureChanged( e );
		}
	}


	//////////////// TreeModel interface implementation ///////////////////////

	/**
	 * Adds a listener for the TreeModelEvent posted after the tree changes.
	 */
	public void addTreeModelListener(TreeModelListener l) {
		treeModelListeners.add(l);
	}

	/**
	 * Returns the child of parent at index index in the parent's child array.
	 */
	public Object getChild( Object parent, int index ) {
		@SuppressWarnings("unchecked")
		HNode<Element> node = (HNode<Element>)parent;
		return ( node.getChildAt( index ) );
	}

	/**
	 * Returns the number of children of parent.
	 */
	public int getChildCount(Object parent) {
		@SuppressWarnings("unchecked")
		HNode<Element> node = (HNode<Element>)parent;
		return ( node.getChildCount() );
	}

	/**
	 * Returns the index of child in parent.
	 */
	public int getIndexOfChild(Object parent, Object child) {
		@SuppressWarnings("unchecked")
		HNode<Element> node = (HNode<Element>)parent;
		return ( node._children != null ? node._children.indexOf( child ) : -1 );
	}

	/**
	 * Returns true if node is a leaf.
	 */
	public boolean isLeaf(Object node) {
		@SuppressWarnings("unchecked")
		HNode<Element> n = (HNode<Element>)node;
		return ( n._children == null );
	}

	/**
	 * Removes a listener previously added with addTreeModelListener().
	 */
	public void removeTreeModelListener(TreeModelListener l) {
		treeModelListeners.remove(l);
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

