
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ANIMATE_NURBS_CURVE_H__
#define __USUL_INTERFACES_ANIMATE_NURBS_CURVE_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"

#include <vector>


namespace Usul {
namespace Interfaces {


struct IAnimateNurbsCurve : public Usul::Interfaces::IUnknown
{
  /// Typedefs.
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vector3<Vec3d> DependentType; // eye, center, up
  typedef std::vector<DependentType> ControlPoints;
  typedef std::vector<double> KnotVector;
  typedef std::vector<double> Parameters;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimateNurbsCurve );

  /// Id for this interface.
  enum { IID = 1839516359u };

  // Animate the curve.
  virtual void                animateNurbsCurve ( const ControlPoints &, const KnotVector &, unsigned int degree, const Parameters & ) = 0;
  virtual void                animateNurbsCurve ( const ControlPoints &, const KnotVector &, unsigned int degree, unsigned int steps ) = 0;
};


}
}

#endif // __USUL_INTERFACES_ANIMATE_NURBS_CURVE_H__
