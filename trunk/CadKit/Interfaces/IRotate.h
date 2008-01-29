
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IRotate: Interface for rotating geometry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_ROTATE_H_
#define _CADKIT_INTERFACE_ROTATE_H_

#include "IUnknown.h"


namespace CadKit
{
template <const unsigned int id, class T> class IRotate : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Modify the current scale.
  virtual void            rotate ( T x, T y, T z, T deg ) = 0;
};


// Common types.
typedef IRotate<1064385620,float>  IRotateFloat;
typedef IRotate<1064385621,double> IRotateDouble;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_ROTATE_H_
