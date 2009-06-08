
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NURBS surface class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_SURFACE_CLASS_H_
#define _GENERIC_NURBS_LIBRARY_SURFACE_CLASS_H_

#include "GN/Splines/Spline.h"


namespace GN {
namespace Splines {


template < class ConfigType_ > class Surface : public Spline < ConfigType_ >
{
public:

  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Useful typedefs.
  ///
  /////////////////////////////////////////////////////////////////////////////

  typedef ConfigType_                             ConfigType;
  typedef Surface < ConfigType >                  ThisType;
  typedef Spline < ConfigType >                   BaseClass;
  typedef typename BaseClass::SizeType            SizeType;
  typedef typename BaseClass::SizeContainer       SizeContainer;
  typedef typename BaseClass::IndependentType     IndependentType;
  typedef typename BaseClass::IndependentArgument IndependentArgument;
  typedef GN::Traits::Surface                     TypeTag;
  typedef BaseClass                               SplineClass;


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Default constructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Surface() : BaseClass()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Copy constructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Surface ( const ThisType &c ) : BaseClass ( c )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Destructor.
  ///
  /////////////////////////////////////////////////////////////////////////////

#ifdef GN_USE_VIRTUAL_PROTECTED_DESTRUCTORS
protected:
  virtual ~Surface()
#else
  ~Surface()
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

  SizeType orderU() const
  {
    return BaseClass::order ( 0 );
  }

  SizeType orderV() const
  {
    return BaseClass::order ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the degree.
  ///
  ///   degree == order - 1.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType degreeU() const
  {
    return BaseClass::degree ( 0 );
  }

  SizeType degreeV() const
  {
    return BaseClass::degree ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of control points.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType numControlPointsU() const
  {
    return BaseClass::numControlPoints ( 0 );
  }

  SizeType numControlPointsV() const
  {
    return BaseClass::numControlPoints ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of knots.
  ///
  /////////////////////////////////////////////////////////////////////////////

  SizeType numKnotsU() const
  {
    return BaseClass::numKnots ( 0 );
  }

  SizeType numKnotsV() const
  {
    return BaseClass::numKnots ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &knotU ( SizeType whichKnot ) const
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  const IndependentType &knotV ( SizeType whichKnot ) const
  {
    return BaseClass::knot ( 1, whichKnot );
  }

  IndependentType &knotU ( SizeType whichIndepVar, SizeType whichKnot )
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  IndependentType &knotV ( SizeType whichIndepVar, SizeType whichKnot )
  {
    return BaseClass::knot ( 1, whichKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the first knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &firstKnotU() const
  {
    return BaseClass::firstKnot ( 0 );
  }

  const IndependentType &firstKnotV() const
  {
    return BaseClass::firstKnot ( 1 );
  }

  IndependentType &firstKnotU()
  {
    return BaseClass::firstKnot ( 0 );
  }

  IndependentType &firstKnotV()
  {
    return BaseClass::firstKnot ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the last knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &lastKnotU() const
  {
    return BaseClass::lastKnot ( 0 );
  }

  const IndependentType &lastKnotV() const
  {
    return BaseClass::lastKnot ( 1 );
  }

  IndependentType &lastKnotU()
  {
    return BaseClass::lastKnot ( 0 );
  }

  IndependentType &lastKnotV()
  {
    return BaseClass::lastKnot ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the left of the first knot vector.
  ///
  /// For example, if the surface is bi-cubic with 7x5 control points, then 
  /// endKnotLeftU(2) will return:
  ///
  ///   0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1
  ///         |
  ///         |
  ///     this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &endKnotLeftU ( SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( 0, whichEndKnot );
  }

  IndependentType &endKnotLeftU ( SizeType whichEndKnot )
  {
    return BaseClass::endKnotLeft ( 0, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the left of the second knot vector.
  ///
  /// For example, if the surface is bi-cubic with 7x5 control points, then 
  /// endKnotLeftV(2) will return:
  ///
  ///   0, 0, 0, 0, 0.5, 1, 1, 1, 1
  ///         |
  ///         |
  ///     this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &endKnotLeftV ( SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( 1, whichEndKnot );
  }

  IndependentType &endKnotLeftV ( SizeType whichEndKnot )
  {
    return BaseClass::endKnotLeft ( 1, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the right of the first knot vector.
  ///
  /// For example, if the surface is bi-cubic with 7x5 control points, then 
  /// endKnotLeftU(2) will return:
  ///
  ///   0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1
  ///                                      |
  ///                                      |
  ///                                  this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &endKnotRightU ( SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( 0, whichEndKnot );
  }

  IndependentType &endKnotRightU ( SizeType whichEndKnot )
  {
    return BaseClass::endKnotRight ( 0, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the end-knot on the right of the second knot vector.
  ///
  /// For example, if the surface is bi-cubic with 7x5 control points, then 
  /// endKnotLeftV(2) will return:
  ///
  ///   0, 0, 0, 0, 0.5, 1, 1, 1, 1
  ///                          |
  ///                          |
  ///                      this knot
  ///
  /////////////////////////////////////////////////////////////////////////////

  const IndependentType &endKnotRightV ( SizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( 1, whichEndKnot );
  }

  IndependentType &endKnotRightV ( SizeType whichEndKnot )
  {
    return BaseClass::endKnotRight ( 1, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Set this surface. This is here for type safety.
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
  /// See if the surfaces are exactly equal. This is here for type safety.
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const ThisType &s ) const
  {
    return BaseClass::equal ( s );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the surfaces are equal. The argument "pred" is a predicate-functor 
  /// that allows for special comparisons, such as being within a tolerance.
  ///
  /////////////////////////////////////////////////////////////////////////////

  template < class Predicate_ > bool equal ( const ThisType &s, const Predicate_ &pred ) const
  {
    return BaseClass::equal ( s, pred );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Return a reference to this surface as a spline.
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
  ( const GN::Splines::Surface<ConfigType> &s1, const GN::Splines::Surface<ConfigType> &s2 )
{
  return s1.equal ( s2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class ConfigType > inline bool operator != 
  ( const GN::Splines::Surface<ConfigType> &s1, const GN::Splines::Surface<ConfigType> &s2 )
{
  return !( s1.equal ( s2 ) );
}


#endif // _GENERIC_NURBS_LIBRARY_SURFACE_CLASS_H_
