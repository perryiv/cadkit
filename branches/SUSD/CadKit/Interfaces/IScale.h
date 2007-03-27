
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IScale: Interface for scaling geometry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_SCALE_H_
#define _CADKIT_INTERFACE_SCALE_H_

#include "IUnknown.h"


namespace CadKit
{
template <const unsigned int id, class T> class IScale : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Modify the current scale.
  virtual void            scale ( T x, T y, T z ) = 0;
};


// Common types.
typedef IScale<1064385606,float>  IScaleFloat;
typedef IScale<1064385607,double> IScaleDouble;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_SCALE_H_
