import java.io.InputStream;
import java.io.BufferedInputStream;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;

class Sound {
	static public void play( String $name ) {
		try {
			Clip clip = AudioSystem.getClip();
			InputStream is = GameGround.class.getResourceAsStream( "/res/" + $name + ".wav" );
			BufferedInputStream bufferedInputStream = new BufferedInputStream( is );
			AudioInputStream audioStream = AudioSystem.getAudioInputStream( bufferedInputStream );
			clip.open( audioStream );
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

