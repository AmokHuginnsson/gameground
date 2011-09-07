import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;

public class SetBangTable extends JPanel implements MouseInputListener {
//--------------------------------------------//

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
	int _hovered = -1;
	int _virtSize = 0;
//--------------------------------------------//
	public SetBangTable() {
		addMouseMotionListener( this );
		addMouseListener( this );
	}
	public void mouseClicked( MouseEvent $event ) {
	}
	public void mouseMoved( MouseEvent $event ) {
		int hovered = -1;
		if ( _hovered != hovered ) {
			_hovered = hovered;
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
		_hovered = -1;
		repaint();
	}
	void drawCard( Graphics $g, int $x, int $y, int $size, int $type ) {
		int height = ( $size * 16 ) / 10;
		int rounding = $size / 8;
		Graphics2D g2D = (Graphics2D)$g;      
		g2D.setStroke( new BasicStroke(2F) );
		$g.setColor( Color.WHITE );
		$g.fillRoundRect( $x, $y, $size, height, rounding, rounding );
		$g.setColor( Color.BLACK );
		$g.drawRoundRect( $x, $y, $size, height, rounding, rounding );
		int color = $type % 3;
		$type /= 3;
		int count = $type % 3;
		$type /= 3;
		int shape = $type % 3;
		$type /= 3;
		int fill = $type % 3;
		switch ( color ) {
			case ( 0 ): {
				$g.setColor( Color.BLUE );
			} break;
			case ( 1 ): {
				$g.setColor( Color.GREEN );
			} break;
			case ( 2 ): {
				$g.setColor( Color.MAGENTA );
			} break;
		}
	}
	protected void paintComponent( Graphics g ) {
		/*
		 * Card relative dimensions:
		 * height : width
		 * 16 : 10
		 */
		super.paintComponent( g );
		java.awt.Dimension pd = getParent().getSize();
		int size = pd.getWidth() < pd.getHeight() ? (int)pd.getWidth() : (int)pd.getHeight();
		drawCard( g, 30, 30, 120, 0 );
	}
}

