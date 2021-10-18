import java.util.Properties;
import java.io.FileReader;
import java.io.FileWriter;

public class Ini extends Properties {
	private static final String _path = System.getenv( "HOME" ) + "/etc/conf/gameground-clientrc";
	public static final long serialVersionUID = 19l;
	public void load() {
		try {
			/* System.getProperties().list( System.out ); */
			load( new FileReader( _path ) );
		} catch ( java.io.FileNotFoundException e ) {
			Con.err( e.getMessage() );
		} catch ( java.io.IOException e ) {
			Con.err( "FATAL ERROR: " + e.getMessage() );
			e.printStackTrace();
			System.exit( 1 );
		} catch ( Exception e ) {
			Con.err( "Insufficient privileges to guess home directory: " + e.getMessage() );
		}
	}
	public void save() {
		try {
			store( new FileWriter( _path ), "Configuration of GameGround client." );
		} catch ( java.io.IOException e ) {
			Con.err( "FATAL ERROR: " + e.getMessage() );
		}
	}
}

