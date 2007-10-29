import java.util.Vector;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;
import org.swixml.XDialog;
import org.swixml.SwingEngine;

public class GameCreator extends XDialog implements ListSelectionListener {
	public static final long serialVersionUID = 17l;
	boolean _ok = false;
	HAbstractConfigurator _conf;
	public JPanel _confHolder;
	public JList _logics;
	public GameCreator( Vector<HPlayerSet> $logics ) {
		try {
			new SwingEngine( this ).insert( AppletJDOMHelper.loadResource( "/res/creator.xml", this ), this );
			System.out.println( "dialogInit: " + $logics );
			_logics.setListData( $logics );
			_logics.addListSelectionListener( this );
			setVisible( true );
		} catch ( java.lang.Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	public void valueChanged( ListSelectionEvent e ) {
		if ( ! e.getValueIsAdjusting() ) {
			int idx = _logics.getSelectedIndex();
			_confHolder.removeAll();
			if ( idx >= 0 ) {
				HPlayerSet item = (HPlayerSet)_logics.getModel().getElementAt( idx );
				HAbstractLogic l = GameGround.getInstance().getLogicBySymbol( item._id );
				_conf = l._gui.getConfigurator();
				assert _conf != null : _conf;
				_conf.setVisible( false );
				_confHolder.add( _conf );
				_conf.setVisible( true );
			}
		}
	}
	public void onOk() {
		_ok = true;
		dispose();
	}
	public void onCancel() {
		dispose();
	}
	boolean confirmed() {
		return ( _ok );
	}
	protected void processWindowEvent( java.awt.event.WindowEvent e ) {
		if ( e.getID() == java.awt.event.WindowEvent.WINDOW_CLOSED ) {
			if ( _conf != null )
				_conf.setVisible( false );
		}
		super.processWindowEvent( e );
	}
}
