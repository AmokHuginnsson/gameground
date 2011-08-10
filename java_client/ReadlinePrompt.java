import java.util.Vector;
import javax.swing.JTextField;

public class ReadlinePrompt extends JTextField {
	public static final long serialVersionUID = 17l;
	Vector<String> _history = new Vector<String>();
	int _index = 0;
	public ReadlinePrompt() {
		super();
		_history.add( "" );
		addKeyListener( new java.awt.event.KeyAdapter() {
			public void keyPressed( java.awt.event.KeyEvent e ) {
				int key = e.getKeyCode();
				if ( key == e.VK_ENTER ) {
					String val = getText();
					if ( val.length() > 0 ) {
						_history.add( val );
						_index = _history.size();
					}
				} else if ( key == e.VK_UP ) {
					-- _index;
					if ( _index < 0 )
						_index = _history.size() - 1;
					setText( _history.elementAt( _index ) );
					selectAll();
				} else if ( key == e.VK_DOWN ) {
					++ _index;
					if ( _index >= _history.size() )
						_index = 0;
					setText( _history.elementAt( _index ) );
					selectAll();
				}
			}
		} );
	}
}

