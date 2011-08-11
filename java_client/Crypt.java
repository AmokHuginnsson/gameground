import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

class Crypt {
	public static String SHA1( String $val ) {
		String val = "";
		try {
			MessageDigest md = MessageDigest.getInstance( "SHA1" );
			md.update( $val.getBytes() );
			byte[] d = md.digest();
			val = String.format( "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
					d[0], d[1], d[2], d[3],
					d[4], d[5], d[6], d[7],
					d[8], d[9], d[10], d[11],
					d[12], d[13], d[14], d[15],
					d[16], d[17], d[18], d[19] );
		} catch ( NoSuchAlgorithmException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( val );
	}
}
