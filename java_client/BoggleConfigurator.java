import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class BoggleConfigurator extends HAbstractConfigurator {
	public static final long serialVersionUID = 17l;
	public JSpinner _playerCount;
	public JSpinner _roundTime;
	public JSpinner _maxRounds;
	public JSpinner _interRoundDelay;
	String getConfigurationString() {
		return ( _playerCount.getValue() + "," + _roundTime.getValue() + "," + _maxRounds.getValue() + "," + _interRoundDelay.getValue() );
	}
	void setDefaults( String $defaults ) {
		String[] tokens = $defaults.split( ",", 4 );
		try {
			SpinnerNumberModel m = (SpinnerNumberModel)_playerCount.getModel();
			m.setMinimum( 2 );
			_playerCount.setValue( new Integer( tokens[0] ).intValue() );
			m = (SpinnerNumberModel)_maxRounds.getModel();
			m.setMinimum( 60 );
			m.setMaximum( 300 );
			_roundTime.setValue( new Integer( tokens[1] ).intValue() );
			m = (SpinnerNumberModel)_roundTime.getModel();
			m.setMinimum( 1 );
			m.setMaximum( 99 );
			_maxRounds.setValue( new Integer( tokens[2] ).intValue() );
			m = (SpinnerNumberModel)_interRoundDelay.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 60 );
			_interRoundDelay.setValue( new Integer( tokens[3] ).intValue() );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
}
