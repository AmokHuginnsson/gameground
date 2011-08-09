import java.util.TreeSet;
import java.util.SortedSet;

class Party {
	String _id;
	String _name;
	String _configuration;
	HAbstractLogic _party;
	SortedSet<Player> _players = java.util.Collections.synchronizedSortedSet( new TreeSet<Player>() );
	public Party( String $id, String $conf ) {
		String[] tokens = $conf.split( ",", 2 );
		_id = $id;
		_name = tokens[0];
		_configuration = tokens[1];
	}
	public String toString() {
		return ( _name + ":" + _id );
	}
	public void setParty( HAbstractLogic $party ) {
		_party = $party;
	}
	public HAbstractLogic getParty() {
		return ( _party );
	}
	public void addPlayer( Player $player ) {
		_players.add( $player );
	}
	public void removePlayer( Player $player ) {
		_players.remove( $player );
	}
	public java.util.Iterator<Player> playerIterator() {
		return ( _players.iterator() );
	}
	public int getPlayerCount() {
		return ( _players.size() );
	}
	public boolean hasPlayer( Player $player ) {
		return ( _players.contains( $player ) );
	}
}
