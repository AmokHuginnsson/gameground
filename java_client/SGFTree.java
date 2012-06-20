import java.util.Arrays;
import java.io.BufferedReader;
import java.io.StringReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.awt.Image;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.Color;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.JPanel;
import javax.swing.event.MouseInputListener;

public class SGFTree extends JPanel implements MouseInputListener {
//--------------------------------------------//
	public static class PROTOCOL {
		public static final String SGF = "sgf";
	}
	public static class STONE {
		public static final byte BLACK = 'b';
		public static final byte WHITE = 'w';
		public static final String NONE_NAME = "None";
		public static final String BLACK_NAME = "Black";
		public static final String WHITE_NAME = "White";
	}
	public static final long serialVersionUID = 7l;
	HAbstractLogic _logic = null;
	GoImages _images = null;
	SGF _sgf = null;
//--------------------------------------------//
	public SGFTree() {
		addMouseMotionListener( this );
		addMouseListener( this );
	}
	void setLogic( HAbstractLogic $logic ) {
		_logic = $logic;
	}
	public void mouseClicked( MouseEvent $event ) {
	}
	public void mouseMoved( MouseEvent $event ) {
	}
	public void mouseDragged( MouseEvent $event ) {
	}
	public void mouseEntered( MouseEvent $event ) {
	}
	public void mouseReleased( MouseEvent $event ) {
	}
	public void mousePressed( MouseEvent $event ) {
	}
	public void mouseExited( MouseEvent $event ) {
	}
	public void setImages( GoImages $images ) {
		_images = $images;
	}
	protected void paintComponent( Graphics g ) {
		super.paintComponent( g );
	}
}

