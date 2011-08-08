import java.util.TreeSet;
import java.util.SortedSet;

class Party {
	String _id;
	String _name;
	HAbstractLogic _party;
	SortedSet<Player> _players = java.util.Collections.synchronizedSortedSet( new TreeSet<Player>() );
	public Party( String $id, String $name ) {
		_id = $id;
		_name = $name;
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
}
