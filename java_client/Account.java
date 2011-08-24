import javax.swing.JTextField;
import javax.swing.JTextArea;
import javax.swing.JButton;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;
import org.swixml.XDialog;
import org.swixml.SwingEngine;

public class Account extends XDialog implements DocumentListener {
	public static final long serialVersionUID = 17l;
	GameGround _app = null;
	boolean _confirmed = false;
	String _configuration;
	public JTextField _name;
	public JTextField _email;
	public JTextField _oldPassword;
	public JTextField _newPassword;
	public JTextField _newPasswordRepeat;
	public JTextArea _description;
	public JButton _ok;
	public Account( GameGround $app, String $conf ) {
		_app = $app;
		String[] tokens = $conf.split( ",", 4 );
		try {
			new SwingEngine( this ).insert( AppletJDOMHelper.loadResource( "/res/account.xml", this ), this );
			_oldPassword.getDocument().addDocumentListener( this );
			_newPassword.getDocument().addDocumentListener( this );
			_newPasswordRepeat.getDocument().addDocumentListener( this );
			_name.setText( Sec.unescape( tokens[1] ) );
			_email.setText( Sec.unescape( tokens[3] ) );
			_description.setText( Sec.unescape( tokens[2] ) );
			setVisible( true );
		} catch ( java.lang.Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void changedUpdate( DocumentEvent e ) {
		validatePasswords();
	}
	public void insertUpdate( DocumentEvent e ) {
		validatePasswords();
	}
	public void removeUpdate( DocumentEvent e ) {
		validatePasswords();
	}
	void validatePasswords() {
		String oldPassword = _oldPassword.getText();
		String newPassword = _newPassword.getText();
		String newPasswordRepeat = _newPasswordRepeat.getText();
		boolean oldPasswordNull = ( oldPassword.length() == 0 );
		boolean newPasswordNull = ( newPassword.length() == 0 );
		boolean newPasswordRepeatNull = ( newPasswordRepeat.length() == 0 );
		boolean valid = false;
		if ( oldPasswordNull && newPasswordNull && newPasswordRepeatNull ) {
			valid = true;
		} else {
			if ( ! ( oldPasswordNull || newPasswordNull || newPasswordRepeatNull ) ) {
				if ( newPassword.equals( newPasswordRepeat )
						&& ( oldPassword.length() >= 6 )
						&& ( newPassword.length() >= 6 ) )
					valid = true;
			}
		}
		_ok.setEnabled( valid );
	}
	public void onOk() {
		_confirmed = true;
		_configuration =
			Sec.escape( _name.getText() ) + ","
			+ Sec.escape( _email.getText() )+ ","
			+ Sec.escape( _description.getText() ) + ","
			+ Crypt.SHA1( _oldPassword.getText() ) + ","
			+ Crypt.SHA1( _newPassword.getText() ) + ","
			+ Crypt.SHA1( _newPasswordRepeat.getText() );
		dispose();
	}
	public void onCancel() {
		dispose();
	}
	boolean confirmed() {
		return ( _confirmed );
	}
	String getConfiguration() {
		return ( _configuration );
	}
}

