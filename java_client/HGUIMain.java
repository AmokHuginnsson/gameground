import java.io.InputStreamReader;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.Container;
import org.swixml.SwingEngine;
import javax.swing.*;

class HGUIMain extends JPanel implements ActionListener {
	public static final long serialVersionUID = 17l;
	public class HWidgets extends SwingEngine {
		/* This is one dirty hack.
		 * To make it work you have to edit org/swixml/SwingEngine.java
		 * and comment out all setAccessible() invocations.
		 */
		public JTextField _edit0;
		public JTextField _edit1;
		public JButton _go;
		public JRadioButton _radio0;
		public JRadioButton _radio1;
		public JRadioButton _radio2;
		public JRadioButton _active;
	}
	public HWidgets _widgets;
	public HGUIMain() throws Exception {
		_widgets = new HWidgets();
		_widgets.insert( new InputStreamReader( HGUIMain.class.getResourceAsStream( "galaxy.xml" ) ), this );
		_widgets._go.addActionListener( this );
		_widgets._radio0.addActionListener( this );
		_widgets._radio1.addActionListener( this );
		_widgets._radio2.addActionListener( this );
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( ( _widgets._active != null ) && ( source == _widgets._go ) ) {
			_widgets._edit1.setText( _widgets._active.getText() );
		} else if ( source.getClass().getName() == "javax.swing.JRadioButton" ) {
			_widgets._active = ( JRadioButton ) source;
			_widgets._edit0.setText( _widgets._active.getText() );
		}
	}
}

