import java.lang.reflect.Method;
import java.lang.Class;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Collections;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.net.Socket;
import javax.net.SocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocketFactory;

class HClient extends Thread {
/*--------------------------------------------*/
	private static SSLContext _sslContext;
	private static final boolean registered;
	static {
		registered = relaxSSL();
	}
	boolean _loop;
	HAbstractLogic _logic;
	BufferedReader _in;
	PrintWriter _out;
	Socket _socket;
/*--------------------------------------------*/
	public HClient() throws Exception {
		_loop = true;
		System.out.println( "HClient" );
	}
	void connect( String $server, int $port ) throws Exception {
		SocketFactory sf = _sslContext.getSocketFactory();
		_socket = sf.createSocket( $server, $port );
		_out = new PrintWriter( _socket.getOutputStream(), true );
		_in = new BufferedReader( new InputStreamReader( _socket.getInputStream() ) );
		System.out.println( "connect" );
	}
	void setLogic( HAbstractLogic $logic ) {
		_logic = $logic;
	}
	void println( String $line ) {
		_out.println( $line );
	}

	public void run() {
		System.out.println( "Awaiting for finish of the initialization." );
		synchronized( this ) {
			notify();
		}
		try {
			System.out.println( "Main client loop." );
			String message = "";
			String command;
			String argument;
			String[] tokens = new String[2];
			while ( _loop && ( message = _in.readLine() ) != null ) {
				if ( message.length() > 0 ) {
					System.out.println( message );
					_logic.processMessage( message );
				} else {
					System.out.println( "Empty message." );
				}
			}
			if ( _loop ) {
				GameGround gg = GameGround.getInstance();
				javax.swing.JOptionPane.showMessageDialog( gg._frame,
						"The connection was closed by the GameGround server.",
						"GameGround - read error ...", javax.swing.JOptionPane.ERROR_MESSAGE );
				gg.setClient( null );
				gg.setFace( HLogin.LABEL );
			} else {
				System.out.println( "Graceful disconnect." );
			}
		} catch ( java.io.IOException e ) {
			System.out.println( "Connection error." );
			e.printStackTrace();
			System.exit( 0 );
		}
	}

	public void disconnect() {
		_loop = false;
		GameGround.getInstance().setClient( null );
	}

	private static boolean relaxSSL() {
		/*
		 * Create a trust manager that does not validate certificate chains
		 */
		TrustManager[] trustAllCerts = new TrustManager[] {
			new X509TrustManager() {
				public java.security.cert.X509Certificate[] getAcceptedIssuers() {
					return null;
				}
				public void checkClientTrusted(
						java.security.cert.X509Certificate[] certs, String authType) {
						}
				public void checkServerTrusted(
						java.security.cert.X509Certificate[] certs, String authType) {
						}
			}
		};

		/*
		 * Install the all-trusting trust manager
		 */
		try {
			_sslContext = SSLContext.getInstance("SSL");
			_sslContext.init(null, trustAllCerts, new java.security.SecureRandom());
		} catch (Exception e) {
		}
		return ( true );
	}

}

