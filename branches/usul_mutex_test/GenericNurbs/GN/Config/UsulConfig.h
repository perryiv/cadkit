
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

//#define GN_USE_VIRTUAL_PROTECTED_DESTRUCTORS


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespaces.
//
///////////////////////////////////////////////////////////////////////////////

namespace GN {
namespace Config {
namespace UsulDetail {


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
//  Class for calculating the square root.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct SquareRoot
{
  static T calculate ( const T &v )
  {
    return ::Usul::Math::sqrt ( v );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class for calculating the power.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct Power
{
  static T calculate ( const T &value, const T &power )
  {
    return ::Usul::Math::pow ( value, power );
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
//  Error-checking type that just throws.
//
///////////////////////////////////////////////////////////////////////////////

typedef ::Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorCheckerThrow;


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
//  End of namespace UsulDetail.
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

template
<
  class IndependentType_, 
  class DependentType_ = IndependentType_,
  class SizeType_ = unsigned int,
  class ErrorCheckerType_ = UsulDetail::ErrorCheckerThrow
>
class UsulConfig
{
public:

  typedef ErrorCheckerType_             ErrorCheckerType;
  typedef GN::Config::Base::StringData  BaseClassType;

  typedef IndependentType_  IndependentType;
  typedef DependentType_    DependentType;
  typedef SizeType_         SizeType;

private:

  typedef std::vector<SizeType>         SizeContainerInner;
  typedef std::vector<IndependentType>  OneIndependentInner;
  typedef std::vector<DependentType>    OneDependentInner;

  typedef ::Usul::Containers::Vector<OneIndependentInner,ErrorCheckerType> OneIndependentOuter;
  typedef ::Usul::Containers::Vector<OneDependentInner,  ErrorCheckerType> OneDependentOuter;

  typedef std::vector<OneIndependentOuter> MultipleIndependentInner;
  typedef std::vector<OneDependentOuter>   MultipleDependentInner;

public:

  typedef ::Usul::Containers::Vector<SizeContainerInner,      ErrorCheckerType> SizeContainer;
  typedef ::Usul::Containers::Vector<MultipleIndependentInner,ErrorCheckerType> IndependentContainer;
  typedef ::Usul::Containers::Vector<MultipleDependentInner,  ErrorCheckerType> DependentContainer;

  typedef std::vector<DependentType> Vector;

  typedef ::Usul::Math::Vector2<DependentType>  Vec2;
  typedef ::Usul::Math::Vector3<DependentType>  Vec3;
  typedef ::Usul::Math::Vector4<DependentType>  Vec4;
  typedef ::Usul::Math::Matrix44<DependentType> Matrix44;

  typedef UsulDetail::FloatTester<IndependentType> IndependentTester;
  typedef UsulDetail::FloatTester<DependentType>   DependentTester;
  typedef UsulDetail::Translation<Matrix44,Vec3>   Translation;
  typedef UsulDetail::Scale<Matrix44,Vec3>         Scale;
  typedef UsulDetail::Multiply<Matrix44,Vec4,Vec3> Multiply;
  typedef UsulDetail::SquareRoot<DependentType>    SquareRoot;
  typedef UsulDetail::Power<DependentType>         Power;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of remaining namespaces.
//
///////////////////////////////////////////////////////////////////////////////

}; // namespace Config
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_USUL_CONFIGURATIONS_H_
