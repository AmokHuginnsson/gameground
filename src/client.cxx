/* Read gameground/LICENSE.md file for copyright and licensing information. */

#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/hexception.hxx>

M_VCSID( "$Id: " __ID__ " $" )
#include "client.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace gameground {

HClient::HClient( yaal::hcore::HStreamInterface::ptr_t const& sock_ )
	: _valid( true )
	, _authenticated( false )
	, _login()
	, _socket( sock_ )
	, _logics() {
}

HClient::~HClient( void ) {
	M_PROLOG
	M_ASSERT( _logics.is_empty() );
	return;
	M_EPILOG
}

void HClient::send( yaal::hcore::HString const& message_ ) {
	M_PROLOG
	*_socket << message_;
	return;
	M_EPILOG
}

void HClient::read( yaal::hcore::HString& line_ ) {
	M_PROLOG
	_socket->read_until( line_ );
	return;
	M_EPILOG
}

void HClient::set_login( yaal::hcore::HString const& login_ ) {
	M_PROLOG
	M_ASSERT( _login.is_empty() );
	_login = login_;
	return;
	M_EPILOG
}

void HClient::enter( HLogic::id_t id_ ) {
	M_PROLOG
	_logics.insert( id_ );
	return;
	M_EPILOG
}

void HClient::leave( HLogic::id_t id_ ) {
	M_PROLOG
	_logics.erase( id_ );
	return;
	M_EPILOG
}

void HClient::invalidate( void ) {
	_valid = false;
}

void HClient::authenticate( void ) {
	_authenticated = true;
}

}

