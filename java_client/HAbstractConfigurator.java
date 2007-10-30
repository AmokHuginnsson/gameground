import javax.swing.JPanel;

abstract class HAbstractConfigurator extends JPanel {
	public static final long serialVersionUID = 17l;
	GameCreator _gc;
	void setup( GameCreator $gc, String $defaults ) {
		_gc = $gc;
		setDefaults( $defaults );
	}
	abstract String getConfigurationString();
	abstract void setDefaults( String $defaults );
}
