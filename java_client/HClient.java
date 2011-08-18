import java.util.concurrent.Semaphore;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.PrintStream;
import java.net.Socket;
import javax.net.SocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import javax.net.ssl.SSLContext;

class HClient extends Thread {
/*--------------------------------------------*/
	private static SSLContext _sslContext;
	private static final boolean registered;
	private static final String D_ENCODING = "iso8859-2";
	static {
		registered = relaxSSL();
	}
	GameGround _app = null;
	boolean _loop;
	BufferedReader _in;
	PrintStream _out;
	Socket _socket;
	Semaphore _running = new Semaphore( 0 );
/*--------------------------------------------*/
	public HClient( GameGround $app ) throws Exception {
		_app = $app;
		_loop = true;
	}
	void connect( String $server, int $port ) throws Exception {
		SocketFactory sf = _sslContext.getSocketFactory();
		_socket = sf.createSocket( $server, $port );
		_out = new PrintStream( _socket.getOutputStream(), true, D_ENCODING );
		_in = new BufferedReader( new InputStreamReader( _socket.getInputStream(), D_ENCODING ) );
	}
	void println( String $line ) {
		_out.print( $line + "\n" );
	}

	public void waitUntilRunning() throws java.lang.InterruptedException {
		_running.acquire();
	}

	public void run() {
		System.out.println( "Awaiting for finish of the initialization." );
		_running.release();
		try {
			System.out.println( "Main client loop." );
			String message = "";
			while ( _loop && ( message = _in.readLine() ) != null ) {
				if ( message.length() > 0 ) {
					System.out.println( "-> " + Con.blue + message + Con.reset );
					_app.processMessage( message );
				} else {
					System.out.println( "Empty message." );
				}
			}
			if ( _loop ) {
				javax.swing.JOptionPane.showMessageDialog( _app._frame,
						"The connection was closed by the GameGround server.",
						"GameGround - read error ...", javax.swing.JOptionPane.ERROR_MESSAGE );
				_app.setClient( null );
				_app.showLoginScreen();
			}
		} catch ( javax.net.ssl.SSLException sslEx ) {
			_app.setClient( null );
			_app.showLoginScreen();
		} catch ( java.io.IOException e ) {
			if ( _loop ) {
				Con.err( "Connection error." );
				e.printStackTrace();
				System.exit( 0 );
			}
		}
		if ( ! _loop )
			System.out.println( "Graceful disconnect." );
		return;
	}

	public void disconnect() {
		Sound.play( "service-logout" );
		_loop = false;
		_app.setClient( null );
		_app.showLoginScreen();
		try {
			_socket.close();
		} catch ( java.io.IOException e ) {
		}
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
						java.security.cert.X509Certificate[] certs, String authType ) {
				}
				public void checkServerTrusted(
						java.security.cert.X509Certificate[] certs, String authType ) {
				}
			}
		};

		/*
		 * Install the all-trusting trust manager
		 */
		try {
			_sslContext = SSLContext.getInstance( "SSL" );
			_sslContext.init( null, trustAllCerts, new java.security.SecureRandom() );
		} catch (Exception e) {
		}
		return ( true );
	}

}

