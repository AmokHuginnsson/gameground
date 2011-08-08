import javax.swing.JPanel;

abstract class HAbstractConfigurator extends JPanel {
	public static final long serialVersionUID = 17l;
	GameCreator _gc;
	void setup( GameCreator $gc ) {
		_gc = $gc;
	}
	abstract String getConfigurationString();
	abstract void setDefaults( String $defaults );
}
