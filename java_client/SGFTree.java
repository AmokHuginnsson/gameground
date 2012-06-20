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
		byte stone = _sgf._game._firstToMove == SGF.Game.Player.BLACK ? Goban.STONE.BLACK : Goban.STONE.WHITE;
		int moveNumber = 1;
		int diameter = _images.getStoneSize();
		if ( diameter > 28 )
			diameter = 28;
		int maxW = 0;
		int maxH = 0;
		int margin = 10;
		g.setFont( _font );
		for ( SGF.Game.Move m : _sgf._game._tree ) {
			int x = margin + ( diameter + margin ) * ( moveNumber - 1 );
			int y = margin;
			if ( ( x + margin + diameter ) > maxW )
				maxW = x + margin + diameter;
			if ( ( y + margin + diameter ) > maxH )
				maxH = y + margin + diameter;
			drawStone( g, x, y, stone, moveNumber );
			stone = Goban.opponent( stone );
			++ moveNumber;
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
	void drawStone( Graphics $gc, int $xx, int $yy, int $color, int $number ) {
		Image img = null;
		int diameter = _images.getStoneSize();
		if ( diameter > 28 )
			diameter = 28;
		if ( $color == Goban.STONE.BLACK ) {
			img = _images._black;
		} else {
			img = _images._whites[ ( ( $yy + $xx ) / diameter ) % GoImages.D_WHITE_LOOKS ];
		}
		$gc.drawImage( img, $xx, $yy, diameter, diameter, this );
		String num = String.valueOf( $number );
		$gc.setColor( $color == Goban.STONE.BLACK ? Color.WHITE : Color.BLACK );
		int len = num.length();
		$gc.drawChars( num.toCharArray(), 0, len, $xx + diameter / 2 - len * 7 / 2, $yy + diameter / 2 + 4 );
	}
}

