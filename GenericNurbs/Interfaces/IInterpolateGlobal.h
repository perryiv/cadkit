
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_INTERFACE_INTERPOLATE_GLOBAL_H_
#define _GENERIC_NURBS_LIBRARY_INTERFACE_INTERPOLATE_GLOBAL_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>


namespace GN {
namespace Interfaces {


template
<
  class PointType_, 
  unsigned long iid
>
struct IInterpolateGlobal : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IInterpolateGlobal );

  // Useful typedef.
  typedef PointType_ PointType;
  typedef std::vector < PointType > PointVector;
  typedef std::vector < PointVector > PointMatrix;
  typedef std::vector < unsigned int > Orders;

  // Id for this interface.
  enum { IID = iid };

  // Interpolate the points. Dimension = points.size(). Independents = order.size().
  virtual void interpolateGlobal ( const PointMatrix &points, const Orders &order, Unknown *caller ) = 0;
};


typedef IInterpolateGlobal < double, 3935915719u >                                  IInterpolateGlobalDouble;
typedef IInterpolateGlobal < float,  2917122247u >                                  IInterpolateGlobalFloat;
typedef IInterpolateGlobal < Usul::Interfaces::IUnknown::ValidRefPtr, 1746708156u > IInterpolateGlobalUnknown;


}; // namespace Interfaces
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_INTERFACE_INTERPOLATE_GLOBAL_H_
