
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Curve of doubles.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_COMPONENTS_CURVE_OF_DOUBLE_CLASS_H_
#define _GENERIC_NURBS_COMPONENTS_CURVE_OF_DOUBLE_CLASS_H_

#include "GenericNurbs/Components/CompileGuard.h"

#include "Usul/Base/Referenced.h"

#include "GN/Config/UsulConfig.h"
#include "GN/Splines/Curve.h"
#include "GN/Interfaces/ICurve.h"
#include "GN/Interfaces/IEvaluatePoint.h"
#include "GN/Interfaces/IInterpolateGlobal.h"


namespace GN {
namespace Components {


class CurveDouble : public Usul::Base::Referenced,
                    public GN::Interfaces::ICurveDouble,
                    public GN::Interfaces::IEvaluatePointDouble,
                    public GN::Interfaces::IInterpolateGlobalDouble
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef GN::Interfaces::ICurveDouble::Unknown Unknown;
  typedef GN::Interfaces::ICurveDouble::IndependentType IndependentType;
  typedef GN::Interfaces::ICurveDouble::DependentType DependentType;
  typedef GN::Interfaces::IEvaluatePointDouble::DependentVector DependentVector;
  typedef GN::Interfaces::ICurveDouble::SizeType SizeType;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CurveDouble );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  CurveDouble();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  GN::Interfaces::ICurveDouble
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the number of dependent variables.
  virtual SizeType        numDepVars() const;

  // Get the order of the curve (degree+1).
  virtual SizeType        order() const;

  // Is it rational?
  virtual bool            rational() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  GN::Interfaces::IEvaluatePointDouble
  //
  /////////////////////////////////////////////////////////////////////////////

  // Evaluate the point at the given parameter.
  virtual void            evaluatePoint ( const IndependentType &u, DependentVector &point, Unknown *caller ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  GN::Interfaces::IInterpolateGlobalDouble
  //
  /////////////////////////////////////////////////////////////////////////////

  // Interpolate the points. Dimension = points.size(). Independents = order.size().
  virtual void            interpolateGlobal ( unsigned int order, const DependentMatrix &points, Unknown *caller );

protected:

  // Do not copy.
  CurveDouble ( const CurveDouble & );
  CurveDouble &operator = ( const CurveDouble & );

  /// Use reference counting.
  virtual ~CurveDouble();

private:

  typedef GN::Config::UsulConfig < IndependentType, DependentType, SizeType > Config;
  typedef GN::Splines::Curve < Config > Curve;

  Curve _curve;
};


}; // namespace Components
}; // namespace GN


#endif // _GENERIC_NURBS_COMPONENTS_CURVE_OF_DOUBLE_CLASS_H_
