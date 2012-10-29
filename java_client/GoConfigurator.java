import javax.swing.JSpinner;
import javax.swing.JComboBox;
import javax.swing.DefaultComboBoxModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class GoConfigurator extends HAbstractConfigurator implements ChangeListener {
	public static final long serialVersionUID = 17l;
	private boolean _ignoreEvents = false;
	public JComboBox<String> _confGoban;
	public JSpinner _confKomi;
	public JSpinner _confHandicaps;
	public DateSpinner _confMainTime;
	public JSpinner _confByoYomiPeriods;
	public DateSpinner _confByoYomiTime;
	private Go.HGUILocal _owner;
	String getConfigurationString() {
		return ( (String)_confGoban.getSelectedItem() + "," + _confKomi.getValue() + "," + _confHandicaps.getValue() + "," + _confMainTime.getValue() + "," + _confByoYomiPeriods.getValue() + "," + _confByoYomiTime.getValue() );
	}
	void init() {
		SpinnerNumberModel m = (SpinnerNumberModel)_confKomi.getModel();
		m.setMinimum( 0 );
		m.setMaximum( 180 );
		m = (SpinnerNumberModel)_confHandicaps.getModel();
		m.setMinimum( 0 );
		m.setMaximum( 9 );
		m = (SpinnerNumberModel)_confByoYomiPeriods.getModel();
		m.setMinimum( 0 );
		m.setMaximum( 99 );
		_confKomi.addChangeListener( this );
		_confHandicaps.addChangeListener( this );
		_confMainTime.addChangeListener( this );
		_confByoYomiPeriods.addChangeListener( this );
		_confByoYomiTime.addChangeListener( this );
	}
	void setDefaults( String $defaults ) {
		String[] tokens = $defaults.split( ",", 6 );
		gobanModel();
		selectGobanSize( tokens[ 0 ] );

		_confKomi.setValue( Integer.parseInt( tokens[ 1 ] ) );
		_confHandicaps.setValue( Integer.parseInt( tokens[ 2 ] ) );
		_confMainTime.setValue( Integer.parseInt( tokens[ 3 ] ) );
		_confByoYomiPeriods.setValue( Integer.parseInt( tokens[ 4 ] ) );
		_confByoYomiTime.setValue( Integer.parseInt( tokens[ 5 ] ) );
	}
	public void gobanModel() {
		DefaultComboBoxModel<String> model = new DefaultComboBoxModel<String>();
		model.addElement( "19" );
		model.addElement( "13" );
		model.addElement( "9" );
		_confGoban.setModel( model );
	}
	public void setEnabled( boolean $enabled ) {
		_confGoban.setEnabled( $enabled );
		_confKomi.setEnabled( $enabled );
		_confHandicaps.setEnabled( $enabled );
		_confMainTime.setEnabled( $enabled );
		_confByoYomiPeriods.setEnabled( $enabled );
		_confByoYomiTime.setEnabled( $enabled );
	}
	public void selectGobanSize( String $size ) {
		_ignoreEvents = true;
		_confGoban.getModel().setSelectedItem( $size );
		_ignoreEvents = false;
	}
	public void stateChanged( ChangeEvent e ) {
		if ( _ignoreEvents )
			return;
		JSpinner s = (JSpinner)e.getSource();
		if ( s == _confKomi )
			_owner.onKomi();
		else if ( s == _confHandicaps )
			_owner.onHandicaps();
		else if ( s == _confMainTime )
			_owner.onMainTime();
		else if ( s == _confByoYomiPeriods )
			_owner.onByoyomiPeriods();
		else if ( s == _confByoYomiTime )
			_owner.onByoyomiTime();
	}
	public void setOwner( Go.HGUILocal $owner ) {
		_owner = $owner;
	}
	void setValue( JSpinner $spinner, int $value ) {
		_ignoreEvents = true;
		$spinner.setValue( $value );
		_ignoreEvents = false;
	}
	public boolean eventsIgnored() {
		return ( _ignoreEvents );
	}
}
