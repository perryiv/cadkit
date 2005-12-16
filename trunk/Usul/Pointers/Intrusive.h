
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Intrusive reference counting functions. Keep these in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTRUSIVE_POINTER_FUNCTIONS_H_
#define _USUL_INTRUSIVE_POINTER_FUNCTIONS_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Intrusive-pointer functions. Keep these in global namespace. These names 
//  are compatable with boost::intrusive_ptr.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline void intrusive_ptr_add_ref ( T *ptr )
{
  ptr->ref();
}
template < class T > inline void intrusive_ptr_release ( T *ptr )
{
  ptr->unref();
}
template < class T > inline void ptr_release_no_delete ( T *ptr )
{
  ptr->unref ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Slightly more convenient (and safe) form of the above.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Pointers
  {
    template < class T > void reference ( T *v )
    {
      if ( v )
        v->ref();
    }
    template < class T > void unreference ( T *v )
    {
      if ( v )
        v->unref();
    }
  }
}


#endif // _USUL_INTRUSIVE_POINTER_FUNCTIONS_H_
