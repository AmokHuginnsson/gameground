import java.io.InputStream;
import java.awt.Image;
import javax.swing.ImageIcon;

public class HImages {
//--------------------------------------------//
	static final int PLANETS_ICON_COUNT = 36;
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

