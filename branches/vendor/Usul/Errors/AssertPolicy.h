
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking class that asserts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_POLICY_ASSERT_H_
#define _USUL_ERROR_POLICY_ASSERT_H_

#include "Usul/Errors/Assert.h"


namespace Usul {
namespace Errors {


struct AssertPolicy
{
  AssertPolicy ( const char *, unsigned int, bool state )
  {
    USUL_ASSERT ( state );
  }
  AssertPolicy ( unsigned int, bool state )
  {
    USUL_ASSERT ( state );
  }
  template < class Arg_ > AssertPolicy ( unsigned int, bool state, const Arg_ & )
  {
    USUL_ASSERT ( state );
  }
  template < class Arg_ > AssertPolicy ( const char *, unsigned int, bool state, const Arg_ & )
  {
    USUL_ASSERT ( state );
  }
  template < class Int_ > static void bounds ( unsigned int id, Int_ size, Int_ index )
  {
    // Put the id in the assert statement.
    USUL_ASSERT ( 0 != id && index >= 0 && index < size );
  }
};


}; // namespace Errors
}; // namespace Usul


#endif // _USUL_ERROR_POLICY_ASSERT_H_
