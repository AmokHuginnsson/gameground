class Player implements Comparable<Player> {
	String _login;
	public Player( String $login ) {
		_login = $login;
	}
	public String toString() {
		return ( _login );
	}
	public int compareTo( Player $player ) {
		return ( _login.compareTo( $player._login ) );
	}
}
