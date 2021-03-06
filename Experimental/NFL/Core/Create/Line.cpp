
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms to make the spline into a line.
//
///////////////////////////////////////////////////////////////////////////////

#include "NFL/Core/Create/Line.h"

#include "GN/Create/Line.h"

#include "Usul/MPL/SameType.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace.
//
///////////////////////////////////////////////////////////////////////////////

namespace NFL {
namespace Core {
namespace Create {


///////////////////////////////////////////////////////////////////////////////
//
//  Make a line between the given points.
//
///////////////////////////////////////////////////////////////////////////////

void line ( NFL::Core::Splines::Spline &s, const NFL::Core::Splines::Spline::Vec3 &point0, const NFL::Core::Splines::Spline::Vec3 &point1 )
{
  USUL_ASSERT_SAME_TYPE ( NFL::Core::Splines::Spline::InternalSpline::Vec3, Usul::Math::Vec3d );
  GN::Create::line ( s.spline(), point0, point1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace Create
} // namespace Core
} // namespace NFL
