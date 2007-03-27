
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NURBS curve class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _NURBS_FOUNDATION_LIBRARY_CURVE_CLASS_H_
#define _NURBS_FOUNDATION_LIBRARY_CURVE_CLASS_H_

#include "NFL/Core/Splines/Spline.h"


namespace NFL {
namespace Core {
namespace Splines {


  class NURBS_SPLINE_EXPORT Curve : public NFL::Core::Splines::Spline
{
public:

  // Typedefs.
  typedef NFL::Core::Splines::Spline BaseClass;

  // Constructors and destructor.
  Curve();
  Curve ( const Curve & );
  ~Curve();

  // Assignment.
  Curve &operator = ( const Curve & );
};


} // namespace Splines
} // namespace Core
} // namespace NFL


#endif //_NURBS_FOUNDATION_LIBRARY_CURVE_CLASS_H_
