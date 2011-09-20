import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.PrintStream;

public class SGFTest {
	public static void main( String[] argv ) {
		int argi = 0;
		for ( String s : argv ) {
			System.out.println( "argv[" + argi + "] = " + s );
			++ argi;
		}
		if ( argv.length == 2 ) {
			try {
				FileInputStream fis = new FileInputStream( argv[0] );
				BufferedReader inStream = new BufferedReader( new InputStreamReader( fis ) );
				SGF sgf = new SGF( SGF.GAME_TYPE.GO, "gameground-client" );
				try {
					sgf.load( inStream );
				} catch ( SGFException se ) {
					Con.err( "SGFException: " + se.getMessage() );
					System.exit( 1 );
				}
				FileOutputStream fos = new FileOutputStream( argv[1] );
				PrintStream outStream = new PrintStream( fos );
				sgf.save( outStream );
			} catch ( java.io.FileNotFoundException fnfe ) {
				Con.err( "java.io.FileNotFoundException: " + fnfe.getMessage() );
				System.exit( 1 );
			}
		}
		System.exit( 0 );
	}
}
