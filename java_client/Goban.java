import java.util.Arrays;
import java.io.BufferedReader;
import java.io.StringReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.awt.Image;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.Color;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;

public abstract class Goban extends JPanel implements MouseInputListener {
//--------------------------------------------//

	public static class STONE {
		public static final byte BLACK = 'b';
		public static final byte WHITE = 'w';
		public static final byte NONE	= ' ';
		public static final byte INVALID = 'N';
		public static final byte WAIT = 'Z';
		public static final String NONE_NAME = "None";
		public static final String BLACK_NAME = "Black";
		public static final String WHITE_NAME = "White";
	}
	public static final long serialVersionUID = 7l;
	public static final int D_MARGIN = 20;
	int _size = Go.GOBAN_SIZE.NORMAL;
	double _diameter = -1;
	int _cursorX = -1;
	int _cursorY = -1;
	int _virtSize = 0;
	GoImages _images = null;
	SGF _sgf = null;
	SGF.Game.Move _lastMove = null;
	byte[] _stones = new byte[Go.GOBAN_SIZE.NORMAL * Go.GOBAN_SIZE.NORMAL];
//--------------------------------------------//
	public Goban() {
		Arrays.fill( _stones, STONE.NONE );
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
	public void mouseExited( MouseEvent $event ) {
		_cursorX = -1;
		_cursorY = -1;
		repaint();
	}
	public void setStones( byte[] $stones ) {
		// System.arraycopy( $stones, 0, _stones, 0, $stones.length );
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
	void updateSGF( String $sgfData ) {
		BufferedReader br = new BufferedReader( new StringReader( $sgfData ) );
		updateSGF( br );
	}
	void updateSGF( BufferedReader $reader ) {
		_sgf.clear();
		try {
			_sgf.load( $reader );
			_lastMove = _sgf.lastMove();
			Arrays.fill( _stones, STONE.NONE );
			for ( SGF.Game.Move m : _sgf._game._blackPreset ) {
				move( m.col(), m.row(), STONE.BLACK );
			}
			for ( SGF.Game.Move m : _sgf._game._whitePreset ) {
				move( m.col(), m.row(), STONE.WHITE );
			}
			byte stone = _sgf._game._firstToMove == SGF.Game.Player.BLACK ? STONE.BLACK : STONE.WHITE;
			for ( SGF.Game.Move m : _sgf._game._tree ) {
				move( m.col(), m.row(), stone );
				stone = opponent( stone );
			}
		} catch ( SGFException se ) {
			Con.err( "SGFException: " + se.getMessage() );
			System.exit( 1 );
		}
		repaint();
	}
	public Dimension getPreferredSize() {
		java.awt.Dimension pd = getParent().getSize();
		int size = pd.getWidth() < pd.getHeight() ? (int)pd.getWidth() : (int)pd.getHeight();
		if ( ( _virtSize != size ) && ( _images != null ) && ( size > 0 ) ) {
			_virtSize = size - 2 * D_MARGIN;
			int margin = _virtSize / ( _size + 4 );
			int inside = _virtSize - 2 * margin;
			_diameter = inside / ( _size - 1 );
			_images.regenerate( _virtSize, (int)_diameter );
		}
		return ( new Dimension( size, size ) );
	}
	void drawGoban( Graphics g ) {
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
			label[ 0 ] = (char)( 'A' + ( i < ( 'i' - 'a' ) ? i : i + 1 ) );
			g.drawChars( label, 0, 1, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) - 4, D_MARGIN - 3 );
			g.drawChars( label, 0, 1, D_MARGIN + margin + ( inside * i ) / ( _size - 1 ) - 4, D_MARGIN + _virtSize + 15 );
			label[ 1 ] = (char)( '0' + ( ( i + 1 ) % 10 ) );
			if ( i > 8 )
				label[ 0 ] = '1';
			else
				label[ 0 ] = STONE.NONE;
			g.drawChars( label, 0, 2, D_MARGIN - 18, D_MARGIN + margin + ( inside * ( _size - 1 - i ) ) / ( _size - 1 ) + 5 );
			g.drawChars( label, 0, 2, D_MARGIN + _virtSize + 2, D_MARGIN + margin + ( inside * ( _size - 1 - i ) ) / ( _size - 1 ) + 5 );
		}
	}
	protected void paintComponent( Graphics g ) {
		super.paintComponent( g );
		drawGoban( g );
		drawStones( g );
		drawByLogic( g );
		if ( _lastMove != null ) {
			int margin = _virtSize / ( _size + 4 );
			int inside = _virtSize - 2 * margin;
			Graphics2D g2D = (Graphics2D)g;      
			g2D.setStroke( new BasicStroke(3F) );
			g.setColor( getStone( _lastMove.col(), _lastMove.row() ) == STONE.BLACK ? Color.WHITE : Color.BLACK );
			g.drawOval(  D_MARGIN + margin + ( inside * _lastMove.col() ) / ( _size - 1 ) - (int)( _diameter / 4 ) - 1,
					D_MARGIN + margin + ( inside * _lastMove.row() ) / ( _size - 1 ) - (int)( _diameter / 4 ) - 1, (int)( _diameter / 2 ), (int)( _diameter / 2 ) );
		}
	}
	protected void drawStone( int $xx, int $yy, int $color, boolean $alpha, Graphics $gc ) {
		Image img = null;
		boolean teritory = false;
		if ( $color == Go.STONE.TERITORY_BLACK ) {
			$color = STONE.BLACK;
			teritory = true;
		}
		if ( $color == Go.STONE.TERITORY_WHITE ) {
			$color = STONE.WHITE;
			teritory = true;
		}
		if ( $color == Go.STONE.DEAD_BLACK ) {
			$alpha = true;
			$color = STONE.BLACK;
		}
		if ( $color == Go.STONE.DEAD_WHITE ) {
			$alpha = true;
			$color = STONE.WHITE;
		}
		if ( $alpha ) {
			if ( $color == STONE.WHITE )
				img = _images._whitesAlpha[ ( $yy * _size + $xx ) % GoImages.D_WHITE_LOOKS ];
			else
				img = _images._blackAlpha;
		} else {
			if ( $color == STONE.WHITE )
				img = _images._whites[ ( $yy * _size + $xx ) % GoImages.D_WHITE_LOOKS ];
			else
				img = _images._black;
		}
		int margin = _virtSize / ( _size + 4 );
		int inside = _virtSize - 2 * margin;
		if ( ! ( $alpha || teritory ) )
			$gc.drawImage( _images._shadow,
					D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( ( _diameter / 2 ) + ( _diameter / 10 ) ),
					D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) + (int)( _diameter / 10 ) - (int)( _diameter / 2 ), this );
		if ( ! teritory )
			$gc.drawImage( img, D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( _diameter / 2 ),
					D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) - (int)( _diameter / 2 ), this );
		else
			$gc.drawImage( img, D_MARGIN + margin + ( inside * $xx ) / ( _size - 1 ) - (int)( _diameter / 4 ),
					D_MARGIN + margin + ( inside * $yy ) / ( _size - 1 ) - (int)( _diameter / 4 ),
					(int)( _diameter / 2 ), (int)( _diameter / 2 ), this );
	}
	private void drawStones( Graphics g ) {
		if ( _stones[0] != STONE.INVALID ) {
			int size = _size * _size;
			size = ( size < _stones.length ? size : _stones.length );
			for ( int i = 0; i < size; ++ i ) {
				byte stone = _stones[ i ];
				if ( ( stone != STONE.NONE ) && ( stone != Go.STONE.DAME ) )
					drawStone( i % _size, i / _size, stone, false, g );
			}
		}
	}
	void setStone( int $col, int $row, byte $stone ) {
		_stones[ $row * _size + $col ] = $stone;
	}
	byte getStone( int $col, int $row ) {
		byte stone = STONE.INVALID;
		if ( validCoords( $col, $row ) )
			stone = _stones[ $row * _size + $col ];
		return ( stone );
	}
	public byte opponent( byte stone ) {
		return ( stone == STONE.WHITE ? STONE.BLACK : STONE.WHITE );
	}
	boolean validCoords( int x, int y ) {
		return ( ( x >= 0 ) && ( x < _size )
				&& ( y >= 0 ) && ( y < _size ) );
	}
	abstract void drawByLogic( Graphics g );
	abstract void move( int $col, int $row, byte $stone );
	void load() {
		JFileChooser fc = new JFileChooser();
		fc.setAcceptAllFileFilterUsed( false );
		fc.setFileFilter( new FileNameExtensionFilter( "Smart Game Format", "sgf" ) );
		if ( fc.showOpenDialog( this ) == JFileChooser.APPROVE_OPTION ) {
			try {
				FileInputStream fis = new FileInputStream( fc.getSelectedFile() );
				BufferedReader inStream = new BufferedReader( new InputStreamReader( fis ) ); 
				updateSGF( inStream );
			} catch ( java.io.FileNotFoundException fnfe ) {
				Con.err( "java.io.FileNotFoundException: " + fnfe.getMessage() );
			}
		}
	}
	void save() {
		JFileChooser fc = new JFileChooser();
		fc.setAcceptAllFileFilterUsed( false );
		fc.setFileFilter( new FileNameExtensionFilter( "Smart Game Format", "sgf" ) );
		if ( fc.showSaveDialog( this ) == JFileChooser.APPROVE_OPTION ) {
			try {
				String path = fc.getSelectedFile().getPath();
				if ( path.indexOf( ".sgf" ) != ( path.length() - 4 ) )
					path += ".sgf";
				FileOutputStream fos = new FileOutputStream( path );
				PrintStream outStream = new PrintStream( fos );
				_sgf.save( outStream );
			} catch ( java.io.FileNotFoundException fnfe ) {
				Con.err( "java.io.FileNotFoundException: " + fnfe.getMessage() );
			}
		}
	}
}

