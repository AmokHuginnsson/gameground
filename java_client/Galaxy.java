import java.awt.*;
import java.applet.*;
import java.awt.event.*;
import javax.swing.*;
import java.net.URL;

class HGUIMain extends JPanel implements ActionListener {
	JPanel _mainPanel;
	JPanel _up;
	JPanel _down;
	JTextField _edit0;
	JTextField _edit1;
	JButton go;
	ButtonGroup _radios;
	JRadioButton _radio0;
	JRadioButton _radio1;
	JRadioButton _radio2;
	AudioClip clip;
	public HGUIMain(String label) {
		_radio0 = new JRadioButton( "Wilk" );
		_radio1 = new JRadioButton( "Kruk" );
		_radio2 = new JRadioButton( "Nied¼wie¼" );
		_radios = new ButtonGroup();
		_edit0 = new JTextField( 20 );
		_edit1 = new JTextField( 20 );
		go = new JButton( "Go !" );
		_mainPanel = new JPanel();
		_up = new JPanel();
		_down = new JPanel();
		
		_radios.add( _radio0 );
		_radios.add( _radio1 );
		_radios.add( _radio2 );
		
		_mainPanel.setLayout( new BoxLayout( _mainPanel, BoxLayout.Y_AXIS ) );
		_mainPanel.add( _up );
		_mainPanel.add( _down );
		_up.add( _radio0 );
		_up.add( _radio1 );
		_up.add( _radio2 );
		_up.add( _edit0 );
		_up.add( _edit1 );
		_down.add( go );
		
		setSize( 400, 200 );
		add( _mainPanel );
		_up.setSize( 380, 100 );
		_up.doLayout();

		go.addActionListener( this );
		_radio0.addActionListener( this );
		_radio1.addActionListener( this );
		_radio2.addActionListener( this );
	}
	public void actionPerformed( ActionEvent $event ) {
		Object source = $event.getSource();
		if ( source == go ) {
			System.out.println( "go" );		
			_edit1.setText( _edit0.getText() );
		} else if ( source.getClass().getName() == "javax.swing.JRadioButton" ) {
			System.out.println( "radio" );
			_edit0.setText( ( ( JRadioButton ) source ).getText() );
		} else {
			System.out.println( source.getClass().getName() );
		}
	}
}

public class /* Application or applet name: */ Galaxy extends JApplet {
	public static final long serialVersionUID = 13l;
	private static JFrame _frame;

	public void init() {
		add( new HGUIMain ("Push Me") );
	} 

	static public void main (String argv[]) {
		_frame = new JFrame();
		_frame.add( new HGUIMain( "Push Me" ) );
		_frame.pack();
		_frame.setVisible( true );
	}
}


