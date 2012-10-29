import java.util.Arrays;
import java.util.ArrayList;
import java.awt.Graphics;
import java.awt.event.MouseEvent;

public class GoGoban extends Goban {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	Go _logic = null;
	byte[] _tmpStones = new byte[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL];
	byte[] _koStones = new byte[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL];
//--------------------------------------------//
	public GoGoban() {
		super();
		_sgf = new SGF( SGF.GAME_TYPE.GO, "gameground-client" );
	}
	void clear() {
		_logic.clearScore();
	}
	void setLogic( Go $go ) {
		super.setLogic( _logic = $go );
	}
	public void mouseClicked( MouseEvent $event ) {
		if ( _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.playerColor() ) ) {
			Sound.play( "stone" );
			_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP + _logic.id() + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PUTSTONE + Go.PROTOCOL.SEPP + _cursorX + Go.PROTOCOL.SEPP + _cursorY );
			_logic.waitToMove();
		} else if ( validCoords( _cursorX, _cursorY ) && ( _logic.toMove() == Go.STONE.MARK ) ) {
			byte stone = getStone( _cursorX, _cursorY );
			if ( ( _logic.playerColor() == stone ) || ( _logic.stoneDead() == stone ) ) {
				System.out.println( "dead mark" );
				_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP + _logic.id() + Go.PROTOCOL.SEP
						+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
						+ Go.PROTOCOL.DEAD + stoneGroup( _cursorX, _cursorY ) );
			}
		}
	}
	private String stoneGroup( int x, int y ) {
		byte stone = _logic.playerColor();
		byte stoneDead = _logic.stoneDead();
		String group = "";
		setStone( x, y, Go.STONE.NONE );
		byte testStone;
		testStone = getStone( x - 1, y );
		if ( ( testStone == stone ) || ( testStone == stoneDead ) )
			group += stoneGroup( x - 1, y );
		testStone = getStone( x + 1, y );
		if ( ( testStone == stone ) || ( testStone == stoneDead ) )
			group += stoneGroup( x + 1, y );
		testStone = getStone( x, y - 1 );
		if ( ( testStone == stone ) || ( testStone == stoneDead ) )
			group += stoneGroup( x, y - 1 );
		testStone = getStone( x, y + 1 );
		if ( ( testStone == stone ) || ( testStone == stoneDead ) )
			group += stoneGroup( x, y + 1 );
		group += ( "," + x + Go.PROTOCOL.SEPP + y );
		setStone( x, y, stone );
		return ( group );
	}
	public void updateSGF( String $sgfData ) {
		super.updateSGF( $sgfData );
	}
	boolean haveLiberties( int $col, int $row, byte stone ) {
		if ( ( $col < 0 ) || ( $col > ( _size - 1 ) )
				|| ( $row < 0 ) || ( $row > ( _size - 1 ) ) )
			return ( false );
		if ( getStone( $col, $row ) == Go.STONE.NONE )
			return ( true );
		if ( getStone( $col, $row ) == stone ) {
			setStone( $col, $row, (byte)Character.toUpperCase( stone ) );	
			return ( haveLiberties( $col, $row - 1, stone )
					|| haveLiberties( $col, $row + 1, stone )
					|| haveLiberties( $col - 1, $row, stone )
					|| haveLiberties( $col + 1, $row, stone ) );
		}
		return ( false );
	}
	int removeDead() {
		int killed = 0;
		for ( int i = 0; i < ( _size * _size ); ++ i ) {
			if ( _stones[i] != STONE.NONE ) {
				if ( Character.isUpperCase( _stones[ i ] ) ) {
					_stones[ i ] = STONE.NONE;
					++ killed;
				}
			}
		}
		return ( killed );
	}
	void resetAlive() {
		for ( int i = 0; i < ( _size * _size ); ++ i ) {
			if ( _stones[i] != STONE.NONE ) {
				if ( Character.isUpperCase( _stones[ i ] ) )
					_stones[ i ] = (byte)Character.toLowerCase( _stones[ i ] );
			}
		}
	}
	int haveKilled( int x, int y, byte stone ) {
		int killed = 0;
		byte foeStone = opponent( stone );
		if ( ( x > 0 ) && ( getStone( x - 1, y ) == foeStone ) && ( ! haveLiberties( x - 1, y, foeStone ) ) ) {
			killed += removeDead();
		} else {
			resetAlive();
		}
		if ( ( x < ( _size - 1 ) ) && ( getStone( x + 1, y ) == foeStone ) && ( ! haveLiberties( x + 1, y, foeStone ) ) ) {
			killed += removeDead();
		} else {
			resetAlive();
		}
		if ( ( y > 0 ) && ( getStone( x, y - 1 ) == foeStone ) && ( ! haveLiberties( x, y - 1, foeStone ) ) ) {
			killed += removeDead();
		} else {
			resetAlive();
		}
		if ( ( y < ( _size - 1 ) ) && ( getStone( x, y + 1 ) == foeStone ) && ( ! haveLiberties( x, y + 1, foeStone ) ) ) {
			killed += removeDead();
		} else {
			resetAlive();
		}
		return ( killed );
	}
	boolean isKo() {
		boolean same = false;
		for ( int i = 0; ( i < ( _size * _size ) )
				&& ( same = ( _stones[ i ] == _koStones[ i ] ) ); ++ i )
			;
		return ( same );
	}
	boolean isSuicide( int x, int y, byte stone ) {
		boolean suicide = false;
		if ( ! haveLiberties( x, y, stone ) )
			suicide = true;
		return ( suicide );
	}	
	boolean breakTheRules( int x, int y, byte stone ) {
		boolean invalid = ! validCoords( x, y );
		if ( ! invalid ) {
			if ( getStone( x, y ) != Go.STONE.NONE )
				invalid = true;
			else {
				System.arraycopy( _stones, 0, _tmpStones, 0, _size * _size );
				setStone( x, y, stone );
				if ( haveKilled( x, y, stone ) == 0 ) {
					if ( isSuicide( x, y, stone ) ) {
						invalid = true;
					}
				}	else if ( isKo() )
					invalid = true;
				System.arraycopy( _tmpStones, 0, _stones, 0, _size * _size );
			}
		}
		return ( invalid );
	}
	void drawByLogic( Graphics g ) {
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.playerColor() ) )
			drawStone( _cursorX, _cursorY, _logic.playerColor(), true, g );
	}
	void move( int $col, int $row, byte $stone ) {
		if ( breakTheRules( $col, $row, $stone ) )
			throw new RuntimeException( "Illegal move!" );
		System.arraycopy( _stones, 0, _koStones, 0, _size * _size );
		setStone( $col, $row, $stone );
		_logic.captures( $stone, haveKilled( $col, $row, $stone ) );
		resetAlive();
	}
	void placeByLogic() {
		int blackTeritory = 0;
		int whiteTeritory = 0;
		int blackDead = 0;
		int whiteDead = 0;
		for ( int i = 0; i < ( _size * _size ); ++ i ) {
			switch ( _stones[i] ) {
				case ( Go.STONE.DEAD_BLACK ): ++ blackDead; break;
				case ( Go.STONE.DEAD_WHITE ): ++ whiteDead; break;
				case ( Go.STONE.TERITORY_BLACK ): ++ blackTeritory; break;
				case ( Go.STONE.TERITORY_WHITE ): ++ whiteTeritory; break;
			}
		}
		SGF.Setup setup = _lastMove.value().setup();
		if ( setup != null ) {
			ArrayList<SGF.Coord> c = setup.get( SGF.Position.BLACK_TERITORY );
			if ( c != null )
				blackTeritory += c.size();
			c = setup.get( SGF.Position.WHITE_TERITORY );
			if ( c != null )
				whiteTeritory += c.size();
		}
		_logic.score( STONE.BLACK, blackTeritory + whiteDead );
		_logic.score( STONE.WHITE, whiteTeritory + blackDead );
	}
}

