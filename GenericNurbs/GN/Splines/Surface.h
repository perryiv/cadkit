
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
  typedef typename BaseClass::UIntType            UIntType;
  typedef typename BaseClass::UIntContainer       UIntContainer;
  typedef typename BaseClass::KnotType            KnotType;
  typedef typename BaseClass::KnotVectorSizeType  KnotVectorSizeType;
  typedef typename BaseClass::KnotArgument        KnotArgument;
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

  GN_DESTRUCTOR_TYPE ~Surface()
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

  UIntType orderU() const
  {
    return BaseClass::order ( 0 );
  }

  UIntType orderV() const
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

  UIntType degreeU() const
  {
    return BaseClass::degree ( 0 );
  }

  UIntType degreeV() const
  {
    return BaseClass::degree ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of control points.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numControlPointsU() const
  {
    return BaseClass::numControlPoints ( 0 );
  }

  UIntType numControlPointsV() const
  {
    return BaseClass::numControlPoints ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the number of knots.
  ///
  /////////////////////////////////////////////////////////////////////////////

  UIntType numKnotsU() const
  {
    return BaseClass::numKnots ( 0 );
  }

  UIntType numKnotsV() const
  {
    return BaseClass::numKnots ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &knotU ( KnotVectorSizeType whichKnot ) const
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  const KnotType &knotV ( KnotVectorSizeType whichKnot ) const
  {
    return BaseClass::knot ( 1, whichKnot );
  }

  KnotType &knotU ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichKnot )
  {
    return BaseClass::knot ( 0, whichKnot );
  }

  KnotType &knotV ( KnotVectorSizeType whichIndepVar, KnotVectorSizeType whichKnot )
  {
    return BaseClass::knot ( 1, whichKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the first knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &firstKnotU() const
  {
    return BaseClass::firstKnot ( 0 );
  }

  const KnotType &firstKnotV() const
  {
    return BaseClass::firstKnot ( 1 );
  }

  KnotType &firstKnotU()
  {
    return BaseClass::firstKnot ( 0 );
  }

  KnotType &firstKnotV()
  {
    return BaseClass::firstKnot ( 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Get the last knot.
  ///
  /////////////////////////////////////////////////////////////////////////////

  const KnotType &lastKnotU() const
  {
    return BaseClass::lastKnot ( 0 );
  }

  const KnotType &lastKnotV() const
  {
    return BaseClass::lastKnot ( 1 );
  }

  KnotType &lastKnotU()
  {
    return BaseClass::lastKnot ( 0 );
  }

  KnotType &lastKnotV()
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

  const KnotType &endKnotLeftU ( KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( 0, whichEndKnot );
  }

  KnotType &endKnotLeftU ( KnotVectorSizeType whichEndKnot )
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

  const KnotType &endKnotLeftV ( KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotLeft ( 1, whichEndKnot );
  }

  KnotType &endKnotLeftV ( KnotVectorSizeType whichEndKnot )
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

  const KnotType &endKnotRightU ( KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( 0, whichEndKnot );
  }

  KnotType &endKnotRightU ( KnotVectorSizeType whichEndKnot )
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

  const KnotType &endKnotRightV ( KnotVectorSizeType whichEndKnot ) const
  {
    return BaseClass::endKnotRight ( 1, whichEndKnot );
  }

  KnotType &endKnotRightV ( KnotVectorSizeType whichEndKnot )
  {
    return BaseClass::endKnotRight ( 1, whichEndKnot );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Resize the surface.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void resize ( UIntType dimension, 
                UIntType orderU, 
                UIntType orderV, 
                UIntType numCtrPtsU, 
                UIntType numCtrPtsV, 
                bool rational )
  {
    UIntContainer o;
    o.resize ( 2 );
    o[0] = orderU;
    o[1] = orderV;

    UIntContainer n;
    n.resize ( 2 );
    n[0] = numCtrPtsU;
    n[1] = numCtrPtsV;

    BaseClass::resize ( dimension, o, n, rational );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Set this surface. This is here for type safety.
  ///
  /////////////////////////////////////////////////////////////////////////////

  void set ( const ThisType &c )
  {
    // Call base class' function.
    BaseClass::set ( s );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// Assignment operator.
  ///
  /////////////////////////////////////////////////////////////////////////////

  ThisType &operator = ( const ThisType &c )
  {
    this->set ( s );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the surfaces are exactly equal. This is here for type safety.
  ///
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const ThisType &c ) const
  {
    return BaseClass::equal ( c );
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  /// See if the surfaces are equal. The argument "pred" is a predicate-functor 
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
