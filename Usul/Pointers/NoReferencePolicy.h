
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Policy class with no reference of any kind.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_POINTERS_NO_REFERENCE_POLICY_H_
#define _USUL_POINTERS_NO_REFERENCE_POLICY_H_

#include "Usul/Config/Config.h"


namespace Usul {
namespace Pointers {


struct NoRefCountPolicy
{
  template < class T > static void ref           ( const T *t ){}
  template < class T > static void unref         ( const T *t ){}
  template < class T > static void unrefNoDelete ( const T *t ){}
};


} // namespace Pointers
} // namespace Usul


#endif // _USUL_POINTERS_NO_REFERENCE_POLICY_H_
