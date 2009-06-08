
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

#ifndef _NURBS_FOUNDATION_LIBRARY_CREATE_LINE_H_
#define _NURBS_FOUNDATION_LIBRARY_CREATE_LINE_H_

#include "NFL/Core/Splines/Spline.h"


namespace NFL {
namespace Core {
namespace Create {


// Make a line between the given points.
NURBS_SPLINE_EXPORT void line ( NFL::Core::Splines::Spline &s, 
                                const NFL::Core::Splines::Spline::Vec3 &point0, 
                                const NFL::Core::Splines::Spline::Vec3 &point1 );


} // namespace Create
} // namespace Core
} // namespace NFL


#endif //_NURBS_FOUNDATION_LIBRARY_CREATE_LINE_H_
