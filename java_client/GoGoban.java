import java.util.Arrays;
import java.awt.Graphics;
import java.awt.event.MouseEvent;

public class GoGoban extends Goban {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	Go _logic = null;
	byte[] _koStones = new byte[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL];
	byte[] _oldStones = new byte[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL];
//--------------------------------------------//
	public GoGoban() {
		super();
		_sgf = new SGF( SGF.GAME_TYPE.GO, "gameground-client" );
	}
	void setGui( Go $go ) {
		_logic = $go;
	}
	public void mouseClicked( MouseEvent $event ) {
		if ( _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.stone() ) ) {
			Sound.play( "stone" );
			_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP + _logic.id() + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PUTSTONE + Go.PROTOCOL.SEPP + _cursorX + Go.PROTOCOL.SEPP + _cursorY );
			_logic.waitToMove();
		} else if ( validCoords( _cursorX, _cursorY ) && ( _logic.toMove() == Go.STONE.MARK ) ) {
			byte stone = getStone( _cursorX, _cursorY );
			if ( ( _logic.stone() == stone ) || ( _logic.stoneDead() == stone ) ) {
				System.out.println( "dead mark" );
				_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP + _logic.id() + Go.PROTOCOL.SEP
						+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
						+ Go.PROTOCOL.DEAD + stoneGroup( _cursorX, _cursorY ) );
			}
		}
	}
	private String stoneGroup( int x, int y ) {
		byte stone = _logic.stone();
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
		System.arraycopy( _stones, 0, _oldStones, 0, _size * _size );
		super.updateSGF( $sgfData );
	}
	boolean haveLiberties( int a_iCol, int a_iRow, byte stone ) {
		if ( ( a_iCol < 0 ) || ( a_iCol > ( _size - 1 ) )
				|| ( a_iRow < 0 ) || ( a_iRow > ( _size - 1 ) ) )
			return ( false );
		if ( getStone( a_iCol, a_iRow ) == Go.STONE.NONE )
			return ( true );
		if ( getStone( a_iCol, a_iRow ) == stone ) {
			setStone( a_iCol, a_iRow, (byte)Character.toUpperCase( stone ) );	
			return ( haveLiberties( a_iCol, a_iRow - 1, stone )
					|| haveLiberties( a_iCol, a_iRow + 1, stone )
					|| haveLiberties( a_iCol - 1, a_iRow, stone )
					|| haveLiberties( a_iCol + 1, a_iRow, stone ) );
		}
		return ( false );
	}
	void removeDead() {
		for ( int i = 0; i < ( _size * _size ); ++ i ) {
			if ( _stones[i] != STONE.NONE ) {
				if ( Character.isUpperCase( _stones[ i ] ) )
					_stones[ i ] = STONE.NONE;
			}
		}
	}
	void resetAlive() {
		for ( int i = 0; i < ( _size * _size ); ++ i ) {
			if ( _stones[i] != STONE.NONE ) {
				if ( Character.isUpperCase( _stones[ i ] ) )
					_stones[ i ] = (byte)Character.toLowerCase( _stones[ i ] );
			}
		}
	}
	boolean haveKilled( int x, int y, byte stone ) {
		boolean killed = false;
		byte foeStone = opponent( stone );
		if ( ( x > 0 ) && ( getStone( x - 1, y ) == foeStone ) && ( ! haveLiberties( x - 1, y, foeStone ) ) ) {
			killed = true;
			removeDead();
		} else {
			resetAlive();
		}
		if ( ( x < ( _size - 1 ) ) && ( getStone( x + 1, y ) == foeStone ) && ( ! haveLiberties( x + 1, y, foeStone ) ) ) {
			killed = true;
			removeDead();
		} else {
			resetAlive();
		}
		if ( ( y > 0 ) && ( getStone( x, y - 1 ) == foeStone ) && ( ! haveLiberties( x, y - 1, foeStone ) ) ) {
			killed = true;
			removeDead();
		} else {
			resetAlive();
		}
		if ( ( y < ( _size - 1 ) ) && ( getStone( x, y + 1 ) == foeStone ) && ( ! haveLiberties( x, y + 1, foeStone ) ) ) {
			killed = true;
			removeDead();
		} else {
			resetAlive();
		}
		return ( killed );
	}
	boolean isKo() {
		boolean same = false;
		for ( int i = 0; ( i < ( _size * _size ) )
				&& ( same = ( _stones[ i ] == _oldStones[ i ] ) ); ++ i )
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
				System.arraycopy( _stones, 0, _koStones, 0, _size * _size );
				setStone( x, y, stone );
				if ( ! haveKilled( x, y, stone ) ) {
					if ( isSuicide( x, y, stone ) )
						invalid = true;
				}	else {
					if ( isKo() )
						invalid = true;
				}
				System.arraycopy( _koStones, 0, _stones, 0, _size * _size );
			}
		}
		return ( invalid );
	}
	void drawByLogic( Graphics g ) {
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _logic.stone() ) )
			drawStone( _cursorX, _cursorY, _logic.stone(), true, g );
	}
	void move( int $col, int $row, byte $stone ) {
		setStone( $col, $row, $stone );
		haveKilled( $col, $row, $stone );
		resetAlive();
	}
/*
	void dump( byte[] $stones ) {
		int i = 0;
		int size = Math.min( (int)Math.sqrt( $stones.length ), _size );
		System.out.print( "+" );
		for ( int c = 0; c < size; ++ c )
			System.out.print( "-" );
		System.out.println( "+" );
		for ( int r = 0; r < size; ++ r ) {
			System.out.print( "|" );
			for ( int c = 0; c < size; ++ c, ++ i ) {
				System.out.print( (char)$stones[i] );
			}
			System.out.println( "|" );
		}
		System.out.print( "+" );
		for ( int c = 0; c < size; ++ c )
			System.out.print( "-" );
		System.out.println( "+" );
	}
*/
}

