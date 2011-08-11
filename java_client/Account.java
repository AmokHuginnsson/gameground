import javax.swing.JTextField;
import javax.swing.JTextArea;
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
	public JTextArea _description;
	public Account( GameGround $app, String $conf ) {
		_app = $app;
		String[] tokens = $conf.split( ",", 3 );
		try {
			new SwingEngine( this ).insert( AppletJDOMHelper.loadResource( "/res/account.xml", this ), this );
			_name.setText( tokens[0] );
			_email.setText( tokens[1] );
			_description.setText( tokens[2] );
			setVisible( true );
		} catch ( java.lang.Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
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

