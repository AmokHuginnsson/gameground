class CallStack {
	public static void print() {
		System.out.println("Call Stack:");
		StackTraceElement[] ste = (new Throwable()).getStackTrace();
		for (int i = 1; i < ste.length; ++ i ) {
			System.out.println( "Frame[" + i + "]: " + ste[i].getClassName() + "." + ste[i].getMethodName() + ", at line: " + ste[i].getLineNumber() + ", in: " + ste[i].getFileName() );
		}
	}
}
