#include <yaal/hcore/harray.hxx>
#include <yaal/hcore/hmap.hxx>
#include <yaal/hcore/hhashmap.hxx>
#include <yaal/hcore/math.hxx>
#include <yaal/hcore/hset.hxx>
#include <yaal/hcore/hstring.hxx>
#include <yaal/hcore/hfile.hxx>
#include <yaal/tools/hstringstream.hxx>

#include "bogglesolver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace gameground {

class HDictionaryNode {
public:
	typedef HHashMap<code_point_t, HDictionaryNode> tails_t;
private:
	bool _isWordFinished;
	tails_t _next;
public:
	HDictionaryNode()
		: _isWordFinished( false )
		, _next() {
	}

	void add_suffix( yaal::hcore::HString const& suffix, int start = 0 ) {
		if ( start >= suffix.get_length() ) {
			_isWordFinished = true;
		} else {
			_next[suffix[start]].add_suffix( suffix, start + 1 );
		}
		return;
	}

	HDictionaryNode const* get( code_point_t c ) const {
		tails_t::const_iterator it( _next.find( c ) );
		return ( it != _next.end() ? &(it->second) : nullptr );
	}

	bool is_word_finished() const {
		return ( _isWordFinished );
	}
};

class HBoard {
	struct HCube {
		typedef yaal::hcore::HArray<HCube const*> neighbors_t;
		const yaal::code_point_t _letter;
		neighbors_t _neighbors;
		mutable bool _visited;
		HCube( code_point_t letter_ )
			: _letter( letter_ )
			, _neighbors()
			, _visited( false ) {
		}
	};
public:
	typedef yaal::hcore::HArray<HCube> cubes_t;
	typedef yaal::hcore::HSet<yaal::hcore::HString> words_t;
private:
	int _size;
	cubes_t _cubes;
public:
	HBoard( yaal::hcore::HString const& board )
		: _size( 0 )
		, _cubes() {
		for ( code_point_t letter : board ) {
			if ( letter != '\n' ) {
				_cubes.emplace_back( letter );
			}
		}
		_size = static_cast<int>( math::square_root( _cubes.size() ) );
		if ( ( _size * _size ) != static_cast<int>( _cubes.size() ) ) {
			throw HRuntimeException("board is not a square");
		}
		typedef yaal::hcore::HPair<int, int> direction_t;
		typedef yaal::hcore::HArray<direction_t> directions_t;
		static directions_t const neighborhood {
			{ -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }
		};
		for ( int y = 0; y < _size; ++y ) {
			for ( int x = 0; x < _size; ++x ) {
				for ( auto&& delta : neighborhood ) {
					int nx = x + delta.first, ny = y + delta.second;
					if (nx >= 0 && nx < _size && ny >= 0 && ny < _size) {
						get_cube(x, y)._neighbors.push_back(&get_cube(nx, ny));
					}
				}
			}
		}
	}

	yaal::hcore::HString print( void ) const {
		HStringStream out;
		for (int y = 0; y < _size; ++y) {
			for (int x = 0; x < _size; ++x) {
				out << "+---";
			}
			out << "\n";
			for (int x = 0; x < _size; ++x) {
				out << "| " << get_cube(x, y)._letter << " ";
			}
			out << "\n";
		}
		return out.str();
	}

	HBoggleSolver::words_t solve( HDictionaryNode const* dictionary ) const {
		words_t words;
		for ( HCube const& cube : _cubes ) {
			HString word;
			solve( words, word, cube, dictionary );
		}
		HBoggleSolver::words_t sorted( words.begin(), words.end() );
		yaal::sort(
			sorted.begin(),
			sorted.end(),
			[]( yaal::hcore::HString const& left_, yaal::hcore::HString const right ) {
				return ( left_.get_size() < right.get_size() );
			}
		);
		return sorted;
	}

private:
	HCube& get_cube( int x, int y ) {
		return _cubes[y * _size + x];
	}

	HCube const& get_cube( int x, int y ) const {
		return _cubes[y * _size + x];
	}

	void solve( words_t& ret, yaal::hcore::HString& word, HCube const& cube, HDictionaryNode const* node ) const {
		HDictionaryNode const* next( node->get( cube._letter ) );
		if ( next == nullptr ) {
			return;
		}
		cube._visited = true;
		word.push_back( cube._letter );
		if ( next->is_word_finished() && word.get_size() >= 3 ) {
			ret.insert( word );
		}
		for ( HCube const* neighbor : cube._neighbors ) {
			if ( !neighbor->_visited ) {
				solve( ret, word, *neighbor, next );
			}
		}
		word.pop_back();
		cube._visited = false;
	}
};

class HBoggleSolver::HBoggleSolverImpl {
private:
	HDictionaryNode _root;
public:
	HBoggleSolverImpl( void )
		: _root() {
	}
	void init( yaal::hcore::HString const& path_ ) {
		M_PROLOG
		HFile dictFile( path_, HFile::OPEN::READING );
		if ( !dictFile ) {
			throw HRuntimeException( dictFile.get_error() );
		}
		HString line;
		int wordCount( 0 );
		try {
			while ( dictFile.read_line( line, HFile::READ::BUFFERED_READS ).good() ) {
				_root.add_suffix( line );
				++ wordCount;
			}
		} catch ( HException const& e ) {
			throw HRuntimeException( to_string( e.what() ).append( " at line: " ).append( wordCount ) );
		}
		cerr << "[OK] HBoggleSolver Ready (" << wordCount << " words loaded)" << endl;
		return;
		M_EPILOG
	}
	HBoggleSolver::words_t solve( yaal::hcore::HString const& board_ ) const {
		M_PROLOG
		HBoard board( board_ );
		return ( board.solve( &_root ) );
		M_EPILOG
	}
};

HBoggleSolver::HBoggleSolver( yaal::hcore::HString const& path_ )
	: _impl() {
	M_PROLOG
	try {
		impl_t impl( make_resource<HBoggleSolverImpl>() );
		impl->init( path_ );
		_impl = yaal::move( impl );
	} catch ( HException const& e ) {
		cerr << "[FAIL] HBoggleSolver failed to initialize: " << e.what() << endl;
	}
	return;
	M_EPILOG
}

HBoggleSolver::words_t HBoggleSolver::solve( yaal::hcore::HString const& board_ ) const {
	M_PROLOG
	return ( _impl->solve( board_ ) );
	M_EPILOG
}

HBoggleSolver::operator bool( void ) const {
	return ( !! _impl );
}

}

