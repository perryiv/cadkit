
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Policy class for deleting object pointers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_POINTERS_OBJECT_DELETE_POLICY_H_
#define _USUL_POINTERS_ARRAY_DELETE_POLICY_H_


namespace Usul {
namespace Pointers {


struct ObjectDeletePolicy
{
  template < class T > static void ref ( T *t )
  {
  }
  template < class T > static void unref ( T *t )
  {
    object_pointer_delete ( t );
  }
  template < class T > static void unrefNoDelete ( T *t )
  {
  }
};


}; // namespace Pointers
}; // namespace Usul


#endif // _USUL_POINTERS_OBJECT_DELETE_POLICY_H_
