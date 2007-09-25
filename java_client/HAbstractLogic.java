import java.lang.reflect.Method;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Collections;

interface HLogic {
	public SortedMap<String, Method> getHandlers();
	void handlerMessage( String $message );
}

public abstract class HAbstractLogic implements HLogic {
	SortedMap<String,Method> _handlers;
	public SortedMap<String, Method> getHandlers() {
		return ( _handlers );
	}
}
