import javax.swing.JComboBox;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

public class BoggleConfigurator extends HAbstractConfigurator {
	private class IdLabel {
		public String _id;
		public String _name;
		public IdLabel( String $id, String $name ) {
			_id = $id;
			_name = $name;
		}
		public String toString() {
			return ( _name );
		}
	}
	public static final long serialVersionUID = 17l;
	public JComboBox<IdLabel> _language;
	public JComboBox<IdLabel> _scoringSystem;
	public JSpinner _playerCount;
	public JSpinner _roundTime;
	public JSpinner _maxRounds;
	public JSpinner _interRoundDelay;
	String getConfigurationString() {
		return (
			((IdLabel)_language.getSelectedItem())._id
			+ "," + ((IdLabel)_scoringSystem.getSelectedItem())._id
			+ "," + _playerCount.getValue()
			+ "," + _roundTime.getValue()
			+ "," + _maxRounds.getValue()
			+ "," + _interRoundDelay.getValue()
		);
	}
	void setDefaults( String $defaults ) {
		_language.removeAll();
		_language.addItem( new IdLabel( "pl", "Polish" ) );
		_language.addItem( new IdLabel( "en", "English" ) );
		_scoringSystem.removeAll();
		_scoringSystem.addItem( new IdLabel( "original", "Original Boggle" ) );
		_scoringSystem.addItem( new IdLabel( "fibonacci", "Fibonacci" ) );
		_scoringSystem.addItem( new IdLabel( "geometric", "Geometric" ) );
		_scoringSystem.addItem( new IdLabel( "fibonacci4", "Fibonacci 4-based" ) );
		_scoringSystem.addItem( new IdLabel( "geometric4", "Geometric 4-based" ) );
		_scoringSystem.addItem( new IdLabel( "longestwords", "Longest words" ) );
		String[] tokens = $defaults.split( ",", 6 );
		try {
			int defaultScotingSystem = 0;
			if ( "original".equals( tokens[1] ) ) {
				defaultScotingSystem = 0;
			} else if ( "fibonacci".equals( tokens[1] ) ) {
				defaultScotingSystem = 1;
			} else if ( "geometric".equals( tokens[1] ) ) {
				defaultScotingSystem = 2;
			} else if ( "fibonacci4".equals( tokens[1] ) ) {
				defaultScotingSystem = 3;
			} else if ( "geometric4".equals( tokens[1] ) ) {
				defaultScotingSystem = 4;
			} else if ( "longestwords".equals( tokens[1] ) ) {
				defaultScotingSystem = 5;
			}
			_scoringSystem.setSelectedIndex( defaultScotingSystem );
			SpinnerNumberModel m = (SpinnerNumberModel)_playerCount.getModel();
			m.setMinimum( 2 );
			_playerCount.setValue( Integer.parseInt( tokens[2] ) );
			m = (SpinnerNumberModel)_roundTime.getModel();
			m.setMinimum( 60 );
			m.setMaximum( 300 );
			_roundTime.setValue( Integer.parseInt( tokens[3] ) );
			m = (SpinnerNumberModel)_maxRounds.getModel();
			m.setMinimum( 1 );
			m.setMaximum( 99 );
			_maxRounds.setValue( Integer.parseInt( tokens[4] ) );
			m = (SpinnerNumberModel)_interRoundDelay.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 60 );
			_interRoundDelay.setValue( Integer.parseInt( tokens[5] ) );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
}
