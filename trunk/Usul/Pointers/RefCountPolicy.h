
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Policy class with reference counting.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_POINTERS_REFERENCE_COUNTING_POLICY_H_
#define _USUL_POINTERS_REFERENCE_COUNTING_POLICY_H_

#include "Usul/Config/Config.h"


namespace Usul {
namespace Pointers {


struct RefCountPolicy
{
  template < class T > static void ref ( T *ptr )
  {
    if ( ptr )
    {
      ptr->ref();
    }
  }
  template < class T > static void unref ( T *ptr )
  {
    if ( ptr )
    {
      ptr->unref();
    }
  }
  template < class T > static void unrefNoDelete ( T *ptr )
  {
    if ( ptr )
    {
      ptr->unref ( false );
    }
  }
};


} // namespace Pointers
} // namespace Usul


#endif // _USUL_POINTERS_REFERENCE_COUNTING_POLICY_H_
