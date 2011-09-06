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
		if ( _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.stone() ) ) {
			Sound.play( "stone" );
			_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP + _logic.id() + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PUTSTONE + Go.PROTOCOL.SEPP + _cursorX + Go.PROTOCOL.SEPP + _cursorY );
			_logic.waitToMove();
		}
	}
	boolean breakTheRules( int x, int y, byte stone ) {
		boolean invalid = ! validCoords( x, y );
		if ( ! invalid ) {
			if ( getStone( x, y ) != Gomoku.STONE.NONE )
				invalid = true;
		}
		return ( invalid );
	}
	void drawByLogic( Graphics g ) {
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.stone() ) ) {
			drawStone( _cursorX, _cursorY, _logic.stone(), true, g );
		}
	}
}

