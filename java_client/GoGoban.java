import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.util.Arrays;

public class GoGoban extends Goban {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	Go _logic = null;
	char[] _koStones;
	char[] _oldStones;
//--------------------------------------------//
	public GoGoban() {
		super();
	}
	void setGui( Go $go ) {
		_logic = $go;
	}
	public void mouseClicked( MouseEvent $event ) {
		if ( _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _stone ) ) {
			_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PUTSTONE + Go.PROTOCOL.SEPP + _cursorX + Go.PROTOCOL.SEPP + _cursorY );
			_logic.waitToMove();
		} else if ( validCoords( _cursorX, _cursorY ) && ( _logic.toMove() == Go.STONE.MARK ) ) {
			char stone = getStone( _cursorX, _cursorY );
			if ( ( _logic.stone() == stone ) || ( _logic.stoneDead() == stone ) ) {
				System.out.println( "dead mark" );
				_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP
						+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
						+ Go.PROTOCOL.DEAD + stoneGroup( _cursorX, _cursorY ) );
			}
		}
	}
	private String stoneGroup( int x, int y ) {
		char stone = _logic.stone();
		char stoneDead = _logic.stoneDead();
		String group = "";
		setStone( x, y, Go.STONE.NONE );
		char testStone;
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
	public void setStones( byte[] $stones, boolean $commit ) {
		if ( _stones != null )
			_oldStones = Arrays.copyOf( _stones, _size * _size );
		else
			_stones = new char[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL]; 
		for ( int i = 0; i < $stones.length; ++ i )
			_stones[ i ] = (char)$stones[ i ];
		if ( $commit )
			_koStones = Arrays.copyOf( _stones, _size * _size );
	}
	boolean haveLiberties( int a_iCol, int a_iRow, char stone ) {
		if ( ( a_iCol < 0 ) || ( a_iCol > ( _size - 1 ) )
				|| ( a_iRow < 0 ) || ( a_iRow > ( _size - 1 ) ) )
			return ( false );
		if ( getStone( a_iCol, a_iRow ) == Go.STONE.NONE )
			return ( true );
		if ( getStone( a_iCol, a_iRow ) == stone ) {
			setStone( a_iCol, a_iRow, Character.toUpperCase( stone ) );	
			return ( haveLiberties( a_iCol, a_iRow - 1, stone )
					|| haveLiberties( a_iCol, a_iRow + 1, stone )
					|| haveLiberties( a_iCol - 1, a_iRow, stone )
					|| haveLiberties( a_iCol + 1, a_iRow, stone ) );
		}
		return ( false );
	}
	boolean haveKilled( int x, int y, char stone ) {
		boolean killed = false;
		char foeStone = opponent( stone );
		setStone( x, y, stone );
		if ( ( x > 0 ) && ( getStone( x - 1, y ) == foeStone ) && ( ! haveLiberties( x - 1, y, foeStone ) ) )
			clearGoban( killed = true );
		else
			clearGoban( false );
		if ( ( x < ( _size - 1 ) ) && ( getStone( x + 1, y ) == foeStone ) && ( ! haveLiberties( x + 1, y, foeStone ) ) )
			clearGoban( killed = true );
		else
			clearGoban( false );
		if ( ( y > 0 ) && ( getStone( x, y - 1 ) == foeStone ) && ( ! haveLiberties( x, y - 1, foeStone ) ) )
			clearGoban( killed = true );
		else
			clearGoban( false );
		if ( ( y < ( _size - 1 ) ) && ( getStone( x, y + 1 ) == foeStone ) && ( ! haveLiberties( x, y + 1, foeStone ) ) )
			clearGoban( killed = true );
		else
			clearGoban( false );
		setStone( x, y, Go.STONE.NONE );
		return ( killed );
	}
	boolean isKo() {
		boolean same = false;
		for ( int i = 0; ( i < ( _size * _size ) )
				&& ( same = ( _koStones[ i ] == _oldStones[ i ] ) ); ++ i )
			;
		return ( same );
	}
	boolean isSuicide( int x, int y, char stone ) {
		boolean suicide = false;
		setStone( x, y, stone );
		if ( ! haveLiberties( x, y, stone ) )
			suicide = true;
		setStone( x, y, Go.STONE.NONE );
		return ( suicide );
	}	
	boolean breakTheRules( int x, int y, char stone ) {
		boolean invalid = ! validCoords( x, y );
		if ( ! invalid ) {
			_koStones = Arrays.copyOf( _stones, _size * _size );
			if ( getStone( x, y ) != Go.STONE.NONE )
				invalid = true;
			else if ( ! haveKilled( x, y, stone ) ) {
				if ( isSuicide( x, y, stone ) )
					invalid = true;
			}	else {
				setStone( x, y, stone );
				if ( isKo() )
					invalid = true;
			}
		}
		return ( invalid );
	}
	void drawByLogic( Graphics g ) {
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _stone ) )
			drawStone( _cursorX, _cursorY, _logic.stone(), true, g );
	}
}

