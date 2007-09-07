
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor for deleting.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTORS_DELETE_H_
#define _USUL_FUNCTORS_DELETE_H_


namespace Usul {
namespace Functors {
namespace General {


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the object.
//
///////////////////////////////////////////////////////////////////////////////

struct DeleteObject
{
  template < class T > void operator() ( T *t )
  {
    delete t;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the array.
//
///////////////////////////////////////////////////////////////////////////////

struct DeleteArray
{
  template < class T > void operator() ( T *t )
  {
    delete [] t;
  }
};


} // namespace General
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTORS_DELETE_H_
