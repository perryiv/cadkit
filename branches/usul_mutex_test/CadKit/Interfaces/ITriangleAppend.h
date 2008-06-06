
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ITriangleAppend: Interface for appending triangles.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_TRIANGLE_APPEND_H_
#define _CADKIT_INTERFACE_TRIANGLE_APPEND_H_

#include "IUnknown.h"


namespace CadKit
{
template <const unsigned int id, class T> class ITriangleAppend : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Set the triangle.
  virtual bool            appendTriangle ( T t0v0, T t0v1, T t0v2, 
                                           T t1v0, T t1v1, T t1v2,
                                           T t2v0, T t2v1, T t2v2,
                                           IUnknown *caller ) = 0;
};


// Common types.
typedef ITriangleAppend<1052885781,float>  ITriangleAppendFloat;
typedef ITriangleAppend<1052885782,double> ITriangleAppendDouble;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_TRIANGLE_APPEND_H_
