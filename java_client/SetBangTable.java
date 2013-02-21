import java.util.Arrays;
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
	HAbstractLogic _logic = null;
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
		Arrays.fill( _cards, -1 );
	}
	void setLogic( HAbstractLogic $logic ) {
		_logic = $logic;
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
			else if ( _selected.size() < 3 ) {
				_selected.add( _hovered );
				if ( _selected.size() == 3 ) {
					StringBuilder val = new StringBuilder();
					for ( Integer i : _selected ) {
						if ( val.length() > 0 )
							val.append( ',' );
						val.append( i );
					}
					_logic._client.println( SetBang.PROTOCOL.CMD + SetBang.PROTOCOL.SEP + _logic.id() + SetBang.PROTOCOL.SEP
							+ SetBang.PROTOCOL.SET + SetBang.PROTOCOL.SEP + val );
				}
			}
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
				int px[] = new int[] { -149, -229, -309, -391, -473, -556, -638, -720, -794, -859, -914, -956, -986, -1000, -998, -963, -894, -794, -668, -520, -354, -175, 0, 72, 149, 229, 309, 391, 473, 556, 638, 720, 794, 859, 914, 956, 986, 1000, 998, 963, 894, 794, 668, 520, 354, 175, 0 };
				int py[] = new int[] { 317, 311, 316, 333, 360, 399, 448, 492, 512, 508, 482, 435, 367, 280, 176, 11, -130, -246, -333, -390, -415, -405, -361, -334, -317, -311, -316, -333, -360, -399, -448, -492, -512, -508, -482, -435, -367, -280, -176, -11, 130, 246, 333, 390, 415, 405, 361 };
				x += w / 2;
				y += h / 2;
				for ( int i = 0; i < px.length; ++ i ) {
					px[i] = x + ( px[i] * w ) / 2000;
					py[i] = y + ( py[i] * w ) / 2000;
				}
				switch ( $fill ) {
					case ( 0 ): {
						g.drawPolygon( px, py, px.length );
					} break;
					case ( 1 ): {
						g.drawPolygon( px, py, px.length );
						g.fillPolygon( px, py, px.length );
					} break;
					case ( 2 ): {
						Color c = g.getColor();
						g.setColor( new Color( c.getRed() + ( ( 255 - c.getRed() ) * 3 ) / 5,
									c.getGreen() + ( ( 255 - c.getGreen() ) * 3 ) / 5,
									c.getBlue() + ( ( 255 - c.getBlue() ) * 3 ) / 5 ) );
						g.fillPolygon( px, py, px.length );
						g.setColor( c );
						g.drawPolygon( px, py, px.length );
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
		drawCard( g, $no, leftMargin + cardCol * ( D_MARGIN + maxCardWidth ), topMargin + cardRow * ( D_MARGIN + maxCardHeight ), maxCardWidth, $type );
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

