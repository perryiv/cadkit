
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NURBS spline class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _NURBS_FOUNDATION_LIBRARY_SPLINE_CLASS_H_
#define _NURBS_FOUNDATION_LIBRARY_SPLINE_CLASS_H_

#include "NFL/Core/Export/Export.h"

#include "GN/Splines/Spline.h"
#include "GN/Config/UsulConfig.h"

#include <string>


namespace NFL {
namespace Core {
namespace Splines {


class NURBS_SPLINE_EXPORT Spline
{
public:

  // Typedefs.
  typedef GN::Config::UsulConfig < double > SplineConfig;
  typedef GN::Splines::Spline < SplineConfig > InternalSpline;
  typedef InternalSpline::Vec3 Vec3;

  // Constructors and destructor.
  Spline();
  Spline ( const Spline & );
  ~Spline();

  // Assignment.
  Spline &operator = ( const Spline & );

  // Set/get the name.
  void                                name ( const std::string &n ) { _s.setStringData ( n ); }
  const std::string &                 name() const { return _s.getStringData(); }

  // Access the internal spline.
  const InternalSpline &              spline() const { return _s; }
  InternalSpline &                    spline()       { return _s; }

private:

  InternalSpline _s;
};


} // namespace Splines
} // namespace Core
} // namespace NFL


#endif //_NURBS_FOUNDATION_LIBRARY_SPLINE_CLASS_H_
