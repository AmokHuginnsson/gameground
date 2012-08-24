import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Dimension;
import java.awt.Color;
import java.awt.BasicStroke;
import java.awt.event.MouseEvent;
import javax.swing.event.MouseInputListener;
import org.swixml.XTabbedPane;

public class TabbedPaneX extends XTabbedPane implements MouseInputListener {
	public static final long serialVersionUID = 17l;
	private static final int margin = 3;
	boolean _hovered = false;
	BasicStroke _strokeOut = new BasicStroke( 4 );
	BasicStroke _strokeIn = new BasicStroke( 2 );
	public TabbedPaneX() {
		super();
		addMouseMotionListener( this );
		addMouseListener( this );
	}
	private int closeButtonSize() {
		Dimension d = getSize();
		Dimension c = getSelectedComponent().getSize();
		return ( ( d.height - c.height ) - margin );
	}
	protected void paintComponent( Graphics g ) {
		super.paintComponent( g );
		int idx = getSelectedIndex();
		if ( idx != 0 ) {
			Dimension d = getSize();
			int h = closeButtonSize();
			Color c = getBackgroundAt( getSelectedIndex() );
			g.setColor( _hovered ? new Color( c.getRed() - 8, c.getGreen() - 8, c.getBlue() - 8 ) : c );
			g.fillRect( d.width - ( h + margin ), margin, h, h - margin - 2 );
			g.setColor( Color.gray );
			int[] frameX = new int[] { d.width - ( h + margin ), d.width - ( h + margin ), d.width - margin, d.width - margin };
			int[] frameY = new int[] { h - margin - 1, margin, margin, h - margin - 1 };
			g.drawPolyline( frameX, frameY, frameX.length );
			Graphics2D g2 = (Graphics2D)g;
			BasicStroke s = (BasicStroke)g2.getStroke();
			g2.setStroke( _strokeOut );
			g.setColor( Color.black );
			g.drawLine( d.width - h + margin * 2, margin * 3, d.width - margin * 4, h - margin * 3 );
			g.drawLine( d.width - h + margin * 2, h - margin * 3, d.width - margin * 4, margin * 3 );
			g2.setStroke( _strokeIn );
			g.setColor( Color.gray );
			g.drawLine( d.width - h + margin * 2, margin * 3, d.width - margin * 4, h - margin * 3 );
			g.drawLine( d.width - h + margin * 2, h - margin * 3, d.width - margin * 4, margin * 3 );
			g2.setStroke( s );
		}
	}
	public void mouseClicked( MouseEvent $event ) {
		int idx = getSelectedIndex();
		if ( idx != 0 ) {
			if ( _hovered ) {
				HGUIface f = (HGUIface)getSelectedComponent();
				if ( f != null )
					f.onClose();
			}
		}
	}
	public void mouseMoved( MouseEvent $event ) {
		int idx = getSelectedIndex();
		if ( idx != 0 ) {
			int x = $event.getX();
			int y = $event.getY();
			Dimension d = getSize();
			int h = closeButtonSize();
			boolean oldHovered = _hovered;
			if ( ( ( d.width - ( h + margin ) ) < x )
					&& ( ( d.width - margin ) > x )
					&& ( margin < y )
					&& ( ( h - margin - 1 ) > y ) )
				_hovered = true;
			else
				_hovered = false;
			if ( _hovered != oldHovered )
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
		int idx = getSelectedIndex();
		if ( idx != 0 ) {
			boolean oldHovered = _hovered;
			_hovered = false;
			if ( _hovered != oldHovered )
				repaint();
		}
	}
}

