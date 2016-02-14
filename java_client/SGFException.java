/*
 * SGF parser Java library
 *
 * This library is part of GameGround project.
 * http://codestation.org/?h-action=menu-project&menu=submenu-project&page=&project=gameground
 *
 * (C) 2016 Marcin Konarski - All Rights Reserved
 */

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
