/*
---           `galaxy' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	galaxy.c - this file is integral part of `galaxy' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <stdhapi.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "galaxy.h"

#include "setup.h"

using namespace std;
using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::tools;
using namespace stdhapi::tools::util;

class HBoard : public HControl
	{
protected:
	/*{*/
	/*}*/
public:
	/*{*/
	HBoard ( HWindow * );
	virtual ~HBoard ( void );
	virtual void refresh ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

class HGalaxyWindow : public HWindow
	{
protected:
	/*{*/
	HBoard * f_poBoard;
	/*}*/
public:
	/*{*/
	HGalaxyWindow ( char const * const );
	virtual ~HGalaxyWindow ( void );
	virtual int init ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	HGalaxyWindow ( HGalaxyWindow const & );
	HGalaxyWindow & operator = ( HGalaxyWindow const & );
	/*}*/
	};

class HClient : public HTUIProcess
	{
protected:
	/*{*/
	HSocket f_oSocket;
	HGalaxyWindow f_oWindow;
	/*}*/
public:
	/*{*/
	HClient ( char const * const );
	virtual ~HClient ( void );
	void init_client ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

HBoard::HBoard ( HWindow * a_poParent )
	: HControl ( a_poParent, 1, 1, 20, 20, "Galaxy" )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HBoard::~HBoard ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HBoard::refresh ( void )
	{
	M_PROLOG
	HBoard::draw_label ( );
	M_EPILOG
	}

HGalaxyWindow::HGalaxyWindow ( char const * const a_pcWindowTitle )
	: HWindow ( a_pcWindowTitle ), f_poBoard ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HGalaxyWindow::~HGalaxyWindow ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HGalaxyWindow::init ( void )
	{
	M_PROLOG
	HWindow::init ( );
	f_poBoard = new HBoard ( this );
	f_poBoard->enable ( true );
	f_poBoard->set_focus ( );
	return ( 0 );
	M_EPILOG
	}

HClient::HClient ( char const * const a_pcProgramName )
	: f_oSocket ( ), f_oWindow ( a_pcProgramName )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HClient::~HClient ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HClient::init_client ( void )
	{
	M_PROLOG
	HTUIProcess::init_tui ( "galaxy", & f_oWindow );
	return;
	M_EPILOG
	}

int main_client ( void )
	{
	HClient l_oClient ( setup.f_pcProgramName );
	l_oClient.init_client ( );
	l_oClient.run ( );
	return ( 0 );
	}

