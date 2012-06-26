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
	int[] _path = null;
	int _maxTreeSize = 0;
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
	public void mouseClicked( MouseEvent $event ) {
	}
	public void mouseMoved( MouseEvent $event ) {
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
	}
	public void setImages( GoImages $images ) {
		_images = $images;
	}
	protected void paintComponent( Graphics g ) {
		java.awt.Dimension d = getSize();
		super.paintComponent( g );
		Graphics2D g2d = (Graphics2D) g;
		g2d.setStroke( new BasicStroke( 3 ) );
		byte stone = _sgf._game._firstToMove == SGF.Game.Player.BLACK ? Goban.STONE.BLACK : Goban.STONE.WHITE;
		int moveNumber = 1;
		int diameter = _images.getStoneSize();
		if ( diameter > 28 )
			diameter = 28;
		int maxW = 0;
		int maxH = 0;
		g.setFont( _font );
		int treeSize = _sgf._game._tree.getSize();
		if ( treeSize > _maxTreeSize ) {
			_maxTreeSize = treeSize;
			_path = ( _path != null ) ? Arrays.copyOf( _path, _maxTreeSize ) : new int[_maxTreeSize];
		}
		if ( _path != null )
			Arrays.fill( _path, 0 );
		HTree<SGF.Game.Move>.HNode<SGF.Game.Move> n = ( _sgf._game._tree._root != null ) ? ( _sgf._game._tree._root.getChildCount() > 0 ? _sgf._game._tree._root.getChildAt( 0 ) : null ) : null;
		HTree<SGF.Game.Move>.HNode<SGF.Game.Move> last = n;
		HTree<SGF.Game.Move>.HNode<SGF.Game.Move> first = n;
		int path = 0;
		while ( n != null ) {
			while ( n != null ) {
				last = n;
				SGF.Game.Move m = n.value();
				int x = margin + ( diameter + margin ) * ( moveNumber - 1 );
				int y = margin + ( diameter + margin ) * path;
				if ( ( x + margin + diameter ) > maxW )
					maxW = x + margin + diameter;
				if ( ( y + margin + diameter ) > maxH )
					maxH = y + margin + diameter;
				HTree<SGF.Game.Move>.HNode<SGF.Game.Move> next = n.getChildAt( 0 );
				drawStone( g, x, y, stone, moveNumber, n == first, next == null );
				stone = Goban.opponent( stone );
				++ moveNumber;
				n = next;
			}
			while ( last != null ) {
				-- moveNumber;
				last = last.getParent();
				if ( last != null ) {
					first = n = last.getChildAt( _path[moveNumber - 1] + 1 );
					if ( n != null ) {
						Con.debug( "" + _path[moveNumber - 1] );
						++ _path[moveNumber - 1];
						++ path;
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
	}
	void drawStone( Graphics $gc, int $xx, int $yy, int $color, int $number, boolean $first, boolean $last ) {
		Image img = null;
		int diameter = _images.getStoneSize();
		if ( diameter > 28 )
			diameter = 28;
		if ( $color == Goban.STONE.BLACK ) {
			img = _images._black;
		} else {
			img = _images._whites[ ( ( $yy + $xx ) / diameter ) % GoImages.D_WHITE_LOOKS ];
		}
		$gc.setColor( Color.black );
		$gc.drawLine( $xx - margin / 2, $yy + ( ! $first ? diameter / 2 : - margin ), $xx + diameter / 2, $yy + diameter / 2 );
		if ( ! $last )
			$gc.drawLine( $xx + diameter / 2, $yy + diameter / 2, $xx + diameter + margin / 2, $yy + diameter / 2 );
		$gc.drawImage( img, $xx, $yy, diameter, diameter, this );
		String num = String.valueOf( $number );
		$gc.setColor( $color == Goban.STONE.BLACK ? Color.WHITE : Color.BLACK );
		int len = num.length();
		$gc.drawChars( num.toCharArray(), 0, len, $xx + diameter / 2 - len * 7 / 2, $yy + diameter / 2 + 4 );
	}
}

