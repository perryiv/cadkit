
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking class that throws.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_POLICY_THROWER_H_
#define _USUL_ERROR_POLICY_THROWER_H_

#include "Usul/Exceptions/Thrower.h"


namespace Usul {
namespace Errors {


template < class Exception_ > struct ThrowingPolicy
{
  ThrowingPolicy ( const char *filename, unsigned int line, bool state )
  {
    if ( !state )
      Usul::Exceptions::Thrower < Exception_ > ( "Error Line: ", line, ", File: ", filename );
  }
  ThrowingPolicy ( unsigned int num, bool state )
  {
    if ( !state )
      Usul::Exceptions::Thrower < Exception_ > ( "Error ", num );
  }
  template < class Arg_ > ThrowingPolicy ( unsigned int num, bool state, const Arg_ &arg )
  {
    if ( !state )
      Usul::Exceptions::Thrower < Exception_ > ( "Error ", num, ": ", arg );
  }
  template < class Arg_ > ThrowingPolicy ( const char *filename, unsigned int line, bool state, const Arg_ &arg )
  {
    if ( !state )
      Usul::Exceptions::Thrower < Exception_ > ( "Error Line: ", line, ", File: ", filename, ", ", arg );
  }
  template < class Int_ > static void bounds ( unsigned int num, Int_ size, Int_ index )
  {
    if ( index < 0 || index >= size )
      Usul::Exceptions::Thrower < Exception_ > ( "Error ", num, ": Index out of range" );
  }
};


} // namespace Errors
} // namespace Usul


#endif // _USUL_ERROR_POLICY_THROWER_H_
