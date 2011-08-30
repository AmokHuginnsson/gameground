public class DurationFormat {
	public static String toString( long $duration ) {
		long durationS = Math.abs( $duration ) / 1000;
		long durationM = durationS / 60;
		long durationH = durationM / 60;
		long durationD = durationH / 24;
		return ( String.format( "%s%d %02d:%02d:%02d", $duration >= 0 ? "" : "-", durationD, durationH % 24, durationM % 60, durationS % 60 ) );
	}
}
