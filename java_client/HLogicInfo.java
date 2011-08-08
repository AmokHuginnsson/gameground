import java.lang.reflect.Method;
import java.util.SortedMap;
import java.util.TreeMap;

public class HLogicInfo {
	public String _symbol;
	public String _face;
	public String _name;
	public String _defaults;
	private Method _creator;
	private SortedMap<String, Party> _partys = java.util.Collections.synchronizedSortedMap( new TreeMap<String, Party>() );

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
	public int getPartysCount() {
		return ( _partys.size() );
	}
	public String toString() {
		return ( _name );
	}
}
