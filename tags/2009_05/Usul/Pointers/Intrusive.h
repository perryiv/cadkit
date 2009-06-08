
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

#ifndef _USUL_INTRUSIVE_POINTER_INTRUSIVE_H_
#define _USUL_INTRUSIVE_POINTER_INTRUSIVE_H_

#include "Usul/Config/Config.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Intrusive-pointer functions. Keep these in global namespace. These names 
//  are compatable with boost::intrusive_ptr.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline void intrusive_ptr_add_ref ( T *ptr )
{
  if ( ptr )
  {
    ptr->ref();
  }
}
template < class T > inline void intrusive_ptr_release ( T *ptr )
{
  if ( ptr )
  {
    ptr->unref();
  }
}
template < class T > inline void ptr_release_no_delete ( T *ptr )
{
  if ( ptr )
  {
    ptr->unref ( false );
  }
}

#endif // _USUL_INTRUSIVE_POINTER_INTRUSIVE_H_
