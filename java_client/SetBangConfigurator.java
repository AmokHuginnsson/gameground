import javax.swing.JComboBox;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

public class SetBangConfigurator extends HAbstractConfigurator {
	private class BoggleScoringSystem {
		public String _id;
		public String _name;
		public BoggleScoringSystem( String $id, String $name ) {
			_id = $id;
			_name = $name;
		}
		public String toString() {
			return ( _name );
		}
	}
	public static final long serialVersionUID = 17l;
	public JSpinner _playerCount;
	String getConfigurationString() {
		return ( _playerCount.getValue().toString() );
	}
	void setDefaults( String $defaults ) {
		try {
			SpinnerNumberModel m = (SpinnerNumberModel)_playerCount.getModel();
			m.setMinimum( 2 );
			_playerCount.setValue( new Integer( $defaults ).intValue() );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
}
