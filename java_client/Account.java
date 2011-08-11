import javax.swing.JTextField;
import org.swixml.XDialog;
import org.swixml.SwingEngine;

public class Account extends XDialog {
	public static final long serialVersionUID = 17l;
	GameGround _app = null;
	boolean _confirmed = false;
	String _configuration;
	public JTextField _name;
	public JTextField _email;
	public JTextField _oldPassword;
	public JTextField _newPassword;
	public JTextField _newPasswordRepeat;
	public Account( GameGround $app ) {
		_app = $app;
		try {
			new SwingEngine( this ).insert( AppletJDOMHelper.loadResource( "/res/account.xml", this ), this );
			setVisible( true );
		} catch ( java.lang.Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void onOk() {
		_confirmed = true;
		_configuration = _name.getText() + "," + _email.getText() + ","
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

