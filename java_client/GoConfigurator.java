import javax.swing.JSpinner;
import javax.swing.JComboBox;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class GoConfigurator extends HAbstractConfigurator {
	public static final long serialVersionUID = 17l;
	public JComboBox _confGoban;
	public JSpinner _confKomi;
	public JSpinner _confHandicaps;
	public JSpinner _confMainTime;
	public JSpinner _confByoYomiPeriods;
	public JSpinner _confByoYomiTime;
	String getConfigurationString() {
		return ( (String)_confGoban.getSelectedItem() + "," + _confKomi.getValue() + "," + _confHandicaps.getValue() + "," + _confMainTime.getValue() + "," + _confByoYomiPeriods.getValue() + "," + _confByoYomiTime.getValue() );
	}
	void setDefaults( String $defaults ) {
		String[] tokens = $defaults.split( ",", 6 );
		try {
			_confGoban.addItem( "19" );
			_confGoban.addItem( "13" );
			_confGoban.addItem( "9" );
			_confGoban.setSelectedItem( tokens[ 0 ] );

			SpinnerNumberModel m = (SpinnerNumberModel)_confKomi.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 6 );
			_confKomi.setValue( new Integer( tokens[ 1 ] ).intValue() );

			m = (SpinnerNumberModel)_confHandicaps.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 9 );
			_confHandicaps.setValue( new Integer( tokens[ 2 ] ).intValue() );

			m = (SpinnerNumberModel)_confMainTime.getModel();
			m.setMinimum( 2 );
			m.setMaximum( 60 );
			_confMainTime.setValue( new Integer( tokens[ 3 ] ).intValue() );

			m = (SpinnerNumberModel)_confByoYomiPeriods.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 20 );
			_confByoYomiPeriods.setValue( new Integer( tokens[ 4 ] ).intValue() );

			m = (SpinnerNumberModel)_confByoYomiTime.getModel();
			m.setMinimum( 0 );
			m.setMaximum( 5 );
			_confByoYomiTime.setValue( new Integer( tokens[ 5 ] ).intValue() );
		} catch ( NumberFormatException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
}
