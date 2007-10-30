import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class HGalaxyConfigurator extends HAbstractConfigurator implements ChangeListener {
	public static final long serialVersionUID = 17l;
	public JSpinner _emperors;
	public JSpinner _systems;
	public JSpinner _boardSize;
	String getConfigurationString() {
		return ( _emperors.getValue() + "," + _boardSize.getValue() + "," + _systems.getValue() );
	}
	void setDefaults( String $defaults ) {
		String[] tokens = $defaults.split( ",", 3 );
		try {
			_emperors.addChangeListener( this );
			_systems.addChangeListener( this );
			SpinnerNumberModel m = (SpinnerNumberModel)_emperors.getModel();
			m.setMinimum( 2 );
			m.setMaximum( 36 );
			_emperors.setValue( new Integer( tokens[0] ).intValue() );
			m = (SpinnerNumberModel)_boardSize.getModel();
			m.setMinimum( 6 );
			m.setMaximum( 20 );
			_boardSize.setValue( new Integer( tokens[1] ).intValue() );
			m = (SpinnerNumberModel)_systems.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 34 );
			_systems.setValue( new Integer( tokens[2] ).intValue() );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void stateChanged( ChangeEvent e ) {
		JSpinner i = (JSpinner)e.getSource();
		JSpinner o = ( i == _emperors ) ? _systems : _emperors;
		int iv = new Integer( i.getValue().toString() ).intValue();
		int ov = new Integer( o.getValue().toString() ).intValue();
		if ( ( iv + ov ) > 36 )
			o.setValue( 36 - iv );
	}
}
