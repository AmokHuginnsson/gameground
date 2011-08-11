class Sec {
	public static String escape( String $val ) {
		return ( $val.replace( "\\", "\\\\" ).replace( "\n", "\\n" ).replace( ":", "\\C" ).replace( ",", "\\K" ) );
	}
	public static String unescape( String $val ) {
		return ( $val.replace( "\\K", "," ).replace( "\\C", ":" ).replace( "\\n", "\n" ).replace( "\\\\", "\\" ) );
	}
}
