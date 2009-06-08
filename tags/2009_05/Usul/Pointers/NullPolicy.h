
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Policy classes regarding null pointers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_POINTERS_NULL_POLICY_H_
#define _USUL_POINTERS_NULL_POLICY_H_


namespace Usul {
namespace Pointers {


///////////////////////////////////////////////////////////////////////////////
//
//  Policy class that does not permit null pointers.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ErrorPolicy_
>
struct NullBadPolicy
{
  template < class T > static void check ( const T *t )
  {
    // This may look like redundancy, however, we do not know how many cpu 
    // cycles it takes to construct the error-policy. Further, this permits 
    // us to easily insert a breakpoint.
    if ( 0x0 == t )
    {
      ErrorPolicy_ ( 1075270902, false, "Null pointer" );
    }
  }
};


} // namespace Pointers
} // namespace Usul


#endif // _USUL_POINTERS_NULL_POLICY_H_
