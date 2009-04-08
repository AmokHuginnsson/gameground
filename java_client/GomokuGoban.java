import java.io.InputStreamReader;
import java.io.InputStream;
import java.net.URL;
import java.awt.Image;
import java.awt.Graphics;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.ImageIcon;
import javax.swing.event.MouseInputListener;
import java.util.Arrays;

public class GomokuGoban extends Goban {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	Gomoku _logic = null;
//--------------------------------------------//
	public GomokuGoban() {
		super();
	}
	void setGui( Gomoku $gomoku ) {
		_logic = $gomoku;
	}
	public void mouseClicked( MouseEvent $event ) {
	}
	boolean breakTheRules( int x, int y, char stone ) {
		boolean invalid = false;
		return ( invalid );
	}
	void drawByLogic( Graphics g ) {
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _stone ) )
			drawStone( _cursorX, _cursorY, _logic.stone(), true, g );
	}
}

