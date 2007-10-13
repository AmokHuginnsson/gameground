import org.jdom.input.SAXBuilder;
import org.jdom.Document;
import org.xml.sax.InputSource;
import java.net.URL;

class AppletJDOMHelper {
	static private InputSource createInputSource( String $dtd, Object $for ) {
		InputSource is = null;
		try { 
			is = new InputSource( $for.getClass().getResourceAsStream( new URL( $dtd ).getFile() ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( is );
	}
	static public Document loadResource( String $resource, Object $for ) {
		SAXBuilder xmlBuilder = new SAXBuilder();
		xmlBuilder.setEntityResolver(
				new org.xml.sax.EntityResolver() {
					public InputSource resolveEntity( String $publicId, String $systemId ) {
						return ( createInputSource( $systemId, this ) );
					}
				}
			);
		Document xml = null;
		try {
			xml = xmlBuilder.build( new InputSource( $for.getClass().getResourceAsStream( $resource ) ) );
		} catch ( Exception e ) {
			e.printStackTrace();
			System.exit( 1 );
		}
		return ( xml );
	}
}

