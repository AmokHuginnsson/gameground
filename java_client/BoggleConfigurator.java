import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class BoggleConfigurator extends HAbstractConfigurator implements ChangeListener {
	public static final long serialVersionUID = 17l;
	public JSpinner _playerCount;
	public JSpinner _roundTime;
	public JSpinner _maxRounds;
	String getConfigurationString() {
		return ( _playerCount.getValue() + "," + _maxRounds.getValue() + "," + _roundTime.getValue() );
	}
	void setDefaults( String $defaults ) {
		String[] tokens = $defaults.split( ",", 3 );
		try {
			_playerCount.addChangeListener( this );
			_roundTime.addChangeListener( this );
			SpinnerNumberModel m = (SpinnerNumberModel)_playerCount.getModel();
			m.setMinimum( 2 );
			m.setMaximum( 36 );
			_playerCount.setValue( new Integer( tokens[0] ).intValue() );
			m = (SpinnerNumberModel)_maxRounds.getModel();
			m.setMinimum( 6 );
			m.setMaximum( 20 );
			_maxRounds.setValue( new Integer( tokens[1] ).intValue() );
			m = (SpinnerNumberModel)_roundTime.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 34 );
			_roundTime.setValue( new Integer( tokens[2] ).intValue() );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void stateChanged( ChangeEvent e ) {
		JSpinner i = (JSpinner)e.getSource();
		JSpinner o = ( i == _playerCount ) ? _roundTime : _playerCount;
		int iv = new Integer( i.getValue().toString() ).intValue();
		int ov = new Integer( o.getValue().toString() ).intValue();
		if ( ( iv + ov ) > 36 )
			o.setValue( 36 - iv );
	}
}
