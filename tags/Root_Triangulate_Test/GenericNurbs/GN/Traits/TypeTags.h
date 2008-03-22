
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  The possible core types.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_TRAITS_POSSIBLE_TYPES_CLASS_H_
#define _GENERIC_NURBS_LIBRARY_TRAITS_POSSIBLE_TYPES_CLASS_H_


namespace GN {
namespace Traits {


///////////////////////////////////////////////////////////////////////////////
///
///  The three types of splines.
///
///////////////////////////////////////////////////////////////////////////////

struct Spline{};                  // n-number of independent variables
struct Curve : public Spline{};   // 1 independent variable
struct Surface : public Spline{}; // 2 independent variables


}; // namespace Traits
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_TRAITS_POSSIBLE_TYPES_CLASS_H_