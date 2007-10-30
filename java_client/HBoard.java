import java.io.InputStreamReader;
import java.io.InputStream;
import java.net.URL;
import java.awt.Image;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.ImageIcon;
import javax.swing.event.MouseInputListener;

class HImages {
//--------------------------------------------//
	static int PLANETS_ICON_COUNT = 36;
	public Image _background;
	public Image[] _systems;
//--------------------------------------------//
	public HImages() {
		_systems = new Image[PLANETS_ICON_COUNT];
		_background = loadImage( "res/god.png" );
		for ( int i = 0; i < PLANETS_ICON_COUNT; ++ i ) {
			String path = "res/system";
			path += i + ".xpm";
			_systems[ i ] = loadImage( path );
		}
	}
	private Image loadImage( String $path ) {
		try {
			InputStream in = getClass().getResourceAsStream( $path );
			int size = in.available();
			byte buffer[] = new byte[size];
			for ( int i = 0; i < size; ++ i )
				buffer[ i ] = (byte)in.read();
			if ( $path.indexOf( "xpm" ) >= 0 ) {
				return Xpm.XpmToImage( new String( buffer ) );
			} else {
				return new ImageIcon( buffer ).getImage();
			}
		} catch ( Exception e ) {
			e.printStackTrace();
		}
		return null;
	}
}

public class HBoard extends JPanel implements MouseInputListener {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	boolean _help;
	int _size;
	double _diameterX;
	double _diameterY;
	int _cursorX;
	int _cursorY;
	int _sourceSystem;
	int _destinationSystem;
	HImages _images;
	HGalaxy _logic;
	HSystem[] _systems;
//--------------------------------------------//
	public HBoard() {
		_help = false;
		_size = -1;
		_cursorX = -1;
		_cursorY = -1;
		_diameterX = -1;
		_diameterY = -1;
		_sourceSystem = -1;
	}
	public void mouseMoved( MouseEvent $event ) {
		int cursorX = (int)( $event.getX() / _diameterX );
		int cursorY = (int)( $event.getY() / _diameterY );
		if ( ( _cursorX != cursorX ) || ( _cursorY != cursorY ) ) {
			_cursorX = cursorX;
			_cursorY = cursorY;
			_logic._gui._systemInfo.setText( "" );
			_logic._gui._emperorInfo.setText( "" );
			_logic._gui._productionInfo.setText( "" );
			_logic._gui._fleetInfo.setText( "" );
			repaint();
		}
	}
	public void mouseDragged( MouseEvent $event ) {
	}
	public void mouseEntered( MouseEvent $event ) {
	}
	public void mouseReleased( MouseEvent $event ) {
		if ( $event.getButton() == MouseEvent.BUTTON3 ) {
			_help = false;
			repaint();
		}
	}
	public void mousePressed( MouseEvent $event ) {
		if ( $event.getButton() == MouseEvent.BUTTON3 ) {
			_help = true;
			repaint();
		}
	}
	public void mouseClicked( MouseEvent $event ) {
		if ( _systems != null ) {
			int x = (int)( $event.getX() / _diameterX );
			int y = (int)( $event.getY() / _diameterY );
			System.out.println( "coords: (" + x + "," + y + ")" );
			int sysNo = getSysNo( x, y );
			if ( sysNo >= 0 ) {
				if ( _logic.getState() != HGalaxy.State.LOCKED ) {
					if ( _logic.getState() == HGalaxy.State.NORMAL ) {
						System.out.println( "colors: " + _systems[ sysNo ]._color + ", " + _logic._color );
						if ( ( _systems[ sysNo ]._color == _logic._color ) && ( _systems[ sysNo ]._fleet > 0 ) ) {
							_logic.setState( HGalaxy.State.SELECT );
							_sourceSystem = sysNo;
						}
					} else if ( _logic.getState() == HGalaxy.State.SELECT ) {
						if ( sysNo != _sourceSystem ) {
							_logic._gui._fleet.setEditable( true );
							_logic._gui._fleet.setText( String.valueOf( _systems[ _sourceSystem ]._fleet ) );
							_logic._gui._fleet.requestFocus();
							_logic._gui._fleet.selectAll();
							_logic.setState( HGalaxy.State.INPUT );
							_destinationSystem = sysNo;
						}
					}
				} else {
					System.out.println( "LOCKED" );
				}
			} else {
				System.out.println( "No system." );
			}
		}
	}
	public void mouseExited( MouseEvent $event ) {
		_cursorX = -1;
		_cursorY = -1;
		repaint();
		_logic._gui._systemInfo.setText( "" );
		_logic._gui._emperorInfo.setText( "" );
		_logic._gui._productionInfo.setText( "" );
		_logic._gui._fleetInfo.setText( "" );
	}
	int distance( int $source, int $destination ) {
		int dx = 0, dy = 0, distance = 0;
		if ( $source != $destination ) {
			dx = _systems[$source]._coordinateX - _systems[$destination]._coordinateX;
			dy = _systems[$source]._coordinateY - _systems[$destination]._coordinateY;
			dx = ( dx >= 0 ) ? dx : - dx;
			dy = ( dy >= 0 ) ? dy : - dy;
			dx = ( ( _size - dx ) < dx ) ? _size - dx : dx;
			dy = ( ( _size - dy ) < dy ) ? _size - dy : dy;
			distance = (int) ( java.lang.Math.sqrt ( (double) ( dx * dx + dy * dy ) ) + 0.5 );
		}
		return ( distance );
	}
	int getSysNo( int $coordX, int $coordY ) {
		int systemCount = _logic.getSystemCount();
		for ( int i = 0; i < systemCount; ++ i ) {
			if ( ( _systems[ i ]._coordinateX == $coordX ) && ( _systems[ i ]._coordinateY == $coordY ) ) {
				return i;
			}
		}
		return -1;
	}
	void setGui( HGalaxy $gui ) {
		_logic = $gui;
	}
	public void setImages( HImages $images ) {
		_images = $images;
	}
	void setSize( int $size ) {
		_size = $size;
	}
	void setSystems( HSystem[] $systems ) {
		_systems = $systems;
		addMouseMotionListener( this );
		addMouseListener( this );
		repaint();
	}
	private void drawSystem( Graphics $gs, int $no, int $coordX, int $coordY, int $color ) {
		$gs.drawImage( _images._systems[$no],
				(int)( $coordX * _diameterX + ( _diameterX - 32 ) / 2 ),
				(int)( $coordY * _diameterY + ( _diameterY - 32 ) / 2 ), this );
		if ( ( $color >= 0 ) || ( ( $coordX == _cursorX ) && ( $coordY == _cursorY ) ) ) {
			if ( ( $coordX == _cursorX ) && ( $coordY == _cursorY ) ) {
				$gs.setColor ( _logic._gui._colors[ HGUIface.Colors.WHITE ] );
				_logic._gui._systemInfo.setText( _logic._systemNames[ $no ] );
				if ( $color >= 0 )
					_logic._gui._emperorInfo.setText( _logic._emperors.get( $color ) );
				else
					_logic._gui._emperorInfo.setText( "" );
				if ( _systems[ $no ]._production >= 0 )
					_logic._gui._productionInfo.setText( String.valueOf( _systems[ $no ]._production ) );
				else
					_logic._gui._productionInfo.setText( "?" );
				if ( _systems[ $no ]._fleet >= 0 )
					_logic._gui._fleetInfo.setText( String.valueOf( _systems[ $no ]._fleet ) );
				else
					_logic._gui._fleetInfo.setText( "?" );
				if ( _logic.getState() == HGalaxy.State.SELECT )
					_logic._gui._arrival.setText( String.valueOf( _logic._round + distance( _sourceSystem, $no ) ) );
			}	else {
				$gs.setColor ( _logic._gui._colors[ _logic._gui.lcolor( $color ) ] );
			}
			$gs.drawRect ( (int)( $coordX * _diameterX + 1 ),
					(int)( $coordY * _diameterY + 1 ),
					(int)( _diameterX - 2 ), (int)( _diameterY - 2 ) );
			$gs.drawRect ( (int)( $coordX * _diameterX + 2 ),
					(int)( $coordY * _diameterY + 2 ),
					(int)( _diameterX - 4 ), (int)( _diameterY - 4 ) );
		}
		if ( _help ) {
			$gs.setColor ( _logic._gui._colors[ HGUIface.Colors.WHITE ] );
			$gs.drawString( _logic._systemNames[ $no ],
					(int)( $coordX * _diameterX + 2 ),
					(int)( ( $coordY + 1 ) * _diameterY - 2 ) );
		}
	}
	protected void paintComponent( Graphics g ) {
		java.awt.Dimension d = getSize();
		_diameterX = d.getWidth() / _size;
		_diameterY = d.getHeight() / _size;
		g.drawImage( _images._background, 0, 0, (int)d.getWidth(), (int)d.getHeight(), Color.black, this );
		if ( _size > 0 ) {
			if ( _systems != null ) {
				int systemCount = _logic.getSystemCount();
				_logic._gui._arrival.setText( String.valueOf( _logic._round ) );
				for ( int i = 0; i < systemCount; ++ i )
					drawSystem( g, i, _systems[ i ]._coordinateX, _systems[ i ]._coordinateY, _systems[ i ]._color );
			}
			/*
			for ( int i = 0; i < 10; ++ i )
				drawSystem( g, i, i + 5, 8, i );
			for ( int i = 0; i < 10; ++ i )
				drawSystem( g, i + 10, i + 5, 9, i );
			for ( int i = 0; i < 10; ++ i )
				drawSystem( g, i + 20, i + 5, 10, i );
			for ( int i = 0; i < 6; ++ i )
				drawSystem( g, i + 30, i + 5, 11, i );
			*/
			g.setColor( Color.darkGray );
			for ( int i = 0; i < _size; ++ i )
				g.drawLine( 0, (int)( i * _diameterY ), (int)d.getWidth(), (int)( i * _diameterY ) );
			for ( int i = 0; i < _size; ++ i )
				g.drawLine( (int)( i * _diameterX ), 0, (int)( i * _diameterX ), (int)d.getHeight() );
		}
	}
}

