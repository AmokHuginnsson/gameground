import java.util.TreeSet;
import java.util.SortedSet;

class HPlayerSet {
	String _id;
	String _login;
	String _configuration;
	SortedSet<String> _players = java.util.Collections.synchronizedSortedSet( new TreeSet<String>() );

	public HPlayerSet( String $id, String $name ) {
		this( $id, $name, null );
	}
	public HPlayerSet( String $id, String $name, String $configuration ) {
		_id = $id;
		_login = $name;
		_configuration = $configuration;
	}
	public String toString() {
		return ( _login );
	}
	public void addPlayer( String $player ) {
		_players.add( $player );
	}
	public void removePlayer( String $player ) {
		_players.remove( $player );
	}
	public java.util.Iterator<String> peopleIterator() {
		return ( _players.iterator() );
	}
}
