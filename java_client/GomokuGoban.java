import java.awt.Graphics;
import java.awt.event.MouseEvent;

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
	boolean breakTheRules( int x, int y, byte stone ) {
		boolean invalid = false;
		return ( invalid );
	}
	void drawByLogic( Graphics g ) {
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.stone() ) )
			drawStone( _cursorX, _cursorY, _logic.stone(), true, g );
	}
}

