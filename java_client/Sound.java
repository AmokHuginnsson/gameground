import java.io.InputStream;
import java.io.BufferedInputStream;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineListener;
import javax.sound.sampled.LineEvent;
import javax.sound.sampled.Clip;

class ClipCloser implements LineListener {
	public void update( LineEvent event ) {
		if ( event.getType() == LineEvent.Type.STOP )
			Sound.close();
	}
}

class Sound {
	static Clip _clip = null;
	static ClipCloser _closer = new ClipCloser();
	static synchronized public void play( String $name ) {
		try {
			if ( _clip == null ) {
				_clip = AudioSystem.getClip();
				_clip.addLineListener( _closer );
			}
			InputStream is = GameGround.class.getResourceAsStream( "/res/" + $name + ".wav" );
			BufferedInputStream bufferedInputStream = new BufferedInputStream( is );
			AudioInputStream audioStream = AudioSystem.getAudioInputStream( bufferedInputStream );
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
		}
	}
	static synchronized void close() {
		if ( ( _clip != null ) && ( _clip.isOpen() ) )
			_clip.close();
	}
}

