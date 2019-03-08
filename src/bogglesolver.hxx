/* Read gameground/LICENSE.md file for copyright and licensing information. */

#ifndef GAMEGROUND_BOGGLESOLVER_HXX_INCLUDED
#define GAMEGROUND_BOGGLESOLVER_HXX_INCLUDED

#include <yaal/hcore/harray.hxx>
#include <yaal/hcore/hstring.hxx>
#include <yaal/hcore/hresource.hxx>

namespace gameground {

class HBoggleSolver {
public:
	typedef yaal::hcore::HArray<yaal::hcore::HString> words_t;
private:
	class HBoggleSolverImpl;
	typedef yaal::hcore::HResource<HBoggleSolverImpl> impl_t;
	impl_t _impl;
public:
	HBoggleSolver( yaal::hcore::HString const& );
	HBoggleSolver( HBoggleSolver&& ) = default;
	HBoggleSolver& operator = ( HBoggleSolver&& ) = default;
	words_t solve( yaal::hcore::HString const& ) const;
	explicit operator bool ( void ) const;
private:
	HBoggleSolver( HBoggleSolver const& ) = delete;
	HBoggleSolver& operator = ( HBoggleSolver const& ) = delete;
};

}

#endif /* not GAMEGROUND_BOGGLESOLVER_HXX_INCLUDED */

