
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NURBS curve class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_CURVE_CLASS_H_
#define _GENERIC_NURBS_LIBRARY_CURVE_CLASS_H_

#include "GN/Splines/Spline.h"


namespace GN {
namespace Splines {


template < class ConfigType_ > class Curve : public Spline < ConfigType_ >
{
public:

  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Useful typedefs.
  ///
  /////////////////////////////////////////////////////////////////////////////

  typedef ConfigType_                                ConfigType;
  typedef Curve < ConfigType >                       ThisType;
  typedef Spline < ConfigType >                      BaseClass;
  typedef typename BaseClass::UIntType               UIntType;
  typedef typename BaseClass::UIntContainer          UIntContainer;
  typedef typename BaseClass::KnotType               KnotType;
  typedef typename BaseClass::KnotVectorSizeType     KnotVectorSizeType;
  typedef typename BaseClass::KnotArgument           KnotArgument;
  typedef typename BaseClass::ControlPointType       ControlPointType;
  typedef typename BaseClass::ControlPointContainer  ControlPointContainer;
  typedef typename BaseClass::UIntContainerSizeType  UIntContainerSizeType;
  typedef GN::Traits::Curve                          TypeTag;
  typedef BaseClass                                  SplineClass;


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Default constructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Curve() : BaseClass()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Copy constructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Curve ( const ThisType &c ) : BaseClass ( c )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Destructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  GN_DESTRUCTOR_TYPE ~Curve()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Ensure public scope because GN_DESTRUCTOR_TYPE is user-defined.
  ///
  /////////////////////////////////////////////////////////////////////////////

public:


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the order.
  ///
  ///   order == degree + 1.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType order() const
  {
    return BaseClass::order ( 0 );
  }
  UIntType order ( UIntContainerSizeType whichIndepVar ) const
  {
    return BaseClass::order ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the degree.
  ///
  ///   degree == order - 1.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType degree() const
  {
    return BaseClass::degree ( 0 );
  }
  UIntType degree ( UIntContainerSizeType whichIndepVar ) const
  {
    return BaseClass::degree ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of control points.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numControlPoints() const
  {
    return BaseClass::numControlPoints ( 0 );
  }
  UIntType numControlPoints ( UIntContainerSizeType whichIndepVar ) const
  {
    return BaseClass::numControlPoints ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of knots.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numKnots() const
  {
    return BaseClass::numKnots ( 0 );
  }
  UIntType numKnots ( UIntContainerSizeType whichIndepVar ) const
  {
    return BaseClass::numKnots ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &knot ( KnotVectorSizeType whichKnot ) const
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  KnotType &knot ( KnotVectorSizeType whichKnot )
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  const KnotType &knot ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichKnot ) const
  {
    return BaseClass::knot ( whichIndepVar, whichKnot );
  }

  KnotType &knot ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichKnot )
  {
    return BaseClass::knot ( whichIndepVar, whichKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the first knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &firstKnot() const
  {
    return BaseClass::firstKnot ( 0 );
  }

  KnotType &firstKnot()
  {
    return BaseClass::firstKnot ( 0 );
  }

  const KnotType &firstKnot ( KnotVectorSizeType whichIndepVar ) const
  {
    return BaseClass::firstKnot ( whichIndepVar );
  }

  KnotType &firstKnot ( KnotVectorSizeType whichIndepVar )
  {
    return BaseClass::firstKnot ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the last knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &lastKnot() const
  {
    return BaseClass::lastKnot ( 0 );
  }

  KnotType &lastKnot()
  {
    return BaseClass::lastKnot ( 0 );
  }

  const KnotType &lastKnot ( KnotVectorSizeType whichIndepVar ) const
  {
    return BaseClass::lastKnot ( whichIndepVar );
  }

  KnotType &lastKnot ( KnotVectorSizeType whichIndepVar )
  {
    return BaseClass::lastKnot ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the left.
  ///
  /// For example, if the spline is a cubic curve with 7 control points, then 
  /// endKnotLeft(2) will return:
  ///
  ///   0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1
  ///         |
  ///         |
  ///     this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &endKnotLeft ( KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( 0, whichEndKnot );
  }

  KnotType &endKnotLeft ( KnotVectorSizeType whichEndKnot )
  {
    return BaseClass::endKnotLeft ( 0, whichEndKnot );
  }

  const KnotType &endKnotLeft ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( whichIndepVar, whichEndKnot );
  }

  KnotType &endKnotLeft ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot )
  {
    return BaseClass::endKnotLeft ( whichIndepVar, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the right.
  ///
  /// For example, if the spline is a cubic curve with 7 control points, then 
  /// endKnotRight(2) will return:
  ///
  ///   0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1
  ///                                      |
  ///                                      |
  ///                                  this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &endKnotRight ( KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( 0, whichEndKnot );
  }

  KnotType &endKnotRight ( KnotVectorSizeType whichEndKnot )
  {
    return BaseClass::endKnotRight ( 0, whichEndKnot );
  }

  const KnotType &endKnotRight ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( whichIndepVar, whichEndKnot );
  }

  KnotType &endKnotRight ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichEndKnot )
  {
    return BaseClass::endKnotRight ( whichIndepVar, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Get the knot multiplicity. Returns zero if the knot is not found.
  //
  /////////////////////////////////////////////////////////////////////////////

  UIntType knotMultiplicity ( KnotArgument u ) const
  {
    return BaseClass::knotMultiplicity ( 0, u );
  }

  UIntType knotMultiplicity ( KnotVectorSizeType whichIndepVar, KnotArgument u ) const
  {
    return BaseClass::knotMultiplicity ( whichIndepVar, u );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Resize the curve.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void resize ( UIntType dimension, 
                UIntType order, 
                UIntType numCtrPts, 
                bool rational )
  {
    UIntContainer o;
    o.resize ( 1 );
    o[0] = order;

    UIntContainer n;
    n.resize ( 1 );
    n[0] = numCtrPts;

    BaseClass::resize ( dimension, o, n, rational );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Set this curve. This is here for type safety.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void set ( const ThisType &c )
  {
    // Call base class' function.
    BaseClass::set ( c );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Assignment operator.
  ///
  /////////////////////////////////////////////////////////////////////////////

  ThisType &operator = ( const ThisType &c )
  {
    this->set ( c );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the curves are exactly equal. This is here for type safety.
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const ThisType &c ) const
  {
    return BaseClass::equal ( c );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the curves are equal. The argument "pred" is a predicate-functor 
  /// that allows for special comparisons, such as being within a tolerance.
  ///
  /////////////////////////////////////////////////////////////////////////////

  template < class Predicate_ > bool equal ( const ThisType &s, const Predicate_ &pred ) const
  {
    return BaseClass::equal ( c, pred );
  }
};


}; // namespace Splines
}; // namespace GN


///////////////////////////////////////////////////////////////////////////////
//
//  Equality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class ConfigType > inline bool operator == 
  ( const GN::Splines::Curve<ConfigType> &c1, const GN::Splines::Curve<ConfigType> &c2 )
{
  return c1.equal ( c2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class ConfigType > inline bool operator != 
  ( const GN::Splines::Curve<ConfigType> &c1, const GN::Splines::Curve<ConfigType> &c2 )
{
  return !( c1.equal ( c2 ) );
}


#endif // _GENERIC_NURBS_LIBRARY_CURVE_CLASS_H_
