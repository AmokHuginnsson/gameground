import javax.swing.JComboBox;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

public class BoggleConfigurator extends HAbstractConfigurator {
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
	public JComboBox<BoggleScoringSystem> _scoringSystem;
	public JSpinner _playerCount;
	public JSpinner _roundTime;
	public JSpinner _maxRounds;
	public JSpinner _interRoundDelay;
	String getConfigurationString() {
		return ( ((BoggleScoringSystem)_scoringSystem.getSelectedItem())._id + "," + _playerCount.getValue() + "," + _roundTime.getValue() + "," + _maxRounds.getValue() + "," + _interRoundDelay.getValue() );
	}
	void setDefaults( String $defaults ) {
		_scoringSystem.removeAll();
		_scoringSystem.addItem( new BoggleScoringSystem( "original", "Original Boggle" ) );
		_scoringSystem.addItem( new BoggleScoringSystem( "fibonacci", "Fibonacci" ) );
		_scoringSystem.addItem( new BoggleScoringSystem( "geometric", "Geometric" ) );
		_scoringSystem.addItem( new BoggleScoringSystem( "fibonacci4", "Fibonacci 4-based" ) );
		_scoringSystem.addItem( new BoggleScoringSystem( "geometric4", "Geometric 4-based" ) );
		_scoringSystem.addItem( new BoggleScoringSystem( "longestwords", "Longest words" ) );
		String[] tokens = $defaults.split( ",", 5 );
		try {
			int defaultScotingSystem = 0;
			if ( "original".equals( tokens[0] ) )
				defaultScotingSystem = 0;
			else if ( "fibonacci".equals( tokens[0] ) )
				defaultScotingSystem = 1;
			else if ( "geometric".equals( tokens[0] ) )
				defaultScotingSystem = 2;
			else if ( "fibonacci4".equals( tokens[0] ) )
				defaultScotingSystem = 3;
			else if ( "geometric4".equals( tokens[0] ) )
				defaultScotingSystem = 4;
			else if ( "longestwords".equals( tokens[0] ) )
				defaultScotingSystem = 5;
			_scoringSystem.setSelectedIndex( defaultScotingSystem );
			SpinnerNumberModel m = (SpinnerNumberModel)_playerCount.getModel();
			m.setMinimum( 2 );
			_playerCount.setValue( new Integer( tokens[1] ).intValue() );
			m = (SpinnerNumberModel)_roundTime.getModel();
			m.setMinimum( 60 );
			m.setMaximum( 300 );
			_roundTime.setValue( new Integer( tokens[2] ).intValue() );
			m = (SpinnerNumberModel)_maxRounds.getModel();
			m.setMinimum( 1 );
			m.setMaximum( 99 );
			_maxRounds.setValue( new Integer( tokens[3] ).intValue() );
			m = (SpinnerNumberModel)_interRoundDelay.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 60 );
			_interRoundDelay.setValue( new Integer( tokens[4] ).intValue() );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
}
