import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.event.MouseEvent;
import java.awt.Color;

public class GomokuGoban extends Goban {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	Gomoku _logic = null;
	byte _winner;
	int _row;
	int _col;
	int _direction = -1;
//--------------------------------------------//
	public GomokuGoban() {
		super();
		_sgf = new SGF( SGF.GAME_TYPE.GOMOKU, "gameground-client" );
	}
	void setLogic( Gomoku $gomoku ) {
		super.setLogic( _logic = $gomoku );
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
	void reset() {
		_direction = -1;
	}
	void fiveInARow( byte stone, int row, int col, int direction ) {
		_winner = stone;
		_row = row;
		_col = col;
		_direction = direction;
	}
	void drawByLogic( Graphics g ) {
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.stone() ) ) {
			drawStone( _cursorX, _cursorY, _logic.stone(), true, g );
		} else if ( _direction >= 0 ) {
			int margin = _virtSize / ( _size + 4 );
			int inside = _virtSize - 2 * margin;
			int s = _size - 1;
			int o = D_MARGIN + margin - (int)( _diameter / 2 );
			int r = (int)( _diameter / 2 );
			int x = (int)( ( 2. * _diameter - _diameter * Math.sqrt( 2. ) ) / 4. );
			g.setColor( Color.GREEN );
			Graphics2D g2D = (Graphics2D)g;      
			g2D.setStroke( new BasicStroke(5F) );
			switch ( _direction ) {
				case ( 0 ): {
					g.drawLine( o + ( inside * _row ) / s, o + ( inside * _col ) / s + r, o + ( inside * _row ) / s, o + ( inside * ( _col + 4 ) ) / s + r );
					g.drawLine( o + ( inside * ( _row + 1 ) ) / s, o + ( inside * _col ) / s + r, o + ( inside * ( _row + 1 ) ) / s, o + ( inside * ( _col + 4 ) ) / s + r );
					g.drawArc( o + ( inside * _row ) / s, o + ( inside * _col ) / s, (int)_diameter, (int)_diameter, 0, 180 );
					g.drawArc( o + ( inside * _row ) / s, o + ( inside * ( _col + 4 ) ) / s, (int)_diameter, (int)_diameter, 0, -180 );
				} break;
				case ( 1 ): {
					g.drawLine( o + ( inside * _row ) / s + r, o + ( inside * _col ) / s, o + ( inside * ( _row + 4 ) ) / s + r, o + ( inside * _col ) / s );
					g.drawLine( o + ( inside * _row ) / s + r, o + ( inside * ( _col + 1 ) ) / s, o + ( inside * ( _row + 4 ) ) / s + r, o + ( inside * ( _col + 1 ) ) / s );
					g.drawArc( o + ( inside * _row ) / s, o + ( inside * _col ) / s, (int)_diameter, (int)_diameter, 90, 180 );
					g.drawArc( o + ( inside * ( _row + 4 ) ) / s, o + ( inside * _col ) / s, (int)_diameter, (int)_diameter, 90, -180 );
				} break;
				case ( 2 ): {
					g.drawLine( o + ( inside * _row ) / s + x, o + ( inside * ( _col + 1 ) ) / s - x, o + ( inside * ( _row + 4 ) ) / s + x, o + ( inside * ( _col + 5 ) ) / s - x );
					g.drawLine( o + ( inside * ( _row + 1 ) ) / s - x, o + ( inside * _col ) / s + x, o + ( inside * ( _row + 5 ) ) / s - x, o + ( inside * ( _col + 4 ) ) / s + x );
					g.drawArc( o + ( inside * _row ) / s, o + ( inside * _col ) / s, (int)_diameter, (int)_diameter, 45, 180 );
					g.drawArc( o + ( inside * ( _row + 4 ) ) / s, o + ( inside * ( _col + 4 ) ) / s, (int)_diameter, (int)_diameter, 45, -180 );
				} break;
				case ( 3 ): {
					g.drawLine( o + ( inside * _row ) / s + x, o + ( inside * _col ) / s + x, o + ( inside * ( _row + 4 ) ) / s + x, o + ( inside * ( _col - 4 ) ) / s + x );
					g.drawLine( o + ( inside * ( _row + 1 ) ) / s - x, o + ( inside * ( _col + 1 ) ) / s - x, o + ( inside * ( _row + 5 ) ) / s - x, o + ( inside * ( _col - 3 ) ) / s - x );
					g.drawArc( o + ( inside * _row ) / s, o + ( inside * _col ) / s, (int)_diameter, (int)_diameter, 135, 180 );
					g.drawArc( o + ( inside * ( _row + 4 ) ) / s, o + ( inside * ( _col - 4 ) ) / s, (int)_diameter, (int)_diameter, 135, -180 );
				} break;
			}
		}
	}
	void move( int $col, int $row, byte $stone ) {
		setStone( $col, $row, $stone );
	}
	void clear() {
	}
	void placeByLogic() {
	}
	boolean isAdmin() {
		return ( false );
	}
	boolean ongoingMatch() {
		return ( false );
	}
	boolean amIPlaying() {
		return ( false );
	}
	void toMove( byte $stone, int $moveNo ) {
		_logic.toMove( $stone, $moveNo );
	}
}

