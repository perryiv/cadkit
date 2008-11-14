
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates the result of a query.
//
///////////////////////////////////////////////////////////////////////////////

#include "OracleWrap/Result.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"

#include "ocilib.h"

#include <stdexcept>

using namespace OracleWrap;

namespace OracleWrap { namespace Detail { std::string getLastError(); } }


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Result::Result ( OCI_Statement *statement, Mutex &mutex ) : BaseClass(),
  _statement ( statement ),
  _result ( 0x0 ),
  _mutex ( mutex )
{
  // Lock the mutex until we are destroyed.
  _mutex.lock();

  // Get the result set. Handle null statements.
  if ( 0x0 != _statement )
  {
    _result = ::OCI_GetResultset ( _statement );
    if ( 0x0 == _result )
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 1819950264: Failed to get result set. ", OracleWrap::Detail::getLastError() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Result::~Result()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Result::_destroy ), "1225065417" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Result::_destroy()
{
  if ( 0x0 != _result )
  {
    if ( 0x0 != _statement )
    {
      if ( FALSE == ::OCI_ReleaseResultsets ( _statement ) )
      {
        std::cout << "Error 5436080370: Failed to release result sets. " << OracleWrap::Detail::getLastError() << std::endl;
      }
      _statement = 0x0;
    }
    _result = 0x0;
  }
  _mutex.unlock();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the result have more rows?
//
///////////////////////////////////////////////////////////////////////////////

bool Result::hasMoreRows() const
{
  return ( ( 0x0 == _result ) ? false : ( TRUE == ::OCI_FetchNext ( _result ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Result::numColumns() const
{
  return ( ( 0x0 == _result ) ? 0 : ( ::OCI_GetColumnCount ( _result ) ) );
}
