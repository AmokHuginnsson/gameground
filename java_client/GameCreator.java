import java.util.Vector;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;
import java.awt.event.ActionEvent;
import org.swixml.XDialog;
import org.swixml.SwingEngine;

public class GameCreator extends XDialog implements ListSelectionListener, DocumentListener {
	public static final long serialVersionUID = 17l;
	boolean _confirmed = false;
	String _configuration;
	String _face;
	HPlayerSet _playerSet;
	HAbstractConfigurator _conf;
	public JPanel _confHolder;
	public JList _logics;
	public JTextField _name;
	public JButton _ok;
	public GameCreator( Vector<HPlayerSet> $logics ) {
		try {
			new SwingEngine( this ).insert( AppletJDOMHelper.loadResource( "/res/creator.xml", this ), this );
			System.out.println( "dialogInit: " + $logics );
			_logics.setListData( $logics );
			_logics.addListSelectionListener( this );
			_name.getDocument().addDocumentListener( this );
			setVisible( true );
		} catch ( java.lang.Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public Action onEditChange = new AbstractAction() {
		public static final long serialVersionUID = 17l;
		public void actionPerformed( ActionEvent $event ) {
			if ( ( _conf != null ) && ( "".compareTo( _name.getText() ) != 0 ) )
				onOk();
		}
	};
	public void changedUpdate( DocumentEvent e ) {
		setEnabledOk();
	}
	public void insertUpdate (DocumentEvent e ) {
		setEnabledOk();
	}
	public void removeUpdate( DocumentEvent e ) {
		setEnabledOk();
	}
	public void valueChanged( ListSelectionEvent e ) {
		if ( ! e.getValueIsAdjusting() ) {
			int idx = _logics.getSelectedIndex();
			_confHolder.removeAll();
			if ( idx >= 0 ) {
				_playerSet = (HPlayerSet)_logics.getModel().getElementAt( idx );
				HAbstractLogic l = GameGround.getInstance().getLogicBySymbol( _playerSet._id );
				_conf = l._gui.getConfigurator();
				_face = l.getInfo()._face;
				assert _conf != null : _conf;
				_conf.setVisible( false );
				_confHolder.add( _conf );
				_conf.setVisible( true );
				_conf.setup( this, _playerSet._configuration );
			} else {
				_conf.setVisible( false );
				_conf = null;
				_playerSet = null;
				_face = "";
			}
		setEnabledOk();
		}
	}
	void setEnabledOk() { setEnabledOk( true ); }
	void setEnabledOk( boolean $enabled ) {
		_ok.setEnabled( $enabled && ( _conf != null ) && ( "".compareTo( _name.getText() ) != 0 ) );
	}
	public void onOk() {
		_confirmed = true;
		_configuration = _playerSet._id + ":" + _name.getText() + "," + _conf.getConfigurationString();
		dispose();
	}
	public void onCancel() {
		dispose();
	}
	boolean confirmed() {
		return ( _confirmed );
	}
	protected void processWindowEvent( java.awt.event.WindowEvent e ) {
		if ( e.getID() == java.awt.event.WindowEvent.WINDOW_CLOSED ) {
			if ( _conf != null )
				_conf.setVisible( false );
		}
		super.processWindowEvent( e );
	}
	String getConfiguration() {
		return ( _configuration );
	}
	String getFace() {
		return ( _face );
	}
}
