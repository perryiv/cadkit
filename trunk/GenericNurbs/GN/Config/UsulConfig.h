
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Configuration for using Usul with GenericNurbs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_USUL_CONFIGURATIONS_H_
#define _GENERIC_NURBS_LIBRARY_USUL_CONFIGURATIONS_H_

#include "GN/Config/BaseClass.h"

#include "Usul/Containers/Vector.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Finite.h"
#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Errors/PrintingPolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/DoNothingPolicy.h"

#include <vector>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Switch for virtual destructors.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _GN_USE_VIRTUAL_DESTRUCTORS
# define GN_DESTRUCTOR_TYPE virtual
#else
# define GN_DESTRUCTOR_TYPE
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespaces.
//
///////////////////////////////////////////////////////////////////////////////

namespace GN {
namespace Usul {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Class for testing floating-point numbers.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct FloatTester
{
  static bool finite ( const T &v )
  {
    return ::Usul::Math::finite ( v );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Adaptor class for making a 4x4 translation matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class Matrix44, class Vec3 > struct Translation
{
  static void make ( Matrix44 &m, const Vec3 &v )
  {
    m.makeTranslation ( v );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Adaptor class for making a 4x4 scale matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class Matrix44, class Vec3 > struct Scale
{
  static void make ( Matrix44 &m, const Vec3 &v )
  {
    m.makeScale ( v );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Adaptor class for multiplying a matrix and a vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class Matrix44, class Vec4, class Vec3 > struct Multiply
{
  static Vec4 calculate ( const Matrix44 &m, const Vec4 &v )
  {
    return ( m * v );
  }
  static Vec3 calculate ( const Matrix44 &m, const Vec3 &v )
  {
    return ( m * v );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking type that asserts and then throws.
//
///////////////////////////////////////////////////////////////////////////////

typedef ::Usul::Errors::CompositePolicy
<
  ::Usul::Errors::AssertPolicy, 
  ::Usul::Errors::ThrowingPolicy < std::runtime_error >
>
ErrorCheckerAssertAndThrow;


///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking type that prints and then throws.
//
///////////////////////////////////////////////////////////////////////////////

typedef ::Usul::Errors::CompositePolicy
<
  ::Usul::Errors::PrintingPolicy, 
  ::Usul::Errors::ThrowingPolicy < std::runtime_error >
>
ErrorCheckerPrintAndThrow;


///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking type depends on the compiler.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
  #ifdef _MSC_VER
    typedef ErrorCheckerAssertAndThrow    ErrorCheckerType;
  #else
    typedef ErrorCheckerPrintAndThrow     ErrorCheckerType;
  #endif
#else
  typedef ::Usul::Errors::DoNothingPolicy ErrorCheckerType;
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Class for calculating the distance between two n-dimensional vectors.
//
///////////////////////////////////////////////////////////////////////////////

template < class V > struct Distance
{
  typedef typename V::value_type value_type;
  typedef typename V::const_iterator const_iterator;
  static value_type squared ( const value_type &v1, const value_type &v2 )
  {
    ErrorCheckerType ( __FILE__, __LINE__, v1.size() == v2.size() );
    value_type dist ( 0 ), temp;
    const_iterator i1 = v1.begin();
    const_iterator i2 = v2.begin();
    while ( i1 != v1.end() && i2 != v2.end() )
    {
      dist += ::Usul::Math::square ( temp * temp );
    }
    return dist;
  }
  static value_type get ( const value_type &v1, const value_type &v2 )
  {
    return ::Usul::Math::sqrt ( squared ( v1, v2 ) );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

};


///////////////////////////////////////////////////////////////////////////////
//
//  Configuration that uses std::vector. This class could be a template that 
//  takes most of these types as parameters. However, that makes compile 
//  errors very difficult to read.
//
///////////////////////////////////////////////////////////////////////////////

template < class RealType_ > class Config
{
public:

  typedef Detail::ErrorCheckerType      ErrorCheckerType;
  typedef GN::Config::Base::StringData  BaseClassType;

  typedef unsigned int UIntType;
  typedef RealType_    KnotType;
  typedef RealType_    ControlPointType;

private:

  typedef std::vector<UIntType>         UIntContainerInner;
  typedef std::vector<KnotType>         OneKnotVectorInner;
  typedef std::vector<ControlPointType> OneCtrPtCoordInner;

  typedef ::Usul::Containers::Vector<OneKnotVectorInner,ErrorCheckerType> OneKnotVectorOuter;
  typedef ::Usul::Containers::Vector<OneCtrPtCoordInner,ErrorCheckerType> OneCtrPtCoordOuter;

  typedef std::vector<OneKnotVectorOuter> AllKnotVectorsInner;
  typedef std::vector<OneCtrPtCoordOuter> AllCtrPtsCoordInner;

public:

  typedef ::Usul::Containers::Vector<UIntContainerInner, ErrorCheckerType> UIntContainer;
  typedef ::Usul::Containers::Vector<AllKnotVectorsInner,ErrorCheckerType> KnotContainer;
  typedef ::Usul::Containers::Vector<AllCtrPtsCoordInner,ErrorCheckerType> ControlPointContainer;

  typedef std::vector<ControlPointType> Vector;

  typedef ::Usul::Math::Vector2<ControlPointType>  Vec2;
  typedef ::Usul::Math::Vector3<ControlPointType>  Vec3;
  typedef ::Usul::Math::Vector4<ControlPointType>  Vec4;
  typedef ::Usul::Math::Matrix44<ControlPointType> Matrix44;

  typedef Detail::FloatTester<KnotType>           KnotTester;
  typedef Detail::FloatTester<ControlPointType>   ControlPointTester;
  typedef Detail::Translation<Matrix44,Vec3>      Translation;
  typedef Detail::Scale<Matrix44,Vec3>            Scale;
  typedef Detail::Multiply<Matrix44,Vec4,Vec3>    Multiply;
  typedef Detail::Distance<Vector>                Distance;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of remaining namespaces.
//
///////////////////////////////////////////////////////////////////////////////

}; // namespace Usul
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_USUL_CONFIGURATIONS_H_
