import java.util.ArrayList;
import javax.swing.JTextField;
import java.awt.event.KeyEvent;

public class ReadlinePrompt extends JTextField {
	public static final long serialVersionUID = 17l;
	ArrayList<String> _history = new ArrayList<String>();
	int _index = 0;
	public ReadlinePrompt() {
		super();
		_history.add( "" );
		addKeyListener( new java.awt.event.KeyAdapter() {
			public void keyPressed( KeyEvent e ) {
				int key = e.getKeyCode();
				if ( key == KeyEvent.VK_ENTER ) {
					String val = getText();
					if ( val.length() > 0 ) {
						_history.add( val );
						_index = _history.size();
					}
				} else if ( key == KeyEvent.VK_UP ) {
					-- _index;
					if ( _index < 0 )
						_index = _history.size() - 1;
					setText( _history.get( _index ) );
					selectAll();
				} else if ( key == KeyEvent.VK_DOWN ) {
					++ _index;
					if ( _index >= _history.size() )
						_index = 0;
					setText( _history.get( _index ) );
					selectAll();
				}
			}
		} );
	}
}

