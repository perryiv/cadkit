
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Function to delete an array. Specialize this function for custom deleting.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ARRAY_POINTER_DELETE_FUNCTION_H_
#define _USUL_ARRAY_POINTER_DELETE_FUNCTION_H_


template < class T > inline void array_pointer_delete ( T *ptr )
{
  delete [] ptr;
}


#endif // _USUL_ARRAY_POINTER_DELETE_FUNCTION_H_