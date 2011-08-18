class CallStack {
	public static void print() {
		Con.err("Call Stack:");
		StackTraceElement[] ste = (new Throwable()).getStackTrace();
		for (int i = 1; i < ste.length; ++ i ) {
			Con.stack( "Frame[" + i + "]: " + ste[i].getClassName() + "." + ste[i].getMethodName() + ", at line: " + ste[i].getLineNumber() + ", in: " + ste[i].getFileName() );
		}
	}
}
