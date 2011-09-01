import java.util.SortedMap;
import java.util.TreeMap;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.SwingUtilities;
import java.awt.Component;
import java.awt.event.FocusEvent;
import javax.swing.Action;
import javax.swing.JFrame;
import javax.swing.AbstractAction;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;
import javax.swing.AbstractListModel;
import javax.swing.ListModel;
import java.awt.event.ActionEvent;
import org.swixml.XDialog;
import org.swixml.SwingEngine;

public class GameCreator extends XDialog implements ListSelectionListener, DocumentListener {
	public static final long serialVersionUID = 17l;
	GameGround _app = null;
	boolean _confirmed = false;
	String _configuration;
	HLogicInfo _info;
	HAbstractConfigurator _conf;
	public JPanel _confHolder;
	public JList _logics;
	public JTextField _name;
	public JButton _ok;
	public GameCreator( GameGround $app, final SortedMap<String, HLogicInfo> $logics, String $logic ) {
		super();
		_app = $app;
		try {
			new SwingEngine( this ).insert( AppletJDOMHelper.loadResource( "/res/creator.xml", this ), this );
			_logics.setModel( new AbstractListModel() {
				public static final long serialVersionUID = 17l;
				public int getSize() {
					java.util.Set<java.util.Map.Entry<String,HLogicInfo>> entSet = $logics.entrySet();
					java.util.Map.Entry<String,HLogicInfo> ent = null;
					java.util.Iterator<java.util.Map.Entry<String,HLogicInfo>> it = entSet.iterator();
					int i = 0;
					while ( it.hasNext() ) {
						ent = it.next();
						if ( ent != null ) {
							HLogicInfo li = ent.getValue();
							if ( ! li._private )
								++ i;
						}
					}
					return ( i );
				}
				public Object getElementAt( int index ) {
					java.util.Set<java.util.Map.Entry<String,HLogicInfo>> entSet = $logics.entrySet();
					java.util.Map.Entry<String,HLogicInfo> ent = null;
					java.util.Iterator<java.util.Map.Entry<String,HLogicInfo>> it = entSet.iterator();
					int i = 0;
					while ( it.hasNext() ) {
						ent = it.next();
						if ( ent != null ) {
							HLogicInfo li = ent.getValue();
							if ( ! li._private ) {
								if ( i == index ) {
									break;
								}
								++ i;
							}
						}
					}
					return ( ent.getValue() );
				}
			} );
			_logics.addListSelectionListener( this );
			_name.getDocument().addDocumentListener( this );
			getRootPane().setDefaultButton( _ok );
			if ( $logic != null ) {
				ListModel lm = _logics.getModel();
				int logicsCount = lm.getSize();
				for ( int i = 0; i < logicsCount; ++ i ) {
					if ( lm.getElementAt( i ).toString().equals( $logic ) ) {
						_logics.setSelectedIndex( i );
						_name.setText( _app.getName() + "'s room ..." );
						changeFocus( _name );
						_name.selectAll();
						break;
					}
				}
			} else
				changeFocus( _logics );
			setVisible( true );
		} catch ( java.lang.Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	private void changeFocus( final Component target ) {
		SwingUtilities.invokeLater( new Runnable() {
			public void run() {
				target.dispatchEvent(
					new FocusEvent( target, FocusEvent.FOCUS_GAINED ) );
			}
		} );
	}
	public void changedUpdate( DocumentEvent e ) {
		setEnabledOk();
	}
	public void insertUpdate( DocumentEvent e ) {
		setEnabledOk();
	}
	public void removeUpdate( DocumentEvent e ) {
		setEnabledOk();
	}
	public void valueChanged( ListSelectionEvent e ) {
		if ( ! e.getValueIsAdjusting() ) {
			int idx = _logics.getSelectedIndex();
			if ( _conf != null ) {
				_conf.setVisible( false );
				_conf = null;
			}
			_confHolder.removeAll();
			if ( idx >= 0 ) {
				_info = (HLogicInfo)_logics.getModel().getElementAt( idx );
				_conf = _info._conf;
				if ( _conf != null ) {
					_conf.setVisible( false );
					_confHolder.add( _conf );
					_conf.setVisible( true );
					_conf.setup( this );
				} else {
					_configuration = _info._defaults;
				}
			}
		setEnabledOk();
		}
	}
	void setEnabledOk() { setEnabledOk( true ); }
	void setEnabledOk( boolean $enabled ) {
		_ok.setEnabled( $enabled && ( _logics.getSelectedIndex() >= 0 ) && ( "".compareTo( _name.getText() ) != 0 ) );
	}
	public void onOk() {
		_confirmed = true;
		if ( _conf != null )
			_configuration = _info._symbol + ":" + _name.getText() + "," + _conf.getConfigurationString();
		else
			_configuration = _info._symbol + ":" + _name.getText() + "," + _configuration;
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
}

