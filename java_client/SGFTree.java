import java.util.Arrays;
import java.io.BufferedReader;
import java.io.StringReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.awt.Image;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.Color;
import java.awt.Font;
import java.awt.Rectangle;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.event.MouseInputListener;

public class SGFTree extends JPanel implements MouseInputListener {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	HAbstractLogic _logic = null;
	GoImages _images = null;
	SGF _sgf = null;
	Font _font = new Font( "dialog", Font.BOLD, 10 );
	BasicStroke _stroke = new BasicStroke( 2 );
	int[] _path = null;
	int[] _maxWidth = null;
	int _maxTreeSize = 0;
	int _hoverX = -1;
	int _hoverY = -1;
	boolean _valid = false;
	HTree<SGF.Move>.HNode<SGF.Move> _hovered = null;
	Rectangle _currentRect = new Rectangle();
	static final int margin = 10;
//--------------------------------------------//
	public SGFTree() {
		addMouseMotionListener( this );
		addMouseListener( this );
	}
	void setLogic( HAbstractLogic $logic ) {
		_logic = $logic;
	}
	void setSGF( SGF $sgf ) {
		_sgf = $sgf;
	}
	int getDiameter() {
		int diameter = _images.getStoneSize();
		if ( diameter > 28 )
			diameter = 28;
		return ( diameter );
	}
	public void mouseClicked( MouseEvent $event ) {
		if ( _hovered != null )
			((Go.HGUILocal)_logic._gui).jumpToMove( _hovered );
	}
	public void mouseMoved( MouseEvent $event ) {
		int diameter = getDiameter();
		int cursorX = ( ( $event.getX() - margin / 2 ) / ( margin + diameter ) );
		int cursorY = ( ( $event.getY() - margin / 2 ) / ( margin + diameter ) );
		if ( ( _hoverX != cursorX ) || ( _hoverY != cursorY ) ) {
			_hoverX = cursorX;
			_hoverY = cursorY;
			_hovered = null;
			repaint();
		}
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
		_hoverX = -1;
		_hoverY = -1;
		_hovered = null;
		repaint();
	}
	public void setImages( GoImages $images ) {
		_images = $images;
	}
	protected void paintComponent( Graphics g ) {
		java.awt.Dimension d = getSize();
		super.paintComponent( g );
		Graphics2D g2d = (Graphics2D) g;
		g2d.setStroke( _stroke );
		byte stone = _sgf._firstToMove == SGF.Player.BLACK ? Goban.STONE.BLACK : Goban.STONE.WHITE;
		int moveNumber = 1;
		int diameter = getDiameter();
		if ( diameter > 28 )
			diameter = 28;
		int maxW = 0;
		int maxH = 0;
		g.setFont( _font );
		int treeSize = _sgf._tree.getSize();
		HTree<SGF.Move>.HNode<SGF.Move> currentMove = ((Go.HGUILocal)_logic._gui).currentMove();
		if ( treeSize > _maxTreeSize ) {
			_maxTreeSize = treeSize;
			_path = ( _path != null ) ? Arrays.copyOf( _path, _maxTreeSize ) : new int[_maxTreeSize];
			_maxWidth = ( _maxWidth != null ) ? Arrays.copyOf( _maxWidth, _maxTreeSize ) : new int[_maxTreeSize];
		}
		if ( _path != null )
			Arrays.fill( _path, 0 );
		if ( _maxWidth != null )
			Arrays.fill( _maxWidth, 0 );
		HTree<SGF.Move>.HNode<SGF.Move> n = ( _sgf._tree._root != null ) ? ( _sgf._tree._root.getChildCount() > 0 ? _sgf._tree._root.getChildAt( 0 ) : null ) : null;
		HTree<SGF.Move>.HNode<SGF.Move> last = n;
		HTree<SGF.Move>.HNode<SGF.Move> first = n;
		int path = 0;
		while ( n != null ) {
			while ( n != null ) {
				int jump = path - _maxWidth[moveNumber - 1];
				_maxWidth[moveNumber] = path;
				last = n;
				SGF.Move m = n.value();
				int x = moveNumber - 1;
				int y = path;
				if ( ( ( x + 1 ) * ( margin + diameter ) ) > maxW )
					maxW = ( x + 1 ) * ( margin + diameter ) + margin;
				if ( ( ( y + 1 ) * ( margin + diameter ) + margin ) > maxH )
					maxH = ( y + 1 ) * ( margin + diameter ) + margin;
				HTree<SGF.Move>.HNode<SGF.Move> next = n.getChildAt( 0 );
				if ( drawStone( g, x, y, stone, moveNumber, n == first, next == null, jump, n == currentMove ) )
					_hovered = n;
				stone = Goban.opponent( stone );
				++ moveNumber;
				n = next;
			}
			while ( last != null ) {
				-- moveNumber;
				stone = Goban.opponent( stone );
				last = last.getParent();
				if ( last != null ) {
					first = n = last.getChildAt( _path[moveNumber] + 1 );
					if ( n != null ) {
						++ _path[moveNumber];
						int zeroPathLen = getZeroPathLen( n );
						int maxWidth = 0;
						for ( int i = moveNumber; i < ( moveNumber + zeroPathLen ); ++ i ) {
							if ( _maxWidth[i] > maxWidth )
								maxWidth = _maxWidth[i];
						}
						path = maxWidth + 1;
						break;
					}
				}
			}
		}
		boolean sizeChanged = false;
		if ( maxW > d.width ) {
			d.width = maxW;
			d.height -= 16;
			sizeChanged = true;
		}
		if ( maxH > d.height ) {
			d.height = maxH;
			sizeChanged = true;
		}
		if ( sizeChanged ) {
			setPreferredSize( d );
			((JScrollPane)(getParent().getParent())).revalidate();
		}
		if ( ! sizeChanged && ! _valid ) {
			scrollRectToVisible( _currentRect );
			_valid = true;
		}
	}
	int getZeroPathLen( HTree<SGF.Move>.HNode<SGF.Move> $node ) {
		int len = 0;
		while ( $node != null ) {
			$node = $node.getChildAt( 0 );
			++ len;
		}
		return ( len );
	}
	boolean drawStone( Graphics $gc, int $xx, int $yy, int $color, int $number, boolean $first, boolean $last, int $jump, boolean $current ) {
		Image img = null;
		int diameter = getDiameter();
		if ( diameter > 28 )
			diameter = 28;
		if ( $color == Goban.STONE.BLACK ) {
			img = _images._black;
		} else {
			img = _images._whites[ ( ( $yy + $xx ) / diameter ) % GoImages.D_WHITE_LOOKS ];
		}
		int x = $xx * ( diameter + margin ) + margin;
		int y = $yy * ( diameter + margin ) + margin;
		boolean hovered = false;
		if ( $current ) {
			$gc.setColor( Color.red );
			int x0 = x - margin / 2;
			int y0 = y - margin / 2;
			int w = diameter + margin;
			int h = diameter + margin;
			_currentRect.setBounds( x0, y0, w, h );
			$gc.fill3DRect( x0, y0, w, h, true );
		} else if ( ( _hoverX == $xx ) && ( _hoverY == $yy ) ) {
			$gc.setColor( Color.gray );
			$gc.fill3DRect( x - margin / 2, y - margin / 2, diameter + margin, diameter + margin, true );
			hovered = true;
		}
		$gc.setColor( Color.black );
		if ( $first ) {
			if ( $jump > 1 ) {
				$gc.drawLine( x - margin - diameter / 2, y - margin - diameter / 2, x + diameter / 2, y + diameter / 2 );
				$gc.drawLine( x - margin - diameter / 2, y - margin - diameter / 2, x - margin - diameter / 2, y - ( margin + diameter ) * ( $jump - 1 ) - diameter / 4 - 4 );
			} else
				$gc.drawLine( x - margin - 4, y - margin - 4, x + diameter / 2, y + diameter / 2 );
		} else
			$gc.drawLine( x - margin / 2, y + diameter / 2, x + diameter / 2, y + diameter / 2 );
		if ( ! $last )
			$gc.drawLine( x + diameter / 2, y + diameter / 2, x + diameter + margin / 2, y + diameter / 2 );
		$gc.drawImage( img, x, y, diameter, diameter, this );
		String num = String.valueOf( $number );
		$gc.setColor( $color == Goban.STONE.BLACK ? Color.WHITE : Color.BLACK );
		int len = num.length();
		$gc.drawChars( num.toCharArray(), 0, len, x + diameter / 2 - len * 7 / 2, y + diameter / 2 + 4 );
		return ( hovered );
	}
}

