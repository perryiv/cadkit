
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
  typedef typename BaseClass::SizeType               SizeType;
  typedef typename BaseClass::SizeContainer          SizeContainer;
  typedef typename BaseClass::IndependentType        IndependentType;
  typedef typename BaseClass::IndependentArgument    IndependentArgument;
  typedef typename BaseClass::DependentType          DependentType;
  typedef typename BaseClass::DependentContainer     DependentContainer;
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

#ifdef GN_USE_VIRTUAL_PROTECTED_DESTRUCTORS
protected:
  virtual ~Curve()
#else
  ~Curve()
#endif
  {
  }
#ifdef GN_USE_VIRTUAL_PROTECTED_DESTRUCTORS
public:
#endif


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the order.
  ///
  ///   order == degree + 1.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType order() const
  {
    return BaseClass::order ( 0 );
  }
  SizeType order ( SizeType whichIndepVar ) const
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

  SizeType degree() const
  {
    return BaseClass::degree ( 0 );
  }
  SizeType degree ( SizeType whichIndepVar ) const
  {
    return BaseClass::degree ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of control points.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType numControlPoints() const
  {
    return BaseClass::numControlPoints ( 0 );
  }
  SizeType numControlPoints ( SizeType whichIndepVar ) const
  {
    return BaseClass::numControlPoints ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of knots.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType numKnots() const
  {
    return BaseClass::numKnots ( 0 );
  }
  SizeType numKnots ( SizeType whichIndepVar ) const
  {
    return BaseClass::numKnots ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &knot ( SizeType whichKnot ) const
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  IndependentType &knot ( SizeType whichKnot )
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  const IndependentType &knot ( SizeType whichIndepVar, SizeType whichKnot ) const
  {
    return BaseClass::knot ( whichIndepVar, whichKnot );
  }

  IndependentType &knot ( SizeType whichIndepVar, SizeType whichKnot )
  {
    return BaseClass::knot ( whichIndepVar, whichKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the first knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &firstKnot() const
  {
    return BaseClass::firstKnot ( 0 );
  }

  IndependentType &firstKnot()
  {
    return BaseClass::firstKnot ( 0 );
  }

  const IndependentType &firstKnot ( SizeType whichIndepVar ) const
  {
    return BaseClass::firstKnot ( whichIndepVar );
  }

  IndependentType &firstKnot ( SizeType whichIndepVar )
  {
    return BaseClass::firstKnot ( whichIndepVar );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the last knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &lastKnot() const
  {
    return BaseClass::lastKnot ( 0 );
  }

  IndependentType &lastKnot()
  {
    return BaseClass::lastKnot ( 0 );
  }

  const IndependentType &lastKnot ( SizeType whichIndepVar ) const
  {
    return BaseClass::lastKnot ( whichIndepVar );
  }

  IndependentType &lastKnot ( SizeType whichIndepVar )
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

  const IndependentType &endKnotLeft ( SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( 0, whichEndKnot );
  }

  IndependentType &endKnotLeft ( SizeType whichEndKnot )
  {
    return BaseClass::endKnotLeft ( 0, whichEndKnot );
  }

  const IndependentType &endKnotLeft ( SizeType whichIndepVar, SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( whichIndepVar, whichEndKnot );
  }

  IndependentType &endKnotLeft ( SizeType whichIndepVar, SizeType whichEndKnot )
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

  const IndependentType &endKnotRight ( SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( 0, whichEndKnot );
  }

  IndependentType &endKnotRight ( SizeType whichEndKnot )
  {
    return BaseClass::endKnotRight ( 0, whichEndKnot );
  }

  const IndependentType &endKnotRight ( SizeType whichIndepVar, SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( whichIndepVar, whichEndKnot );
  }

  IndependentType &endKnotRight ( SizeType whichIndepVar, SizeType whichEndKnot )
  {
    return BaseClass::endKnotRight ( whichIndepVar, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Get the knot multiplicity. Returns zero if the knot is not found.
  //
  /////////////////////////////////////////////////////////////////////////////

  SizeType knotMultiplicity ( IndependentArgument u ) const
  {
    return BaseClass::knotMultiplicity ( 0, u );
  }

  SizeType knotMultiplicity ( SizeType whichIndepVar, IndependentArgument u ) const
  {
    return BaseClass::knotMultiplicity ( whichIndepVar, u );
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

  template < class Predicate_ > bool equal ( const ThisType &c, const Predicate_ &pred ) const
  {
    return BaseClass::equal ( c, pred );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Return a reference to this curve as a spline.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SplineClass &spline()
  {
    return *this;
  }
  const SplineClass &spline() const
  {
    return *this;
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
