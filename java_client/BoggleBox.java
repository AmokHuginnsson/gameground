import javax.swing.JPanel;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Color;

public class BoggleBox extends JPanel {
	public static final long serialVersionUID = 17l;
	protected void paintBorder( Graphics g ) {
		java.awt.Dimension pd = getSize();
		g.setColor( Color.ORANGE.darker() );
		g.drawOval( 4, 4, (int)pd.getWidth() - 8, (int)pd.getHeight() - 8 );
		g.drawOval( 3, 3, (int)pd.getWidth() - 6, (int)pd.getHeight() - 6 );
	}
}

