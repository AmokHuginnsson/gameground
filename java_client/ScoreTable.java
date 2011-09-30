import java.util.Comparator;
import javax.swing.table.TableRowSorter;
import javax.swing.JTable;

public class ScoreTable extends JTable {
	public static final long serialVersionUID = 17l;
	public ScoreTable() {
		super();
	}
	Scores setColumns( String[] $columns ) {
		Scores scores = null;
		setModel( scores = new Scores( $columns ) );
		Comparator<String> comparator = new Comparator<String>() {
			public int compare( String s1, String s2 ) {
				return ( Integer.parseInt( s2 ) - Integer.parseInt( s1 ) );
			}
		};
		TableRowSorter<Scores> rs = new TableRowSorter<Scores>();
		rs.setModel( scores );
		int columnCount = scores.getColumnCount();
		if ( columnCount > 1 )
			rs.setComparator( 1, comparator );
		if ( columnCount > 2 )
			rs.setComparator( 2, comparator );
		setRowSorter( rs );
		setShowGrid( false );
		setAutoCreateRowSorter( true );
		scores.reload();
		return ( scores );
	}
}

