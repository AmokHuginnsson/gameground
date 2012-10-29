import java.util.Date;
import java.util.Calendar;
import java.util.SimpleTimeZone;
import java.text.DateFormat;
import javax.swing.JSpinner;
import javax.swing.SpinnerDateModel;
import javax.swing.JFormattedTextField;
import javax.swing.JTextField;
import javax.swing.text.DefaultFormatter;
import javax.swing.text.InternationalFormatter;
import javax.swing.text.DefaultCaret;
import java.awt.event.FocusListener;
import java.awt.event.FocusEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.SwingUtilities;

public class DateSpinner extends JSpinner implements ChangeListener, FocusListener {
	public static final long serialVersionUID = 17l;
	JSpinner.DateEditor _editor = null;
	JFormattedTextField _textfield = null;
	private boolean _ignoreEvents = false;
	int _selectedField = 1;
	public DateSpinner( InitClass $init ) {
		super( new SpinnerDateModel() );
		String[] tokens = $init.data().split( "\\|", 2 );
		SpinnerDateModel dm = (SpinnerDateModel)getModel();
		_editor = new JSpinner.DateEditor( this, tokens[0] );
		int max = Integer.parseInt( tokens[ 1 ] );
		dm.setStart( new Date( 0 ) );
		dm.setValue( new Date( 0 ) );
		dm.setEnd( new Date( max * 1000 ) );
		dm.setCalendarField( Calendar.MINUTE );
		_textfield = _editor.getTextField();
		DefaultFormatter f = (DefaultFormatter)_textfield.getFormatter();
		DateFormat df = ((DateFormat)((InternationalFormatter)f).getFormat());
		df.setLenient( false );
		df.setTimeZone( new SimpleTimeZone( 0, "UTC" ) );
		f.setCommitsOnValidEdit( true );
		f.setAllowsInvalid( false );
		f.setOverwriteMode( true );
		_textfield.setHorizontalAlignment( JTextField.RIGHT );
		setEditor( _editor );
		_textfield.addFocusListener( this );
		addChangeListener( this );
	}
	void selectField() {
		SwingUtilities.invokeLater( new Runnable() {
			@Override
			public void run() {
				_textfield.setCaretPosition( _selectedField * 3 );
				_textfield.moveCaretPosition( _selectedField * 3 + 2 );
			}
		} );
	}
	@Override
	public void focusGained( FocusEvent e ) {
		selectField();
	}
	@Override
	public void focusLost( FocusEvent e ) {
		_selectedField = _textfield.getCaretPosition() / 3;
	}
	@Override
	public void setValue( Object $seconds ) {
		_ignoreEvents = true;
		if ( $seconds instanceof Date )
			super.setValue( $seconds );
		else {
			int value = ((Integer)$seconds).intValue();
			super.setValue( new Date( value * 1000 ) );
		}
		if ( _textfield != null )
			selectField();
		_ignoreEvents = false;
	}
	public int getValueInt() {
		SpinnerDateModel dm = (SpinnerDateModel)getModel();
		Date d = dm.getDate();
		Calendar c = Calendar.getInstance();
		c.setTime( d );
		return ( (int)( c.getTimeInMillis() / 1000l ) );
	}
	public void stateChanged( ChangeEvent e ) {
		if ( ( _textfield != null ) && ! _ignoreEvents )
			_selectedField = _textfield.getCaretPosition() / 3;
	}
}
