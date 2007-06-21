#include <iostream>

#include <yaal/yaal.h>

using namespace std;
using namespace yaal;
using namespace yaal::hcore;

namespace boggle_data
{

static char const n_ppcDices[ 16 ][ 6 ] =
	{
		{ 'A', 'S', 'L', 'G', 'O', 'R' },
		{ 'O', '¯', 'S', 'M', 'C', 'T' },
		{ 'Æ', 'T', 'Z', 'M', 'V', 'J' },
		{ 'S', 'I', 'L', 'W', 'E', '¦' },
		{ 'B', 'W', 'N', 'I', 'I', 'R' },
		{ 'A', 'A', 'I', 'Y', 'M', 'K' },
		{ 'N', 'O', 'E', 'K', 'D', 'R' },
		{ 'Y', 'A', 'D', 'E', '£', 'A' },
		{ 'U', 'I', 'A', 'O', 'E', 'Y' },
		{ 'E', 'B', '¬', 'P', 'E', 'Z' },
		{ 'I', 'C', 'P', 'N', 'N', 'A' },
		{ 'O', 'K', 'R', '£', 'I', 'P' },
		{ 'G', 'T', 'M', 'C', '¡', 'Ñ' },
		{ 'Ó', 'D', 'Z', 'N', 'W', 'H' },
		{ 'Y', 'A', 'O', 'Z', 'E', 'Z' },
		{ 'S', 'F', 'H', 'J', 'L', 'Ê' }
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

int main2( int, char ** )
	{
	HBoggle boggle;
	boggle.generate_game();
	boggle.dump();
	return ( 0 );
	}
