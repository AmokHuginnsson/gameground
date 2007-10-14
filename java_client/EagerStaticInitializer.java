import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.lang.Class;
import java.lang.reflect.Method;

class EagerStaticInitializer {
	static public void touch( Object $for ) {
		BufferedReader in = new BufferedReader( new InputStreamReader( $for.getClass().getResourceAsStream( "/class.list" ) ) );
		String line;
		try {
			while ( ( line = in.readLine() ) != null ) {
				if ( line.indexOf( "$" ) == -1 ) {
					try {
						Class dynamic = Class.forName( line.substring( 0, line.indexOf( ".class" ) ) );
					} catch ( java.lang.ClassNotFoundException e ) {
					}
				}
			}
		} catch ( java.io.IOException ee ) {
			ee.printStackTrace();
			System.exit( 1 );
		}
	}
}
