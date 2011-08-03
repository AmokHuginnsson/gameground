import java.lang.reflect.Method;

public class HLogicInfo {
	public String _symbol;
	public String _face;
	public String _name;
	public String _defaults;
	private Method _creator;

	public HLogicInfo( String $symbol, String $face, String $name, Method $creator ) {
		_symbol = $symbol;
		_face = $face;
		_name = $name;
		_creator = $creator;
	}
	public HAbstractLogic create( GameGround $app ) {
		HAbstractLogic logic = null;
		try {
			logic = (HAbstractLogic)_creator.invoke( null, new Object [] { $app, this } );
		} catch ( java.lang.IllegalAccessException e ) {
			e.printStackTrace();
			System.exit( 1 );
		} catch ( java.lang.reflect.InvocationTargetException e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( logic );
	}
}
