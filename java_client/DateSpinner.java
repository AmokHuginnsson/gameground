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
import javax.swing.SwingUtilities;

public class DateSpinner extends JSpinner implements FocusListener {
	public static final long serialVersionUID = 17l;
	JSpinner.DateEditor _editor = null;
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
		JFormattedTextField tf = _editor.getTextField();
		DefaultFormatter f = (DefaultFormatter)tf.getFormatter();
		DateFormat df = ((DateFormat)((InternationalFormatter)f).getFormat());
		df.setLenient( false );
		df.setTimeZone( new SimpleTimeZone( 0, "UTC" ) );
		f.setCommitsOnValidEdit( true );
		f.setAllowsInvalid( false );
		f.setOverwriteMode( true );
		tf.setHorizontalAlignment( JTextField.RIGHT );
		setEditor( _editor );
		tf.addFocusListener( this );
	}
	@Override
	public void focusGained( FocusEvent e ) {
		final JFormattedTextField tf = (JFormattedTextField)e.getSource();
		SwingUtilities.invokeLater( new Runnable() {
			@Override
			public void run() {
				JSpinner spinner = (JSpinner)tf.getParent().getParent();
				tf.setCaretPosition( 3 );
				tf.moveCaretPosition( 5 );
			}
		} );
	}
	@Override
	public void focusLost( FocusEvent e ) {
	}
	@Override
	public void setValue( Object $seconds ) {
		if ( $seconds instanceof Date )
			super.setValue( $seconds );
		else {
			int value = ((Integer)$seconds).intValue();
			super.setValue( new Date( value * 1000 ) );
		}
	}
	public int getValueInt() {
		SpinnerDateModel dm = (SpinnerDateModel)getModel();
		Date d = dm.getDate();
		Calendar c = Calendar.getInstance();
		c.setTime( d );
		return ( (int)( c.getTimeInMillis() / 1000l ) );
	}
}
