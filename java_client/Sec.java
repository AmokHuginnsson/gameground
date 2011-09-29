class Sec {
	public static String escape( String $val ) {
		return ( $val.replace( "\\", "\\E" ).replace( "\n", "\\n" ).replace( "\"", "\\Q" ).replace( "'", "\\A" ).replace( ":", "\\C" ).replace( ",", "\\K" ) );
	}
	public static String unescape( String $val ) {
		return ( $val.replace( "\\K", "," ).replace( "\\C", ":" ).replace( "\\A", "'" ).replace( "\\Q", "\"" ).replace( "\\n", "\n" ).replace( "\\E", "\\" ) );
	}
}
