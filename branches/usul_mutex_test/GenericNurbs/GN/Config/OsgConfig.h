
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Configuration for using OpenSceneGraph with GenericNurbs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_OSG_CONFIGURATIONS_H_
#define _GENERIC_NURBS_LIBRARY_OSG_CONFIGURATIONS_H_

#include "osg/Vec4"
#include "osg/Vec3"
#include "osg/Vec2"
#include "osg/Matrix"
#include "osg/Referenced"

#include <vector>
#include <stdexcept>
#include <sstream>
#include <math.h>


///////////////////////////////////////////////////////////////////////////////
//
//  Switch for virtual destructors.
//
///////////////////////////////////////////////////////////////////////////////

#define GN_USE_VIRTUAL_PROTECTED_DESTRUCTORS


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespaces.
//
///////////////////////////////////////////////////////////////////////////////

namespace GN {
namespace Config {
namespace OsgDetail {


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
    m.makeTranslate ( v );
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
    return ( v * m );
  }
  static Vec3 calculate ( const Matrix44 &m, const Vec3 &v )
  {
    return ( v * m );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking class that throws.
//
///////////////////////////////////////////////////////////////////////////////

struct ThrowingPolicy
{
  ThrowingPolicy ( const char *filename, unsigned int line, bool state, const char *message = 0x0 )
  {
    if ( !state )
    {
      std::ostringstream out;
      out << "Error!\n\tLine: " << line << "\n\tFile: " << filename;
      if ( message )
        out << "\n\tMessage: " << message;
      throw std::runtime_error ( out.str() );
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Base class that holds a string. The only reason for this layer, rather 
//  than making osg::Referenced the immediate base class, is that one of the 
//  functions in the test-program uses the member getStringData().
//
///////////////////////////////////////////////////////////////////////////////

struct StringData : ::osg::Referenced
{
  StringData() : _string(){}
  StringData ( const std::string &s ) : _string ( s ){}
  StringData ( const StringData &b ) : _string ( b._string ){}
  StringData &operator = ( const StringData &b ) { this->set ( b ); return *this; }
  void                  set ( const StringData &b ) { _string = b._string; }
  const std::string &   getStringData() const { return _string; }
  void                  setStringData ( const std::string &s ) { _string = s; }
protected:
  virtual ~StringData(){}
private:
  std::string _string;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace OsgDetail.
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
  class SizeType_ = unsigned int
>
class OsgConfig
{
public:

  typedef OsgDetail::ThrowingPolicy  ErrorCheckerType;
  typedef OsgDetail::StringData      BaseClassType;

  typedef IndependentType_  IndependentType;
  typedef DependentType_    DependentType;
  typedef SizeType_         SizeType;

private:

  typedef std::vector<IndependentType>  IndependentSequence;
  typedef std::vector<DependentType>    DependentSequence;

public:

  typedef std::vector<SizeType>             SizeContainer;
  typedef std::vector<IndependentSequence>  IndependentContainer;
  typedef std::vector<DependentSequence>    DependentContainer;

  typedef std::vector<DependentType> Vector;

  typedef ::osg::Vec2       Vec2;
  typedef ::osg::Vec3       Vec3;
  typedef ::osg::Vec4       Vec4;
  typedef ::osg::Matrixf    Matrix44;

  typedef OsgDetail::FloatTester<IndependentType>  IndependentTester;
  typedef OsgDetail::FloatTester<DependentType>    DependentTester;
  typedef OsgDetail::Translation<Matrix44,Vec3>    Translation;
  typedef OsgDetail::Scale<Matrix44,Vec3>          Scale;
  typedef OsgDetail::Multiply<Matrix44,Vec4,Vec3>  Multiply;
  typedef OsgDetail::SquareRoot<DependentType>     SquareRoot;
  typedef OsgDetail::Power<DependentType>          Power;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of remaining namespaces.
//
///////////////////////////////////////////////////////////////////////////////

}; // namespace Config
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_OSG_CONFIGURATIONS_H_
