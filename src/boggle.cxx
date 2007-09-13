#include <iostream>

#include <yaal/yaal.h>

using namespace std;
using namespace yaal;
using namespace yaal::hcore;

namespace boggle_data
{

static char const n_ppcDices[ 16 ][ 6 ] =
	{
		{ 'A', 'A', 'D', 'E', '£', 'Y' },
		{ 'A', 'A', 'I', 'K', 'M', 'Y' },
		{ 'A', 'C', 'I', 'N', 'N', 'P' },
		{ 'A', 'E', 'I', 'O', 'U', 'Y' },
		{ 'A', 'E', 'O', 'Y', 'Z', 'Z' },
		{ 'A', 'G', 'L', 'O', 'R', 'S' },
		{ '¡', 'C', 'G', 'M', 'Ñ', 'T' },
		{ 'B', 'E', 'E', 'P', 'Z', '¬' },
		{ 'B', 'I', 'I', 'N', 'R', 'W' },
		{ 'C', 'M', 'O', 'S', 'T', '¯' },
		{ 'Æ', 'J', 'M', 'T', 'V', 'Z' },
		{ 'D', 'E', 'K', 'N', 'O', 'R' },
		{ 'D', 'H', 'Ó', 'N', 'W', 'Z' },
		{ 'E', 'I', 'L', 'S', '¦', 'W' },
		{ 'Ê', 'F', 'H', 'J', 'L', 'S' },
		{ 'I', 'K', '£', 'O', 'P', 'R' }
	};

struct BOGGLE
	{
	enum
		{
		UNINITIALIZED_SLOT = -1,
		SIDES = 6,
		DICE_COUNT = 16
		};
	};

}

class HBoggle
	{
	int f_ppiGame[16][2];
public:
	void generate_game( void );
	void dump( void );
	};

void HBoggle::generate_game( void )
	{
	HRandomizer rnd;
	randomizer_helper::init_randomizer_from_time( rnd );
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		f_ppiGame[ i ][ 0 ] = boggle_data::BOGGLE::UNINITIALIZED_SLOT;
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		{
		int k = 0, l_iSlot = rnd.rnd( boggle_data::BOGGLE::DICE_COUNT - i );
		for ( int j = 0; j < l_iSlot; ++ j, ++ k )
			while ( f_ppiGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
				++ k;
		while ( f_ppiGame[ k ][ 0 ] != boggle_data::BOGGLE::UNINITIALIZED_SLOT )
			++ k;
		f_ppiGame[ k ][ 0 ] = i;
		f_ppiGame[ k ][ 1 ] = rnd.rnd( boggle_data::BOGGLE::SIDES );
		}
	return;
	}

void HBoggle::dump( void )
	{
	for ( int i = 0; i < boggle_data::BOGGLE::DICE_COUNT; ++ i )
		{
		if ( i && ! ( i % 4 ) )
			cout << endl;
		cout << boggle_data::n_ppcDices[ f_ppiGame[ i ][ 0 ] ][ f_ppiGame[ i ][ 1 ] ];
		}
	cout << endl;
	return;
	}

int create_game( int, char ** )
	{
	HBoggle boggle;
	boggle.generate_game();
	boggle.dump();
	return ( 0 );
	}
