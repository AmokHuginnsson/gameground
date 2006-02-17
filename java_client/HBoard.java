import java.io.InputStreamReader;
import java.io.InputStream;
import java.net.URL;
import java.awt.Image;
import java.awt.Graphics;
import javax.swing.JPanel;
import javax.swing.ImageIcon;
import java.awt.Color;

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

public class HBoard extends JPanel {
//--------------------------------------------//
	public static final long serialVersionUID = 7l;
	int _size;
	int _diameter;
	HImages _images;
	HGUIMain _gui;
	HSystem[] _systems;
//--------------------------------------------//
	public HBoard() {
		_size = -1;
	}
	void setGui( HGUIMain $gui ) {
		_gui = $gui;
	}
	public void setImages( HImages $images ) {
		_images = $images;
	}
	void setSize( int $size ) {
		_size = $size;
		_diameter = 640 / $size;
	}
	void setSystems( HSystem[] $systems ) {
		_systems = $systems;
//		updateUI();
	}
	private void drawSystem( Graphics $gs, int $no, int $coordX, int $coordY, int $color ) {
		$gs.drawImage( _images._systems[$no],
				$coordX * _diameter + ( _diameter - 32 ) / 2,
				$coordY * _diameter + ( _diameter - 32 ) / 2, this );
		if ( $color >= 0 ) {
			$gs.setColor ( _gui._widgets._colors[ $color ] );
			$gs.drawRect ( $coordX * _diameter + 1,
					$coordY * _diameter + 1,
					_diameter - 2, _diameter - 2 );
			$gs.drawRect ( $coordX * _diameter + 2,
					$coordY * _diameter + 2,
					_diameter - 4, _diameter - 4 );
		}
	}
	protected void paintComponent( Graphics g ) {
		g.drawImage( _images._background, 0, 0, this );
		if ( _size > 0 ) {
			int systemCount = _gui._client.getSystemCount();
			if ( _systems != null ) {
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
				g.drawLine( 0, i * _diameter, 640, i * _diameter );
			for ( int i = 0; i < _size; ++ i )
				g.drawLine( i * _diameter, 0, i * _diameter, 640 );
		}
	}
}

