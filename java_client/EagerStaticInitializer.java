import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.lang.Class;
import java.lang.reflect.Method;

class EagerStaticInitializer {
	static public void touch( Object $for ) { touch( $for, null ); }
	static public void touch( Object $for, String $method ) {
		BufferedReader in = new BufferedReader( new InputStreamReader( $for.getClass().getResourceAsStream( "/class.list" ) ) );
		String line;
		try {
			while ( ( line = in.readLine() ) != null ) {
				if ( line.indexOf( "$" ) == -1 ) {
					try {
						Class<?> dynamic = Class.forName( line.substring( 0, line.indexOf( ".class" ) ) );
						if ( $method != null ) {
							try {
								Method method = dynamic.getDeclaredMethod( $method, new Class<?>[]{ GameGround.class } );
								method.invoke( dynamic, $for );
							} catch ( java.lang.reflect.InvocationTargetException e ) {
							} catch ( java.lang.IllegalAccessException e ) {
							} catch ( java.lang.NoSuchMethodException e ) {
							}
						}
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
