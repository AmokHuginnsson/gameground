import java.io.InputStream;
import java.io.BufferedInputStream;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineListener;
import javax.sound.sampled.LineEvent;
import javax.sound.sampled.Clip;

class ClipCloser implements LineListener {
	Clip _clip = null;
	ClipCloser( Clip $clip ) {
		_clip = $clip;
	}
	public void update( LineEvent event ) {
		if ( event.getType() == LineEvent.Type.STOP )
			_clip.close();
	}
}

class Sound {
	static public void play( String $name ) {
		try {
			Clip clip = AudioSystem.getClip();
			InputStream is = GameGround.class.getResourceAsStream( "/res/" + $name + ".wav" );
			BufferedInputStream bufferedInputStream = new BufferedInputStream( is );
			AudioInputStream audioStream = AudioSystem.getAudioInputStream( bufferedInputStream );
			clip.open( audioStream );
			clip.setFramePosition( 0 );
			clip.loop( 0 );
			clip.addLineListener( new ClipCloser( clip ) );
			clip.start(); 
		} catch ( javax.sound.sampled.LineUnavailableException lue ) {
			Con.err( "LineUnavailableException: " + lue.getMessage() );
		} catch ( javax.sound.sampled.UnsupportedAudioFileException uafe ) {
			Con.err( "UnsupportedAudioFileException: " + uafe.getMessage() );
		} catch ( java.io.IOException ioe ) {
			Con.err( "IOException: " + ioe.getMessage() );
		}
	}
}

