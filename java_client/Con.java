public class Con {
	public static String bold = "\033[1m";
	public static String reset = "\033[0m";
	public static String black = "\033[0;30m";
	public static String red = "\033[0;31m";
	public static String green = "\033[0;32m";
	public static String brown = "\033[0;33m";
	public static String blue = "\033[0;34m";
	public static String magenta = "\033[0;35m";
	public static String cyan = "\033[0;36m";
	public static String lightgray = "\033[0;37m";
	public static String gray = "\033[0;90m";
	public static String brightred = "\033[0;91m";
	public static String brightgreen = "\033[0;92m";
	public static String yellow = "\033[0;93m";
	public static String brightblue = "\033[0;94m";
	public static String brightmagenta = "\033[0;95m";
	public static String brightcyan = "\033[0;96m";
	public static String white = "\033[0;97m";
	static public void err( String $msg ) {
		System.err.println( red + $msg + reset );
	}
	static public void stack( String $msg ) {
		System.err.println( yellow + $msg + reset );
	}
	static public void debug( String $msg ) {
		System.err.println( brown + $msg + reset );
	}
	static public void info( String $msg ) {
		System.err.println( cyan + $msg + reset );
	}
	static public void print( String $msg ) {
		System.out.println( reset + $msg + reset );
	}
}

