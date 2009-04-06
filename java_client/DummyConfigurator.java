import javax.swing.JPanel;

public class DummyConfigurator extends HAbstractConfigurator {
	public static final long serialVersionUID = 17l;
	GameCreator _gc;
	void setup( GameCreator $gc, String $defaults ) {
		_gc = $gc;
		setDefaults( $defaults );
	}
	String getConfigurationString() {
		return ( "" );
	}
	void setDefaults( String $defaults ) {
	}
}
