import java.util.TreeSet;
import java.util.SortedSet;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.Point;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;
import java.awt.Polygon;

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
	public static final int D_MARGIN = 30;
	int[] _cards = null;
	int _hovered = -1;
	SortedSet<Integer> _selected = java.util.Collections.synchronizedSortedSet( new TreeSet<Integer>() );
	int _virtSize = 0;
//--------------------------------------------//
	public SetBangTable() {
		addMouseMotionListener( this );
		addMouseListener( this );
		/* for testing gui only */
		_cards = new int[12];
		_cards[0] = 0;
		_cards[1] = 3;
		_cards[2] = 6;
		_cards[3] = 8;
		_cards[4] = 15;
		_cards[5] = 22;
		_cards[6] = 27;
		_cards[7] = 38;
		_cards[8] = 45;
		_cards[9] = 52;
		_cards[10] = 54;
		_cards[11] = 58;
	}
	public void setCards( int[] $cards ) {
		_cards = $cards;
	}
	public void mouseClicked( MouseEvent $event ) {
	}
	public void mouseMoved( MouseEvent $event ) {
		int hovered = getHovered( $event.getPoint() );
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
		if ( _hovered != -1 ) {
			if ( _selected.contains( _hovered ) )
				_selected.remove( _hovered );
			else if ( _selected.size() < 3 )
				_selected.add( _hovered );
			repaint();
		}
	}
	public void mouseExited( MouseEvent $event ) {
		_hovered = -1;
		repaint();
	}
	void drawSymbol( Graphics g, int x, int y, int w, int h, int $shape, int $fill ) {
		switch ( $shape ) {
			case ( 0 ): {
				switch ( $fill ) {
					case ( 0 ): {
						g.drawRoundRect( x, y, w, h, h, h );
					} break;
					case ( 1 ): {
						g.fillRoundRect( x, y, w, h, h, h );
						g.drawRoundRect( x, y, w, h, h, h );
					} break;
					case ( 2 ): {
						Color c = g.getColor();
						g.setColor( new Color( c.getRed() + ( ( 255 - c.getRed() ) * 3 ) / 5,
									c.getGreen() + ( ( 255 - c.getGreen() ) * 3 ) / 5,
									c.getBlue() + ( ( 255 - c.getBlue() ) * 3 ) / 5 ) );
						g.fillRoundRect( x, y, w, h, h, h );
						g.setColor( c );
						g.drawRoundRect( x, y, w, h, h, h );
					} break;
				}
			} break;
			case ( 1 ): {
				Polygon p = new Polygon();
				p.addPoint( x, y + h / 2 );
				p.addPoint( x + w / 2, y );
				p.addPoint( x + w, y + h / 2 );
				p.addPoint( x + w / 2, y + h );
				switch ( $fill ) {
					case ( 0 ): {
						g.drawPolygon( p );
					} break;
					case ( 1 ): {
						g.fillPolygon( p );
						g.drawPolygon( p );
					} break;
					case ( 2 ): {
						Color c = g.getColor();
						g.setColor( new Color( c.getRed() + ( ( 255 - c.getRed() ) * 3 ) / 5,
									c.getGreen() + ( ( 255 - c.getGreen() ) * 3 ) / 5,
									c.getBlue() + ( ( 255 - c.getBlue() ) * 3 ) / 5 ) );
						g.fillPolygon( p );
						g.setColor( c );
						g.drawPolygon( p );
					} break;
				}
			} break;
			case ( 2 ): {
				int ox = w / 20;
				int oz = w / 18;
				int oy = w / 28;
				int r = w / 6;
				switch ( $fill ) {
					case ( 0 ): {
						g.drawArc( x, y, w / 2, h, 40, 175 );
						g.drawArc( x + w / 2, y, w / 2, h, 220, 175 );
						g.drawArc( x + ox, y + h - r - oy, r, r, 200, 120 );
						g.drawArc( x + ( w - r ) - oz, y + oy, r, r, 20, 120 );
						g.drawArc( x + r + oy, y + h / 2 + ox * 2, ( w * 20 ) / 55, h / 3, 27, 136 );
						g.drawArc( x + w / 2 - ( oy * 2 ), y - oz / 2, ( w * 20 ) / 55, h / 3, 207, 136 );
					} break;
					case ( 1 ): {
						g.fillArc( x, y, w / 2, h, 30, 195 );
						g.fillArc( x + w / 2, y, w / 2, h, 210, 195 );
						g.fillArc( x + ox, y + h - r - oy, r, r, 200, 120 );
						g.fillArc( x + ( w - r ) - oz, y + oy, r, r, 20, 120 );
						g.fillRoundRect( x + oz, y + oy * 2, w - 2 * oz, h - 4 * oy, r, r );
						Color c = g.getColor();
						g.setColor( Color.WHITE );
						g.fillOval( x + r + oy, y + h / 2 + ox * 2, ( w * 20 ) / 55, h / 3 );
						g.fillOval( x + w / 2 - ( oy * 2 ), y - oz / 2, ( w * 20 ) / 55, h / 3 );
						g.setColor( c );
						g.drawArc( x, y, w / 2, h, 40, 175 );
						g.drawArc( x + w / 2, y, w / 2, h, 220, 175 );
						g.drawArc( x + ox, y + h - r - oy, r, r, 200, 120 );
						g.drawArc( x + ( w - r ) - oz, y + oy, r, r, 20, 120 );
						g.drawArc( x + r + oy, y + h / 2 + ox * 2, ( w * 20 ) / 55, h / 3, 27, 136 );
						g.drawArc( x + w / 2 - ( oy * 2 ), y - oz / 2, ( w * 20 ) / 55, h / 3, 207, 136 );
					} break;
					case ( 2 ): {
						Color c = g.getColor();
						g.setColor( new Color( c.getRed() + ( ( 255 - c.getRed() ) * 3 ) / 5,
									c.getGreen() + ( ( 255 - c.getGreen() ) * 3 ) / 5,
									c.getBlue() + ( ( 255 - c.getBlue() ) * 3 ) / 5 ) );
						g.fillArc( x, y, w / 2, h, 30, 195 );
						g.fillArc( x + w / 2, y, w / 2, h, 210, 195 );
						g.fillArc( x + ox, y + h - r - oy, r, r, 200, 120 );
						g.fillArc( x + ( w - r ) - oz, y + oy, r, r, 20, 120 );
						g.fillRoundRect( x + oz, y + oy * 2, w - 2 * oz, h - 4 * oy, r, r );
						g.setColor( Color.WHITE );
						g.fillOval( x + r + oy, y + h / 2 + ox * 2, ( w * 20 ) / 55, h / 3 );
						g.fillOval( x + w / 2 - ( oy * 2 ), y - oz / 2, ( w * 20 ) / 55, h / 3 );
						g.setColor( c );
						g.drawArc( x, y, w / 2, h, 40, 175 );
						g.drawArc( x + w / 2, y, w / 2, h, 220, 175 );
						g.drawArc( x + ox, y + h - r - oy, r, r, 200, 120 );
						g.drawArc( x + ( w - r ) - oz, y + oy, r, r, 20, 120 );
						g.drawArc( x + r + oy, y + h / 2 + ox * 2, ( w * 20 ) / 55, h / 3, 27, 136 );
						g.drawArc( x + w / 2 - ( oy * 2 ), y - oz / 2, ( w * 20 ) / 55, h / 3, 207, 136 );
					} break;
				}
			} break;
		}
	}
	void drawCard( Graphics $g, int $no, int $x, int $y, int $size, int $type ) {
		int height = ( $size * 16 ) / 10;
		int symDist = ( height * 9 ) / 100;
		int symWidth = ( $size * 65 ) / 100;
		int symHeight = ( height * 2 ) / 11;
		int rounding = $size / 8;
		int left = ( $size - symWidth ) / 2;
		int top = ( height - ( symHeight * 3 + symDist * 2 ) ) / 2;
		Graphics2D g2D = (Graphics2D)$g;      
		$g.setColor( Color.WHITE );
		boolean selected = _selected.contains( $no );
		g2D.setStroke( new BasicStroke( selected ? 5F : 2F ) );
		$g.fillRoundRect( $x, $y, $size, height, rounding, rounding );
		if ( $no == _hovered ) {
			if ( selected )
				$g.setColor( Color.CYAN.brighter().brighter() );
			else
				$g.setColor( Color.GRAY );
		} else if ( selected )
			$g.setColor( Color.CYAN.darker() );
		else
			$g.setColor( Color.BLACK );
		$g.drawRoundRect( $x, $y, $size, height, rounding, rounding );
		g2D.setStroke( new BasicStroke(4F) );
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
				$g.setColor( Color.GREEN.darker() );
			} break;
			case ( 2 ): {
				$g.setColor( Color.MAGENTA );
			} break;
		}
		switch ( count ) {
			case ( 0 ): {
				drawSymbol( $g, $x + left, $y + top + symHeight + symDist, symWidth, symHeight, shape, fill );
			} break;
			case ( 1 ): {
				int top2 = ( height - ( symHeight * 2 + symDist ) ) / 2;
				drawSymbol( $g, $x + left, $y + top2, symWidth, symHeight, shape, fill );
				drawSymbol( $g, $x + left, $y + top2 + symHeight + symDist, symWidth, symHeight, shape, fill );
			} break;
			case ( 2 ): {
				drawSymbol( $g, $x + left, $y + top, symWidth, symHeight, shape, fill );
				drawSymbol( $g, $x + left, $y + top + symHeight + symDist, symWidth, symHeight, shape, fill );
				drawSymbol( $g, $x + left, $y + top + ( symHeight + symDist ) * 2, symWidth, symHeight, shape, fill );
			} break;
		}
	}
	void drawCard( Graphics g, int $no, int $type ) {
		/*
		 * Card relative dimensions:
		 * height : width
		 * 16 : 10
		 */
		java.awt.Dimension pd = getSize();
		int cardCol = $no % 4;
		int cardRow = $no / 4;
		int maxCardWidth = (int)( ( pd.getWidth() - 5 * D_MARGIN ) / 4 );
		int maxCardHeight = (int)( ( pd.getHeight() - 4 * D_MARGIN ) / 3 );
		int relCardMaxHeight = ( maxCardWidth * 16 ) / 10;
		if ( relCardMaxHeight > maxCardHeight )
			maxCardWidth = ( maxCardHeight * 10 ) / 16;
		else
			maxCardHeight = relCardMaxHeight;
		int leftMargin = (int)( ( pd.getWidth() - ( maxCardWidth * 4 + D_MARGIN * 3 ) ) / 2 );
		int topMargin = (int)( ( pd.getHeight() - ( maxCardHeight * 3 + D_MARGIN * 2 ) ) / 2 );
		drawCard( g, $no, leftMargin + cardCol * ( D_MARGIN + maxCardWidth ), D_MARGIN + cardRow * ( D_MARGIN + maxCardHeight ), maxCardWidth, $type );
	}
	boolean isHovered( int $no, Point $point ) {
		java.awt.Dimension pd = getSize();
		int cardCol = $no % 4;
		int cardRow = $no / 4;
		int maxCardWidth = (int)( ( pd.getWidth() - 5 * D_MARGIN ) / 4 );
		int maxCardHeight = (int)( ( pd.getHeight() - 4 * D_MARGIN ) / 3 );
		int relCardMaxHeight = ( maxCardWidth * 16 ) / 10;
		if ( relCardMaxHeight > maxCardHeight )
			maxCardWidth = ( maxCardHeight * 10 ) / 16;
		else
			maxCardHeight = relCardMaxHeight;
		int leftMargin = (int)( ( pd.getWidth() - ( maxCardWidth * 4 + D_MARGIN * 3 ) ) / 2 );
		int topMargin = (int)( ( pd.getHeight() - ( maxCardHeight * 3 + D_MARGIN * 2 ) ) / 2 );
		int x = (int)$point.getX();
		int y = (int)$point.getY();
		return ( ( x >= ( leftMargin + cardCol * ( D_MARGIN + maxCardWidth ) ) )
				&& ( x < ( ( leftMargin + cardCol * ( D_MARGIN + maxCardWidth ) ) + maxCardWidth ) )
				&& ( y >= ( D_MARGIN + cardRow * ( D_MARGIN + maxCardHeight ) ) )
				&& ( y < ( D_MARGIN + cardRow * ( D_MARGIN + maxCardHeight ) + maxCardHeight ) ) );
	}
	int getHovered( Point $point ) {
		if ( _cards != null ) {
			for ( int i = 0; i < _cards.length; ++ i ) {
				if ( _cards[i] >= 0 ) {
					if ( isHovered( i, $point ) )
						return ( i );
				}
			}
		}
		return ( -1 );
	}
	protected void paintComponent( Graphics g ) {
		super.paintComponent( g );
		if ( _cards != null ) {
			for ( int i = 0; i < _cards.length; ++ i ) {
				if ( _cards[i] >= 0 ) {
					drawCard( g, i, _cards[i] );
				}
			}
		}
	}
}

