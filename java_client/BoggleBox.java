import javax.swing.JPanel;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Color;

public class BoggleBox extends JPanel {
	public static final long serialVersionUID = 17l;
	protected void paintBorder( Graphics g ) {
		super.paintBorder( g );
		java.awt.Dimension pd = getSize();
		g.setColor( getParent().getBackground() );
		g.fillRect( 0, 0, (int)pd.getWidth(), (int)pd.getHeight() );
		g.setColor( Color.ORANGE );
		g.fillOval( 0, 0, (int)pd.getWidth() - 1, (int)pd.getHeight() - 1 );
	}
}

