class PlayerScore {
	String _name;
	int[] _score = new int[2];
	PlayerScore( String $name ) {
		this( $name, 0, 0 );
	}
	PlayerScore( String $name, int $score0 ) {
		this( $name, $score0, 0 );
	}
	PlayerScore( String $name, int $score0, int $score1 ) {
		_name = $name;
		_score[0] = $score0;
		_score[1] = $score1;
	}
	void set( String $name, int $score0 ) {
		_name = $name;
		_score[0] = $score0;
	}
	void set( String $name, int $score0, int $score1 ) {
		_name = $name;
		_score[0] = $score0;
		_score[1] = $score1;
	}
	String getName() {
		return ( _name );
	}
	String get( int $field ) {
		String field = "";
		switch ( $field ) {
			case ( 0 ):
				field = _name;
			break;
			default:
				field += _score[$field-1];
			break;
		}
		return ( field );
	}
}
