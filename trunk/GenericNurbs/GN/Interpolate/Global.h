
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_
#define _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_

#include "GN/Macros/ErrorCheck.h"
#include "GN/MPL/TypeCheck.h"


namespace GN {
namespace Interpolate {


///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template
  <
    class CurveType, 
    class IndependentContainer, 
    class DependentContainer
  >
  struct Global
  {

  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class CurveType, 
  class IndependentContainer, 
  class DependentContainer
>
void global ( const CurveType &curve,
              const typename CurveType::UIntType &order,
              const IndependentContainer &params,
              const IndependentContainer &knots,
              const DependentContainer &points )
{
  GN_CAN_BE_CURVE ( CurveType );
  // TODO
}


}; // namespace Interpolate
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_
