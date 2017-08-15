import java.util.SortedMap;
import java.util.TreeMap;
import java.io.InputStream;
import java.io.BufferedInputStream;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineListener;
import javax.sound.sampled.LineEvent;
import javax.sound.sampled.Clip;

class ClipCloser implements LineListener {
	public void update( LineEvent event ) {
		if ( ! Sound.playing() && ( event.getType() == LineEvent.Type.STOP ) )
			Sound.close();
	}
}

class Sound {
	static boolean _enabled = true;
	static Clip _clip = null;
	static ClipCloser _closer = new ClipCloser();
	static boolean _playing = false;
	private static SortedMap<String, AudioInputStream> _clipCache = java.util.Collections.synchronizedSortedMap( new TreeMap<String, AudioInputStream>() );

	static synchronized public void play( String $name ) {
		if ( _enabled ) {
			_playing = true;
			try {
				if ( _clip == null ) {
					_clip = AudioSystem.getClip();
					_clip.addLineListener( _closer );
				}
				AudioInputStream audioStream = _clipCache.get( $name );
				if ( audioStream == null ) {
					InputStream is = GameGround.class.getResourceAsStream( "/res/" + $name + ".wav" );
					BufferedInputStream bufferedInputStream = new BufferedInputStream( is );
					_clipCache.put( $name, audioStream = AudioSystem.getAudioInputStream( bufferedInputStream ) );
					audioStream.mark( 1 << 24 );
				} else
					audioStream.reset();
				if ( _clip.isRunning() ) {
					_clip.stop();
				}
				if ( _clip.isOpen() )
					_clip.close();
				_clip.open( audioStream );
				_clip.start();
			} catch ( javax.sound.sampled.LineUnavailableException lue ) {
				Con.err( "LineUnavailableException: " + lue.getMessage() );
			} catch ( javax.sound.sampled.UnsupportedAudioFileException uafe ) {
				Con.err( "UnsupportedAudioFileException: " + uafe.getMessage() );
			} catch ( java.io.IOException ioe ) {
				Con.err( "IOException: " + ioe.getMessage() );
			} catch ( java.lang.IllegalArgumentException iae ) {
				Con.err( "IllegalArgumentException: " + iae.getMessage() );
			}
			_playing = false;
		}
	}
	static void setEnabled( boolean $enabled ) {
		_enabled = $enabled;
	}
	static synchronized void close() {
		if ( ( _clip != null ) && ( _clip.isOpen() ) )
			_clip.close();
	}
	static boolean playing() {
		return ( _playing || _clip.isRunning() );
	}
}

