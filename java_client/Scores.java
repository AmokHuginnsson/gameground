import javax.swing.table.AbstractTableModel;
import java.util.ArrayList;

public class Scores extends AbstractTableModel {
	String[] _header = null;
	private ArrayList<PlayerScore> _players = new ArrayList<PlayerScore>();
	public static final long serialVersionUID = 17l;
	public Scores( String[] $columns ) {
		_header = $columns;
	}
	public String getColumnName(int col) {
		return ( _header != null ? _header[ col ] : "" );
	}
	public int getRowCount() {
		return ( _players.size() );
	}
	public int getColumnCount() {
		return ( _header != null ? _header.length : 0 );
	}
	public Object getValueAt(int row, int col) {
		return _players.get( row ).get( col );
	}
	public boolean isCellEditable(int row, int col)
		{ return false; }
	public void setValueAt(Object value, int row, int col) {
	}
	public int size() {
		return ( _players.size() );
	}
	public PlayerScore get( int $row ) {
		return ( _players.get( $row ) );
	}
	public void add( PlayerScore $score ) {
		_players.add( $score );
	}
	public void reload() {
		fireTableDataChanged();
	}
}
