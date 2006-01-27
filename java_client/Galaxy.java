import java.awt.*;
import java.applet.*;
import java.awt.event.*;
import javax.swing.*;
import java.net.URL;

public class Galaxy extends JApplet implements ActionListener {
	JPanel mainPanel;
	JPanel up;
	JPanel down;
	JTextField edit0;
	JTextField edit1;
	JButton go;
	ButtonGroup radios;
	JRadioButton radio0;
	JRadioButton radio1;
	JRadioButton radio2;
	AudioClip clip;
	public static final long serialVersionUID = 13l;
	public void init() {
		radio0 = new JRadioButton( "Wilk" );
		radio1 = new JRadioButton( "Kruk" );
		radio2 = new JRadioButton( "Nied¼wie¼" );
		radios = new ButtonGroup();
		edit0 = new JTextField( 20 );
		edit1 = new JTextField( 20 );
		go = new JButton( "Go !" );
		mainPanel = new JPanel();
		up = new JPanel();
		down = new JPanel();
		
		radios.add( radio0 );
		radios.add( radio1 );
		radios.add( radio2 );
		
		mainPanel.setLayout( new BoxLayout( mainPanel, BoxLayout.Y_AXIS ) );
		mainPanel.add( up );
		mainPanel.add( down );
		up.add( radio0 );
		up.add( radio1 );
		up.add( radio2 );
		up.add( edit0 );
		up.add( edit1 );
		down.add( go );
		
		setSize( 400, 200 );
		getContentPane().add( mainPanel );
		up.setSize( 380, 100 );
		up.doLayout();

		go.addActionListener( this );
		radio0.addActionListener( this );
		radio1.addActionListener( this );
		radio2.addActionListener( this );
	}
	public void actionPerformed( ActionEvent event ) {
		Object source = event.getSource();
		if ( source == go ) {
			System.out.println( "go" );
			
			edit1.setText( edit0.getText() );
		} else if ( source.getClass().getName() == "javax.swing.JRadioButton" ) {
			System.out.println( "radio" );
			edit0.setText( ( ( JRadioButton ) source ).getText() );
		} else {
			System.out.println( source.getClass().getName() );
		}
	}
}

