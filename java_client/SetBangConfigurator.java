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
	public JSpinner _deckCount;
	public JSpinner _interRoundDelay;
	String getConfigurationString() {
		return ( _playerCount.getValue().toString() + "," + _deckCount.getValue() + "," + _interRoundDelay.getValue() );
	}
	void setDefaults( String $defaults ) {
		try {
			String[] tokens = $defaults.split( ",", 3 );
			SpinnerNumberModel m = (SpinnerNumberModel)_playerCount.getModel();
			m.setMinimum( 2 );
			_playerCount.setValue( Integer.parseInt( tokens[0] ) );
			m = (SpinnerNumberModel)_deckCount.getModel();
			m.setMinimum( 1 );
			m.setMaximum( 99 );
			_deckCount.setValue( Integer.parseInt( tokens[1] ) );
			m = (SpinnerNumberModel)_interRoundDelay.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 60 );
			_interRoundDelay.setValue( Integer.parseInt( tokens[2] ) );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
}
