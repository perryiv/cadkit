
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a NURBS curve.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_INTERFACE_CURVE_H_
#define _GENERIC_NURBS_LIBRARY_INTERFACE_CURVE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace GN {
namespace Interfaces {


template
<
  class IndependentType_, 
  class DependentType_, 
  unsigned long iid
>
struct ICurve : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICurve );

  // Useful typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef IndependentType_ IndependentType;
  typedef DependentType_ DependentType;
  typedef unsigned int SizeType;

  // Id for this interface.
  enum { IID = iid };

  // Get the number of dependent variables.
  virtual SizeType        numDepVars() const = 0;

  // Get the order of the curve (degree+1).
  virtual SizeType        order() const = 0;

  // Is it rational?
  virtual bool            rational() const = 0;
};


typedef ICurve < double, double, 1409974239u >                                  ICurveDouble;
typedef ICurve < float,  float,  1254820572u >                                  ICurveFloat;
typedef ICurve < double, Usul::Interfaces::IUnknown::ValidRefPtr, 2542678121u > ICurveUnknown;


}; // namespace Interfaces
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_INTERFACE_CURVE_H_
