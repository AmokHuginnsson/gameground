public class SGFException extends Exception {
	public static final long serialVersionUID = 17l;
	int _code = 0;
	public SGFException( String $message, int $code ) {
		super( $message );
		_code = $code;
	}
	public SGFException( String $message ) {
		super( $message );
	}
	public String getMessage() {
		return ( super.getMessage() + ": " + _code );
	}
}
