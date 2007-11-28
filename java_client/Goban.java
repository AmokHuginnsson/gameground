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

public class Goban extends JPanel implements MouseInputListener {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	public static final int D_MARGIN = 20;
	int _size = Go.GOBAN_SIZE.NORMAL;
	double _diameter = -1;
	int _cursorX = -1;
	int _cursorY = -1;
	int _virtSize = 0;
	GoImages _images = null;
	Go _logic = null;
	char _stone;
	char[] _stones = null;
	char[] _koStones;
	char[] _oldStones;
//--------------------------------------------//
	public Goban() {
		addMouseMotionListener( this );
		addMouseListener( this );
	}
	public void mouseMoved( MouseEvent $event ) {
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		int cursorX = (int)( ( $event.getX() + ( _diameter / 2 ) - ( D_MARGIN + margin ) ) * ( _size - 1 ) ) / inside;
		int cursorY = (int)( ( $event.getY() + ( _diameter / 2 ) - ( D_MARGIN + margin ) ) * ( _size - 1 ) ) / inside;
		if ( ( _cursorX != cursorX ) || ( _cursorY != cursorY ) ) {
			_cursorX = cursorX;
			_cursorY = cursorY;
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
	public void mouseClicked( MouseEvent $event ) {
		if ( _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _stone ) )
			_logic._client.println( Go.PROTOCOL.CMD + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PLAY + Go.PROTOCOL.SEP
					+ Go.PROTOCOL.PUTSTONE + Go.PROTOCOL.SEPP + _cursorX + Go.PROTOCOL.SEPP + _cursorY );
	}
	public void mouseExited( MouseEvent $event ) {
		_cursorX = -1;
		_cursorY = -1;
		repaint();
	}
	void setGui( Go $gui ) {
		_logic = $gui;
	}
	public void setStone( char $stone ) {
		_stone = $stone;
	}
	public void setStones( byte[] $stones ) {
		if ( _stones != null )
			_oldStones = Arrays.copyOf( _stones, _size * _size );
		else
			_stones = new char[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL]; 
		for ( int i = 0; i < $stones.length; ++ i )
			_stones[ i ] = (char)$stones[ i ];
	}
	public void setImages( GoImages $images ) {
		_images = $images;
	}
	void setSize( int $size ) {
		_size = $size;
		_virtSize = 0;
		getPreferredSize();
		repaint();
	}
	public Dimension getPreferredSize() {
		java.awt.Dimension pd = getParent().getSize();
		int size = pd.getWidth() < pd.getHeight() ? (int)pd.getWidth() : (int)pd.getHeight();;
		if ( ( _virtSize != size ) && ( _images != null ) && ( size > 0 ) ) {
			_virtSize = size - 2 * D_MARGIN;
			int margin = _virtSize / ( _size + 4 );
			int inside = _virtSize - 2 * margin;
			_diameter = inside / ( _size - 1 );
			_images.regenerate( _virtSize, (int)_diameter );
		}
		return ( new Dimension( size, size ) );
	}
	protected void paintComponent( Graphics g ) {
		super.paintComponent( g );
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		g.drawImage( _images._background, D_MARGIN, D_MARGIN, _virtSize, _virtSize, Color.black, this );
		g.setColor( Color.black );
		for ( int i = 0; i < _size; ++ i ) {
			g.drawLine( D_MARGIN + margin, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ),
					D_MARGIN + _virtSize - margin, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) );
			g.drawLine( D_MARGIN + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + _virtSize - margin,
					D_MARGIN + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + margin );
			g.drawLine( D_MARGIN + margin, D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ),
					D_MARGIN + _virtSize - margin, D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ) );
			g.drawLine( D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + _virtSize - margin,
					D_MARGIN + 1 + margin + ( inside * i ) / ( _size - 1 ), D_MARGIN + margin );
		}
		int hoshi = 9;
		g.fillOval( D_MARGIN + _virtSize / 2 - hoshi / 2, D_MARGIN + _virtSize / 2 - hoshi / 2, hoshi, hoshi );
		int hoshiLine = 3;
		if ( _size == 9 )
			hoshiLine = 2;
		int offset = ( inside * hoshiLine ) / ( _size - 1 ) - hoshi / 2;
		int noffset = ( inside * ( ( _size - 1 ) - hoshiLine ) ) / ( _size - 1 ) - hoshi / 2;
		g.fillOval( D_MARGIN + margin + offset, D_MARGIN + _virtSize / 2 - hoshi / 2, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + noffset, D_MARGIN + _virtSize / 2 - hoshi / 2, hoshi, hoshi );
		g.fillOval( D_MARGIN + _virtSize / 2 - hoshi / 2, D_MARGIN + margin + offset, hoshi, hoshi );
		g.fillOval( D_MARGIN + _virtSize / 2 - hoshi / 2, D_MARGIN + margin + noffset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + offset, D_MARGIN + margin + offset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + noffset, D_MARGIN + margin + offset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + offset, D_MARGIN + margin + noffset, hoshi, hoshi );
		g.fillOval( D_MARGIN + margin + noffset, D_MARGIN + margin + noffset, hoshi, hoshi );
		char[] label = new char[2];
		for ( int i = 0; i < _size; ++ i ) {
			label[ 0 ] = (char)( 'A' + i );
			g.drawChars( label, 0, 1, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) - 4, D_MARGIN - 3 );
			g.drawChars( label, 0, 1, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) - 4, D_MARGIN + _virtSize + 15 );
			label[ 1 ] = (char)( '0' + ( ( i + 1 ) % 10 ) );
			if ( i > 8 )
				label[ 0 ] = '1';
			else
				label[ 0 ] = Go.STONE.NONE;
			g.drawChars( label, 0, 2, D_MARGIN - 18, D_MARGIN + margin + ( inside * ( _size - 1 - i ) ) / ( _size - 1 ) + 5 );
			g.drawChars( label, 0, 2, D_MARGIN + _virtSize + 2, D_MARGIN + margin + ( inside * ( _size - 1 - i ) ) / ( _size - 1 ) + 5 );
		}
		drawStones( g );
		if ( ( _logic != null ) && _logic.isMyMove() && ! breakTheRules( _cursorX, _cursorY, _stone ) )
			drawStone( _cursorX, _cursorY, _logic.stone(), true, g );
	}
	private void drawStone( int $xx, int $yy, int $color, boolean $alpha, Graphics $gc ) {
		Image img = null;
		if ( $color == Go.STONE.WHITE ) {
			if ( $alpha )
				img = _images._whitesAlpha[ ( $yy * _size + $xx ) % GoImages.D_WHITE_LOOKS ];
			else
				img = _images._whites[ ( $yy * _size + $xx ) % GoImages.D_WHITE_LOOKS ];
		} else {
			if ( $alpha )
				img = _images._blackAlpha;
			else
				img = _images._black;
		}
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		if ( ! $alpha )
			$gc.drawImage( _images._shadow, D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( ( _diameter / 2 ) + ( _diameter / 10 ) ),
					D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) + (int)( _diameter / 10 ) - (int)( _diameter / 2 ), this );
		$gc.drawImage( img, D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( _diameter / 2 ),
				D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) - (int)( _diameter / 2 ), this );
	}
	private void drawStones( Graphics g ) {
		if ( _stones != null ) {
			int size = _size * _size;
			size = ( size < _stones.length ? size : _stones.length );
			for ( int i = 0; i < size; ++ i ) {
				char stone = _stones[ i ];
				if ( stone != Go.STONE.NONE )
					drawStone( i % _size, i / _size, stone, false, g );
			}
		}
	}
	void setStone( int $col, int $row, char $stone ) {
		_koStones[ $row * _size + $col ] = $stone;
	}
	char getStone( int $col, int $row ) {
		return ( _koStones[ $row * _size + $col ] );
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
	void clearGoban( boolean removeDead ) {
		for ( int i = 0; i < _size; i++ ) {
			for ( int j = 0; j < _size; j++ ) {
				if ( getStone( i, j ) != Go.STONE.NONE ) {
					if ( removeDead && Character.isUpperCase( getStone( i, j ) ) )
						setStone( i, j, Go.STONE.NONE );
					else
						setStone( i, j, Character.toLowerCase( getStone( i, j ) ) );
				}
			}
		}
	}
	char oponent( char stone ) {
		return ( stone == Go.STONE.WHITE ? Go.STONE.BLACK : Go.STONE.WHITE );
	}
	boolean haveKilled( int x, int y, char stone ) {
		boolean killed = false;
		char foeStone = oponent( stone );
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
		boolean invalid = false;
		if ( ( x < 0 ) || ( x > ( _size - 1 ) )
				|| ( y < 0 ) || ( y > ( _size - 1 ) ) )
			invalid = true;
		else {
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
}

