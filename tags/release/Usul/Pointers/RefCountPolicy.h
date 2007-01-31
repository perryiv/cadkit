
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


namespace Usul {
namespace Pointers {


struct RefCountPolicy
{
  template < class T > static void ref ( T *t )
  {
    intrusive_ptr_add_ref ( t );
  }
  template < class T > static void unref ( T *t )
  {
    intrusive_ptr_release ( t );
  }
  template < class T > static void unrefNoDelete ( T *t )
  {
    ptr_release_no_delete ( t );
  }
};


}; // namespace Pointers
}; // namespace Usul


#endif // _USUL_POINTERS_REFERENCE_COUNTING_POLICY_H_
