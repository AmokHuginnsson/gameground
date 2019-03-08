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
			_emperors.setValue( Integer.parseInt( tokens[0] ) );
			m = (SpinnerNumberModel)_boardSize.getModel();
			m.setMinimum( 6 );
			m.setMaximum( 20 );
			_boardSize.setValue( Integer.parseInt( tokens[1] ) );
			m = (SpinnerNumberModel)_systems.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 34 );
			_systems.setValue( Integer.parseInt( tokens[2] ) );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void stateChanged( ChangeEvent e ) {
		JSpinner i = (JSpinner)e.getSource();
		JSpinner o = ( i == _emperors ) ? _systems : _emperors;
		int iv = Integer.parseInt( i.getValue().toString() );
		int ov = Integer.parseInt( o.getValue().toString() );
		if ( ( iv + ov ) > 36 )
			o.setValue( 36 - iv );
	}
}
