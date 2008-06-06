
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for evaluating a point.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_INTERFACE_EVALUATE_POINT_H_
#define _GENERIC_NURBS_LIBRARY_INTERFACE_EVALUATE_POINT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>


namespace GN {
namespace Interfaces {


template
<
  class IndependentType_, 
  class DependentType_, 
  unsigned long iid
>
struct IEvaluatePoint : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IEvaluatePoint );

  // Useful typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef IndependentType_ IndependentType;
  typedef DependentType_ DependentType;
  typedef std::vector < DependentType > DependentVector;

  // Id for this interface.
  enum { IID = iid };

  // Evaluate the point at the given parameter.
  virtual void evaluatePoint ( const IndependentType &u, DependentVector &point, Unknown *caller ) const = 0;
};


typedef IEvaluatePoint < double, double, 3391742854u >                                  IEvaluatePointDouble;
typedef IEvaluatePoint < float,  double, 1165485047u >                                  IEvaluatePointFloat;
typedef IEvaluatePoint < double, Usul::Interfaces::IUnknown::ValidRefPtr, 2673575468u > IEvaluatePointUnknown;


}; // namespace Interfaces
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_INTERFACE_EVALUATE_POINT_H_
