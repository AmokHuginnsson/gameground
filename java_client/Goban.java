import java.util.Arrays;
import java.util.Collections;
import java.util.ArrayList;
import java.util.StringTokenizer;
import java.io.BufferedReader;
import java.io.StringReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.FileOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.awt.Image;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.Color;
import java.awt.Font;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.BadLocationException;

public abstract class Goban extends JPanel implements MouseInputListener {
//--------------------------------------------//
	public static class PROTOCOL {
		public static final String SGF = "sgf";
	}

	public static class STONE {
		public static final byte BLACK = 'b';
		public static final byte WHITE = 'w';
		public static final byte NONE	= ' ';
		public static final byte INVALID = 'N';
		public static final byte WAIT = 'Z';
		public static final String NONE_NAME = "None";
		public static final String BLACK_NAME = "Black";
		public static final String WHITE_NAME = "White";
	}
	public static final long serialVersionUID = 7l;
	public static final int D_MARGIN = 20;
	static final String SGF_DIR_PROP = "sgf-dir";
	static final int ALL = 30000;
	static final SGF.Position[] _markers = new SGF.Position[] {
			SGF.Position.TRIANGLE,
			SGF.Position.SQUARE,
			SGF.Position.CIRCLE,
			SGF.Position.MARK,
			SGF.Position.BLACK_TERITORY,
			SGF.Position.WHITE_TERITORY
	};
	HAbstractLogic _logic = null;
	int _size = Go.GOBAN_SIZE.NORMAL;
	double _diameter = -1;
	int _cursorX = -1;
	int _cursorY = -1;
	int _virtSize = 0;
	int _viewMoveNo = 0;
	int _commentStart = 0;
	int _commentLength = 0;
	GoImages _images = null;
	Font _font = null;
	int _fontSize = -1;
	SGF _sgf = null;
	HTree<SGF.Move>.HNode<SGF.Move> _viewMove = null;
	HTree<SGF.Move>.HNode<SGF.Move> _currentMove = null;
	byte[] _stones = new byte[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL];
	ArrayList<HTree<SGF.Move>.HNode<SGF.Move>> _branch = new ArrayList<HTree<SGF.Move>.HNode<SGF.Move>>();
	StringBuilder _selection = new StringBuilder();
//--------------------------------------------//
	public Goban() {
		Arrays.fill( _stones, STONE.NONE );
		addMouseMotionListener( this );
		addMouseListener( this );
	}
	void setLogic( HAbstractLogic $logic ) {
		_logic = $logic;
	}
	public void mouseMoved( MouseEvent $event ) {
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		int cursorX = (int)( ( $event.getX() + ( _diameter / 2 ) - ( D_MARGIN + margin ) ) * ( _size - 1 ) );
		int cursorY = (int)( ( $event.getY() + ( _diameter / 2 ) - ( D_MARGIN + margin ) ) * ( _size - 1 ) );
		cursorX = ( cursorX >= 0 ? cursorX / inside : -1 );
		cursorY = ( cursorY >= 0 ? cursorY / inside : -1 );
		if ( ( _cursorX != cursorX ) || ( _cursorY != cursorY ) ) {
			_cursorX = cursorX;
			_cursorY = cursorY;
			repaint();
		}
	}
	public SGF getSGF() {
		return ( _sgf );
	}
	public void mouseDragged( MouseEvent $event ) {
	}
	public void mouseEntered( MouseEvent $event ) {
	}
	public void mouseReleased( MouseEvent $event ) {
	}
	public void mousePressed( MouseEvent $event ) {
	}
	public void mouseExited( MouseEvent $event ) {
		_cursorX = -1;
		_cursorY = -1;
		repaint();
	}
	public void setImages( GoImages $images ) {
		_images = $images;
	}
	void setSize( int $size ) {
		_size = $size;
		_virtSize = 0;
		getPreferredSize();
		repaint();
	}
	void updateSGF( String $sgfData ) {
		BufferedReader br = new BufferedReader( new StringReader( $sgfData ) );
		updateSGF( br );
	}
	void selectBranch( HTree<SGF.Move>.HNode<SGF.Move> $node, boolean $jumpToMove, boolean $sendSelect ) {
		if ( $node != _viewMove ) {
			int moveNo = 0;
			if ( ! _branch.contains( $node ) ) {
				int toEnd = 0;
				while ( $node.getChildAt( 0 ) != null ) {
					$node = $node.getChildAt( 0 );
					++ toEnd;
				}
				_branch.clear();
				while ( $node != null ) {
					_branch.add( $node );
					$node = $node.getParent();
					++ moveNo;
				}
				Collections.reverse( _branch );
				moveNo -= toEnd;
			} else {
				while ( $node != null ) {
					$node = $node.getParent();
					++ moveNo;
				}
			}
			if ( $jumpToMove )
				placeStones( moveNo > 0 ? moveNo - 1 : 0, $sendSelect );
		}
	}
	void selectBranch( HTree<SGF.Move>.HNode<SGF.Move> $node, boolean $jumpToMove ) {
		selectBranch( $node, $jumpToMove, ! ongoingMatch() );
	}
	void selectBranch( HTree<SGF.Move>.HNode<SGF.Move> $node ) {
		selectBranch( $node, true );
	}
	boolean onBranch( HTree<SGF.Move>.HNode<SGF.Move> $node ) {
		return ( _branch.contains( $node ) );
	}
	void placeStones( int $to, boolean $sendSelect ) {
		clear();
		DefaultStyledDocument log = (DefaultStyledDocument)_logic._gui._logPad.getStyledDocument();
		if ( _commentLength > 0 ) {
			try {
				log.remove( _commentStart, _commentLength );
			} catch ( BadLocationException ble ) {
			}
		}
		_commentLength = _sgf._comment.length();
		if ( ( _commentLength > 0 ) && ( $to == 0 ) ) {
			_commentStart = log.getLength();
			_logic._gui.log( "Game Started\n", HGUIface.Colors.OTHERGRAY, HGUIface.Style.BOLD );
			boolean needNL = _sgf._comment.charAt( _commentLength - 1 ) != '\n';
			_logic._gui.log( _sgf._comment + ( needNL ? "\n" : "" ), HGUIface.Colors.OTHERGRAY );
			_commentLength += 13;
			if ( needNL )
				++ _commentLength;
		}
		Arrays.fill( _stones, STONE.NONE );
		HTree<SGF.Move>.HNode<SGF.Move> root = _sgf._tree.getRoot();
		SGF.Setup s = root.value().setup();
		if ( s != null ) {
			ArrayList<SGF.Coord> coords = s.get( SGF.Position.BLACK );
			if ( coords != null )
				for ( SGF.Coord c : coords ) {
					move( c.col(), c.row(), STONE.BLACK );
			}
			coords = s.get( SGF.Position.WHITE );
			if ( coords != null )
				for ( SGF.Coord c : coords ) {
					move( c.col(), c.row(), STONE.WHITE );
			}
		}
		byte stone = _sgf._firstToMove == SGF.Player.BLACK ? STONE.BLACK : STONE.WHITE;
		int moveNumber = 0;
		_viewMove = _sgf._tree.getRoot();
		for ( HTree<SGF.Move>.HNode<SGF.Move> n : _branch ) {
			if ( moveNumber > $to )
				break;
			SGF.Move m = n.value();
			_viewMove = n;
			if ( m.type() == SGF.Move.Type.MOVE ) {
				if ( m.coord().col() != -97 ) /* Check if not pass. */
					move( m.col(), m.row(), stone );
				stone = opponent( stone );
			} else if ( m.type() == SGF.Move.Type.SETUP ) {
				edit( m.setup() );
			}
			++ moveNumber;
		}
		_viewMoveNo = moveNumber > 0 ? moveNumber - 1 : 0;
		((GobanHolderInterface)_logic._gui).setMoveSlider( _viewMoveNo, _branch.size() > 0 ? _branch.size() - 1 : 0 );
		SGF.Move m = ( _viewMove != null ? _viewMove.value() : null );
		if ( ( _viewMove == null ) && ( _sgf._tree.getRoot() != null ) ) 
			m = _sgf._tree.getRoot().value();
		toMove( ( moveNumber % 2 ) == 1 ? Go.STONE.BLACK : Go.STONE.WHITE, moveNumber - 1 );
		if ( m != null ) {
			SGF.Setup setup = m.setup();
			if ( setup != null ) {
				ArrayList<SGF.Coord> coords = setup.get( SGF.Position.BLACK_TERITORY );
				if ( coords != null ) {
					for ( SGF.Coord c : coords ) {
						if ( getStone( c.col(), c.row() ) == Go.STONE.WHITE )
							setStone( c.col(), c.row(), Go.STONE.DEAD_WHITE );
					}
				}
				coords = setup.get( SGF.Position.WHITE_TERITORY );
				if ( coords != null ) {
					for ( SGF.Coord c : coords ) {
						if ( getStone( c.col(), c.row() ) == Go.STONE.BLACK )
							setStone( c.col(), c.row(), Go.STONE.DEAD_BLACK );
					}
				}
			}
			if ( moveNumber > 1 ) {
				_commentLength = m.comment().length();
				if ( _commentLength > 0 ) {
					_commentStart = log.getLength();
					String prefix = "Move: " + ( moveNumber - 1 ) + "\n";
					_logic._gui.log( prefix, HGUIface.Colors.OTHERGRAY, HGUIface.Style.BOLD );
					boolean needNL = m.comment().charAt( _commentLength - 1 ) != '\n';
					_logic._gui.log( m.comment() + ( needNL ? "\n" : "" ), HGUIface.Colors.OTHERGRAY );
					_commentLength += prefix.length();
					if ( needNL )
						++ _commentLength;
				}
			}
		}
		placeByLogic();

		if ( $sendSelect && isAdmin() ) {
			/* Find _viewMove address in tree. */
			_selection.delete( 0, _selection.length() );
			if ( _viewMove != null ) {
				for ( int i = 0; i < _branch.size(); ++ i ) {
					HTree<SGF.Move>.HNode<SGF.Move> n = _branch.get( i );
					if ( n == _viewMove ) {
						_selection.insert( 0, i );
						break;
					}
					if ( n.getChildCount() > 1 ) {
						HTree<SGF.Move>.HNode<SGF.Move> c = _branch.get( i + 1 );
						for ( int j = 0; j < n.getChildCount(); ++ j ) {
							if ( c == n.getChildAt( j ) ) {
								_selection.append( ',' );
								_selection.append( j );
							}
						}
					}
				}
				_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP + _logic.id() + Go.PROTOCOL.SEP
						+ Go.PROTOCOL.SELECT + Go.PROTOCOL.SEP + _selection );
			}
		}
		_logic._gui.repaint();
	}
	void placeStones( int $to ) {
		placeStones( $to, ! ongoingMatch() );
	}
	void placeStones() {
		placeStones( ALL );
	}
	abstract void toMove( byte $stone, int $moveNo );
	abstract boolean isAdmin();
	abstract boolean ongoingMatch();
	void select( String $path ) {
		StringTokenizer t = new StringTokenizer( $path, "," );
		int moveNo = Integer.parseInt( t.nextToken() );
		HTree<SGF.Move>.HNode<SGF.Move> m = _sgf._tree.getRoot();
		for ( int i = 0; i < moveNo; ++ i ) {
			int cc = m.getChildCount();
			int child = 0;
			if ( cc > 1 )
				child = Integer.parseInt( t.nextToken() );
			m = m.getChildAt( child );
		}
		if ( ( ! isAdmin() || ongoingMatch() ) && ( ( _currentMove == null ) || ( _viewMove == null ) || ( _viewMove == _currentMove ) ) )
			selectBranch( m );
		_currentMove = m;
		_logic._gui.repaint();
	}
	abstract void placeByLogic();
	void edit( SGF.Setup $setup ) {
		ArrayList<SGF.Coord> coords = $setup.get( SGF.Position.REMOVE );
		if ( coords != null ) {
			for ( SGF.Coord c : coords )
				setStone( c.col(), c.row(), STONE.NONE );
		}
		coords = $setup.get( SGF.Position.BLACK );
		if ( coords != null ) {
			for ( SGF.Coord c : coords )
				setStone( c.col(), c.row(), STONE.BLACK );
		}
		coords = $setup.get( SGF.Position.WHITE );
		if ( coords != null ) {
			for ( SGF.Coord c : coords )
				setStone( c.col(), c.row(), STONE.WHITE );
		}
	}
	abstract void clear();
	void updateSGF( BufferedReader $reader ) {
		_sgf.clear();
		_branch.clear();
		try {
			_sgf.load( $reader );
			_viewMove = null;
			_currentMove = null;
			((GobanHolderInterface)_logic._gui).updateSetup();
			if ( _sgf._tree._root != null ) {
				HTree<SGF.Move>.HNode<SGF.Move> n = _sgf._tree.getRoot();
				while ( n.getChildAt( 0 ) != null ) {
					n = n.getChildAt( 0 );
				}
				selectBranch( n, true, false );
			}
		} catch ( SGFException se ) {
			Con.err( "SGFException: " + se.getMessage() );
			System.exit( 1 );
		}
	}
	void goToFirst() {
		placeStones( 0 );
	}
	void goToPrevious() {
		placeStones( _viewMoveNo - 1 );
	}
	void goToNext() {
		placeStones( _viewMoveNo + 1 );
	}
	void goToLast() {
		placeStones();
	}
	void choosePreviousPath() {
		if ( ( _viewMove != null ) && ( _viewMove.getChildCount() > 1 ) ) {
			int idx = _branch.indexOf( _viewMove );
			if ( ( idx >= 0 ) && ( idx < ( _branch.size() - 1 ) ) ) {
				HTree<SGF.Move>.HNode<SGF.Move> next = _branch.get( idx + 1 );
				int childCount = _viewMove.getChildCount();
				idx = -1;
				for ( int i = 0; i < childCount; ++ i ) {
					if ( _viewMove.getChildAt( i ) == next ) {
						if ( i > 0 ) {
							selectBranch( _viewMove.getChildAt( i - 1 ), false );
							_logic.getGUI().repaint();
						}
						break;
					}
				}
			}
		}
	}
	void chooseNextPath() {
		if ( ( _viewMove != null ) && ( _viewMove.getChildCount() > 1 ) ) {
			int idx = _branch.indexOf( _viewMove );
			if ( ( idx >= 0 ) && ( idx < ( _branch.size() - 1 ) ) ) {
				HTree<SGF.Move>.HNode<SGF.Move> next = _branch.get( idx + 1 );
				int childCount = _viewMove.getChildCount();
				idx = -1;
				for ( int i = 0; i < childCount; ++ i ) {
					if ( _viewMove.getChildAt( i ) == next ) {
						if ( i < ( childCount - 1 ) ) {
							selectBranch( _viewMove.getChildAt( i + 1 ), false );
							_logic.getGUI().repaint();
						}
						break;
					}
				}
			}
		}
	}
	public Dimension getPreferredSize() {
		java.awt.Dimension pd = getParent().getSize();
		int size = pd.getWidth() < pd.getHeight() ? (int)pd.getWidth() : (int)pd.getHeight();
		if ( ( _virtSize != size ) && ( _images != null ) && ( size > 0 ) ) {
			_virtSize = size - 2 * D_MARGIN;
			int margin = _virtSize / ( _size + 4 );
			int inside = _virtSize - 2 * margin;
			_diameter = inside / ( _size - 1 );
			_images.regenerate( _virtSize, (int)_diameter );
		}
		return ( new Dimension( size, size ) );
	}
	void drawGoban( Graphics g ) {
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		g.drawImage( _images._background, D_MARGIN, D_MARGIN, _virtSize, _virtSize, Color.black, this );
		g.setColor( Color.black );
		for ( int i = 0; i < _size; ++ i ) {
			g.drawLine( D_MARGIN + margin, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ),
					D_MARGIN + _virtSize - margin, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) );
			g.drawLine( D_MARGIN + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + _virtSize - margin,
					D_MARGIN + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + margin );
			g.drawLine( D_MARGIN + margin, D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ),
					D_MARGIN + _virtSize - margin, D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ) );
			g.drawLine( D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + _virtSize - margin,
					D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + margin );
		}
		int hoshi = 9;
		g.fillOval( D_MARGIN + _virtSize / 2 - hoshi / 2, D_MARGIN + _virtSize / 2 - hoshi / 2, hoshi, hoshi );
		int hoshiLine = 3;
		if ( _size == 9 )
			hoshiLine = 2;
		int offset = ( inside * hoshiLine ) / ( _size - 1 ) - hoshi / 2;
		int noffset = ( inside * ( ( _size - 1 ) - hoshiLine ) ) / ( _size - 1 ) - hoshi / 2;
		g.fillOval( D_MARGIN + margin + offset, D_MARGIN + _virtSize / 2 - hoshi / 2, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + noffset, D_MARGIN + _virtSize / 2 - hoshi / 2, hoshi, hoshi );
		g.fillOval( D_MARGIN + _virtSize / 2 - hoshi / 2, D_MARGIN + margin + offset, hoshi, hoshi );
		g.fillOval( D_MARGIN + _virtSize / 2 - hoshi / 2, D_MARGIN + margin + noffset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + offset, D_MARGIN + margin + offset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + noffset, D_MARGIN + margin + offset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + offset, D_MARGIN + margin + noffset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + noffset, D_MARGIN + margin + noffset, hoshi, hoshi );
		char[] label = new char[2];
		g.setColor( Color.white );
		for ( int i = 0; i < _size; ++ i ) {
			label[ 0 ] = (char)( 'A' + ( i < ( 'i' - 'a' ) ? i : i + 1 ) );
			g.drawChars( label, 0, 1, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) - 4, D_MARGIN - 3 );
			g.drawChars( label, 0, 1, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) - 4, D_MARGIN + _virtSize + 15 );
			label[ 1 ] = (char)( '0' + ( ( i + 1 ) % 10 ) );
			if ( i > 8 )
				label[ 0 ] = '1';
			else
				label[ 0 ] = STONE.NONE;
			g.drawChars( label, 0, 2, D_MARGIN - 18, D_MARGIN + margin + ( inside * ( _size - 1 - i ) ) / ( _size - 1 ) + 5 );
			g.drawChars( label, 0, 2, D_MARGIN + _virtSize + 2, D_MARGIN + margin + ( inside * ( _size - 1 - i ) ) / ( _size - 1 ) + 5 );
		}
	}
	void mark( int $col, int $row, SGF.Position $mark, Graphics $gc ) {
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		Graphics2D g2D = (Graphics2D)$gc;
		g2D.setStroke( new BasicStroke(2.5F) );
		$gc.setColor( getStone( $col, $row ) == STONE.BLACK ? Color.WHITE : Color.BLACK );
		switch ( $mark ) {
			case TRIANGLE: {
				$gc.drawLine( D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ) - (int)( _diameter / 8 ), 
						D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) + (int)( _diameter / 4 + 1 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) + (int)( _diameter / 4 ) + 1 - (int)( _diameter / 8 ) );
				$gc.drawLine( D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) + (int)( _diameter / 4 + 1 - (int)( _diameter / 8 ) ), 
						D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ) - (int)( _diameter / 8 ) );
				$gc.drawLine( D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) + (int)( _diameter / 4 + 2 - (int)( _diameter / 8 ) ), 
						D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) + (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) + (int)( _diameter / 4 ) + 2 - (int)( _diameter / 8 ) );
			} break;
			case SQUARE: {
				$gc.drawRect( D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ), (int)( _diameter / 2 ), (int)( _diameter / 2 ) );
			} break;
			case CIRCLE: {
				$gc.drawOval( D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ), (int)( _diameter / 2 ), (int)( _diameter / 2 ) );
			} break;
			case MARK: {
				$gc.drawLine( D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ), 
						D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) + (int)( _diameter / 4 + 1 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) + (int)( _diameter / 4 ) + 1 );
				$gc.drawLine( D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) + (int)( _diameter / 4 + 1 ), 
						D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) + (int)( _diameter / 4 + 1 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ) );
			} break;
			case BLACK_TERITORY: {
				$gc.drawImage( _images._black, D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						(int)( _diameter / 2 ), (int)( _diameter / 2 ), this );
			} break;
			case WHITE_TERITORY: {
				$gc.drawImage( _images._whites[( $row * _size + $col ) % GoImages.D_WHITE_LOOKS], D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 4 ),
						(int)( _diameter / 2 ), (int)( _diameter / 2 ), this );
			} break;
		}
	}
	void label( int $col, int $row, String $label, Graphics $gc ) {
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		byte stone = getStone( $col, $row );
		$gc.setColor( stone == STONE.BLACK ? Color.WHITE : Color.BLACK );
		if ( stone == STONE.NONE ) {
			int x1 = margin + ( inside * $col ) / ( _size - 1 ) - (int)( _diameter / 2 );
			int x2 = margin + ( inside * ( $col + 1 ) ) / ( _size - 1 ) - (int)( _diameter / 2 );
			int y1 = margin + ( inside * $row ) / ( _size - 1 ) - (int)( _diameter / 2 );
			int y2 = margin + ( inside * ( $row + 1 ) ) / ( _size - 1 ) - (int)( _diameter / 2 );
			$gc.drawImage( _images._background,
					D_MARGIN + x1 + 4, D_MARGIN + y1 + 4, D_MARGIN + x2 - 4, D_MARGIN + y2 - 4,
					x1 + 4, y1 + 4, x2 - 4, y2 - 4,
					this );
		}

		$gc.drawString( $label,
				D_MARGIN + margin + ( inside * $col ) / ( _size - 1 ) - (int)( ( _diameter - 2 ) / 3.52 ),
				D_MARGIN + margin + ( inside * $row ) / ( _size - 1 ) + (int)( _diameter / 10 ) + (int)( ( _diameter - 4 ) / 3.3 ) );
	}
	protected void paintComponent( Graphics g ) {
		super.paintComponent( g );
		drawGoban( g );
		drawStones( g );
		drawByLogic( g );
		SGF.Move m = null;
		if ( _viewMove != null ) {
			m = _viewMove.value();
			if ( m.type() == SGF.Move.Type.MOVE ) {
				mark( m.col(), m.row(), SGF.Position.CIRCLE, g );
			}
		}
		if ( ( _viewMove == null ) && ( _sgf._tree.getRoot() != null ) ) 
			m = _sgf._tree.getRoot().value();
		if ( m != null ) {
			SGF.Setup setup = m.setup();
			if ( setup != null ) {
				for ( SGF.Position p : _markers ) {
					ArrayList<SGF.Coord> coords = setup.get( p );
					if ( coords != null ) {
						for ( SGF.Coord c : coords ) {
							mark( c.col(), c.row(), p, g );
						}
					}
				}
				int fontSize = (int)( _diameter - 4 );
				if ( fontSize != _fontSize ) {
					_fontSize = fontSize;
					String fn = Font.MONOSPACED + "-" + _fontSize;
					_font = Font.decode( fn );
				}
				Font f = g.getFont();
				g.setFont( _font );
				for ( SGF.Setup.Label l : setup.labels() ) {
					label( l._coord.col(), l._coord.row(), l._label, g );
				}
				g.setFont( f );
			}
		}
	}
	protected void drawStone( int $xx, int $yy, int $color, boolean $alpha, Graphics $gc ) {
		Image img = null;
		boolean teritory = false;
		if ( $color == Go.STONE.TERITORY_BLACK ) {
			$color = STONE.BLACK;
			teritory = true;
		}
		if ( $color == Go.STONE.TERITORY_WHITE ) {
			$color = STONE.WHITE;
			teritory = true;
		}
		if ( $color == Go.STONE.DEAD_BLACK ) {
			$alpha = true;
			$color = STONE.BLACK;
		}
		if ( $color == Go.STONE.DEAD_WHITE ) {
			$alpha = true;
			$color = STONE.WHITE;
		}
		if ( $alpha ) {
			if ( $color == STONE.WHITE )
				img = _images._whitesAlpha[ ( $yy * _size + $xx ) % GoImages.D_WHITE_LOOKS ];
			else
				img = _images._blackAlpha;
		} else {
			if ( $color == STONE.WHITE )
				img = _images._whites[ ( $yy * _size + $xx ) % GoImages.D_WHITE_LOOKS ];
			else
				img = _images._black;
		}
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		if ( ! ( $alpha || teritory ) )
			$gc.drawImage( _images._shadow,
					D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( ( _diameter / 2 ) + ( _diameter / 10 ) ),
					D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) + (int)( _diameter / 10 ) - (int)( _diameter / 2 ), this );
		if ( ! teritory )
			$gc.drawImage( img, D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( _diameter / 2 ),
					D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) - (int)( _diameter / 2 ), this );
		else
			$gc.drawImage( img, D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( _diameter / 4 ),
					D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) - (int)( _diameter / 4 ),
					(int)( _diameter / 2 ), (int)( _diameter / 2 ), this );
	}
	private void drawStones( Graphics g ) {
		if ( _stones[0] != STONE.INVALID ) {
			int size = _size * _size;
			size = ( size < _stones.length ? size : _stones.length );
			for ( int i = 0; i < size; ++ i ) {
				byte stone = _stones[ i ];
				if ( ( stone != STONE.NONE ) && ( stone != Go.STONE.DAME ) )
					drawStone( i % _size, i / _size, stone, false, g );
			}
		}
	}
	void setStone( int $col, int $row, byte $stone ) {
		_stones[ $row * _size + $col ] = $stone;
	}
	byte getStone( int $col, int $row ) {
		byte stone = STONE.INVALID;
		if ( validCoords( $col, $row ) )
			stone = _stones[ $row * _size + $col ];
		return ( stone );
	}
	public static byte opponent( byte stone ) {
		return ( stone == STONE.WHITE ? STONE.BLACK : STONE.WHITE );
	}
	boolean validCoords( int x, int y ) {
		return ( ( x >= 0 ) && ( x < _size )
				&& ( y >= 0 ) && ( y < _size ) );
	}
	abstract void drawByLogic( Graphics g );
	abstract void move( int $col, int $row, byte $stone );
	void load() {
		Ini ini = _logic._app.ini();
		String startDir = ini.getProperty( SGF_DIR_PROP );
		JFileChooser fc = new JFileChooser();
		if ( startDir != null )
			fc.setCurrentDirectory( new File( startDir ) );
		fc.setAcceptAllFileFilterUsed( false );
		fc.setFileFilter( new FileNameExtensionFilter( "Smart Game Format", "sgf" ) );
		if ( fc.showOpenDialog( this ) == JFileChooser.APPROVE_OPTION ) {
			try {
				FileInputStream fis = new FileInputStream( fc.getSelectedFile() );
				BufferedReader inStream = new BufferedReader( new InputStreamReader( fis ) );
				SGF sgf = new SGF( SGF.GAME_TYPE.GO, "gameground-client" );
				try {
					sgf.load( inStream );
					ini.setProperty( SGF_DIR_PROP, fc.getCurrentDirectory().getPath() );
					ini.save();
					ByteArrayOutputStream baos = new ByteArrayOutputStream();
					PrintStream ps = new PrintStream( baos );
					sgf.save( ps, true );
					_logic._client.println( HAbstractLogic.PROTOCOL.CMD + HAbstractLogic.PROTOCOL.SEP + _logic.id() + HAbstractLogic.PROTOCOL.SEP + PROTOCOL.SGF + HAbstractLogic.PROTOCOL.SEP + baos.toString( "ISO-8859-2" ).replace( "\n", "\\n" ) );
				} catch ( java.io.UnsupportedEncodingException uee ) {
					Con.err( "java.io.UnsupportedEncodingException: " + uee.getMessage() );
				} catch ( SGFException se ) {
					Con.err( "SGFException: " + se.getMessage() );
				}
			} catch ( java.io.FileNotFoundException fnfe ) {
				Con.err( "java.io.FileNotFoundException: " + fnfe.getMessage() );
			}
		}
	}
	void save() {
		Ini ini = _logic._app.ini();
		String startDir = ini.getProperty( SGF_DIR_PROP );
		JFileChooser fc = new JFileChooser();
		if ( startDir != null )
			fc.setCurrentDirectory( new File( startDir ) );
		fc.setAcceptAllFileFilterUsed( false );
		fc.setFileFilter( new FileNameExtensionFilter( "Smart Game Format", "sgf" ) );
		if ( fc.showSaveDialog( this ) == JFileChooser.APPROVE_OPTION ) {
			try {
				String path = fc.getSelectedFile().getPath();
				ini.setProperty( SGF_DIR_PROP, fc.getCurrentDirectory().getPath() );
				ini.save();
				if ( path.indexOf( ".sgf" ) != ( path.length() - 4 ) )
					path += ".sgf";
				FileOutputStream fos = new FileOutputStream( path );
				PrintStream outStream = new PrintStream( fos );
				_sgf.save( outStream );
			} catch ( java.io.FileNotFoundException fnfe ) {
				Con.err( "java.io.FileNotFoundException: " + fnfe.getMessage() );
			}
		}
	}
	void dump( byte[] $stones ) {
		int i = 0;
		int size = Math.min( (int)Math.sqrt( $stones.length ), _size );
		System.out.print( "+" );
		for ( int c = 0; c < size; ++ c )
			System.out.print( "-" );
		System.out.println( "+" );
		for ( int r = 0; r < size; ++ r ) {
			System.out.print( "|" );
			for ( int c = 0; c < size; ++ c, ++ i ) {
				System.out.print( (char)$stones[i] );
			}
			System.out.println( "|" );
		}
		System.out.print( "+" );
		for ( int c = 0; c < size; ++ c )
			System.out.print( "-" );
		System.out.println( "+" );
	}
}

