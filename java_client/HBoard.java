import java.io.InputStreamReader;
import java.io.InputStream;
import java.net.URL;
import java.awt.Image;
import java.awt.Graphics;
import javax.swing.*;

public class HBoard extends JPanel {
	public static final long serialVersionUID = 7l;
	Image _background;
	public HBoard() {
		try {
			InputStream in = getClass().getResourceAsStream( "res/god.png" );
			int size = in.available();
			byte buffer[] = new byte[size];
			for( int i = 0; i < size; ++ i )
				buffer[ i ] = (byte)in.read();
			_background = new ImageIcon( buffer ).getImage();
		} catch ( Exception e ) {
			e.printStackTrace();
		}
	}
	protected void paintComponent( Graphics g ) {
		g.drawImage( _background, 0, 0, this );
	}
}

