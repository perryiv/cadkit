
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Configuration for using GMTL with GenericNurbs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_GMTL_CONFIGURATIONS_H_
#define _GENERIC_NURBS_LIBRARY_GMTL_CONFIGURATIONS_H_

#include "GN/Config/BaseClass.h"

#include "gmtl/gmtl.h"

#include <vector>
#include <stdexcept>
#include <sstream>

#ifdef _MSC_VER
#include <float.h>
#elif __CYGWIN__
#include <ieeefp.h>
#else
#include <math.h>
#endif


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
namespace gmtl {
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
    #ifdef _MSC_VER
    return ( 0 != ::_finite ( double ( v ) ) );
    #else
    return ( 0 != ::finite ( double ( v ) ) );
    #endif
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
    return ::sqrt ( v );
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
    return ::pow ( value, power );
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
    ::gmtl::identity ( m );
    ::gmtl::setTrans ( m, v );
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
    ::gmtl::identity ( m );
    ::gmtl::setScale ( m, v );
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
//  Error-checking class that throws.
//
///////////////////////////////////////////////////////////////////////////////

struct ThrowingPolicy
{
  ThrowingPolicy ( const char *filename, unsigned int line, bool state )
  {
    if ( !state )
    {
      std::ostringstream message;
      message << "Error!\n\tLine: " << line << "\n\tFile: " << filename;
      throw std::runtime_error ( message.str() );
    }
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

  typedef Detail::ThrowingPolicy        ErrorCheckerType;
  typedef GN::Config::Base::StringData  BaseClassType;

  typedef unsigned int SizeType;
  typedef RealType_    IndependentType;
  typedef RealType_    DependentType;

private:

  typedef std::vector<IndependentType>  IndependentSequence;
  typedef std::vector<DependentType>    DependentSequence;

public:

  typedef std::vector<SizeType>             SizeContainer;
  typedef std::vector<IndependentSequence>  IndependentContainer;
  typedef std::vector<DependentSequence>    DependentContainer;

  typedef std::vector<DependentType> Vector;

  typedef ::gmtl::Vec<DependentType,2>       Vec2;
  typedef ::gmtl::Vec<DependentType,3>       Vec3;
  typedef ::gmtl::Vec<DependentType,4>       Vec4;
  typedef ::gmtl::Matrix<DependentType,4,4>  Matrix44;

  typedef Detail::FloatTester<IndependentType> IndependentTester;
  typedef Detail::FloatTester<DependentType>   DependentTester;
  typedef Detail::Translation<Matrix44,Vec3>   Translation;
  typedef Detail::Scale<Matrix44,Vec3>         Scale;
  typedef Detail::Multiply<Matrix44,Vec4,Vec3> Multiply;
  typedef Detail::SquareRoot<DependentType>    SquareRoot;
  typedef Detail::Power<DependentType>         Power;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of remaining namespaces.
//
///////////////////////////////////////////////////////////////////////////////

}; // namespace gmtl
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_GMTL_CONFIGURATIONS_H_
