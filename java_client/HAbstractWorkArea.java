public abstract class HAbstractWorkArea {
	GameGround _app = null;
	HGUIface _gui = null;
	public HAbstractWorkArea( GameGround $app ) {
		_app = $app;
	}
	public void init( HGUIface $gui ) {
		_gui = $gui;
		_gui.init();
	}
	public HGUIface getGUI() {
		return ( _gui );
	}
	public abstract void reinit();
	public abstract void cleanup();
}
