/*
 * SGF parser Java library
 *
 * This library is part of GameGround project.
 * http://codestation.org/?h-action=menu-project&menu=submenu-project&page=&project=gameground
 *
 * (C) 2016 Marcin Konarski - All Rights Reserved
 */

import java.lang.Iterable;
import java.util.ArrayList;

abstract class TypeReference<T> {}

public class HTree<Element> implements Iterable<Element> {
	public class HNode<Element> implements Iterable<HNode<Element>> {
		ArrayList<HNode<Element>> _children = null;
		HTree<Element> _tree = null;
		HNode<Element> _parent = null;
		Element _value = null;
		public HNode( Element $value, HTree<Element> $tree, HNode<Element> $parent ) {
			_value = $value;
			_tree = $tree;
			++ _tree._size;
			_parent = $parent;
		}
		public HNode<Element> getChildAt( int $index ) {
			return ( _children != null ? ( ( ( $index >= 0 ) && ( $index < _children.size() ) ) ? _children.get( $index ) : null ) : null );
		}
		public HNode<Element> getParent() {
			return ( _parent );
		}
		int getChildCount() {
			return ( _children != null ? _children.size() : 0 );
		}
		public java.util.Iterator<HNode<Element>> iterator() {
			return ( _children != null ? _children.iterator() : null );
		}
		public HNode<Element> addNode( Element $value ) {
			if ( _children == null )
				_children = new ArrayList<HNode<Element>>();
			HNode<Element> node = null;
			_children.add( node = new HNode<Element>( $value, _tree, this ) );
			return ( node );
		}
		public Element value() {
			return ( _value );
		}
		public void clear() {
			-- _tree._size;
			_tree = null;
			_parent = null;
			if ( _children != null ) {
				for ( HNode<Element> n : _children )
					n.clear();
				_children = null;
			}
		}
	}
	public class Iterator implements java.util.Iterator<Element> {
		HNode<Element> _node = null;
		public Iterator( HNode<Element> $node ) {
			_node = $node;
		}
		public boolean hasNext() {
			return ( _node != null );
		}
		public Element next() {
			HNode<Element> node = _node;
			if ( _node != null )
				_node = _node.getChildAt( 0 );
			return ( node != null ? node.value() : null );
		}
		public void remove() {
		}
	}
	HNode<Element> _root;
	int _size = 0;

	public HTree() {
	}
	public HNode<Element> createNewRoot( Element $value ) {
		return ( _root = new HNode<Element>( $value, this, null ) );
	}

	/**
	 * Returns the root of the tree.
	 */
	public HNode<Element> getRoot() {
		return ( _root );
	}
	public void clear() {
		if ( _root != null )
			_root.clear();
		_root = null;
	}

	public int getSize() {
		return ( _size );
	}

	public Iterator iterator() {
		return ( new Iterator( ( _root != null ) && ( _root.getChildCount() > 0 ) ? _root.getChildAt( 0 ) : null ) );
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

}

