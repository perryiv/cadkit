
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

#include "GN/Config/BaseClass.h"

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

#define GN_DESTRUCTOR_TYPE protected: virtual


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespaces.
//
///////////////////////////////////////////////////////////////////////////////

namespace GN {
namespace osg {
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

  typedef Detail::ThrowingPolicy  ErrorCheckerType;
  typedef Detail::StringData      BaseClassType;

  typedef unsigned int UIntType;
  typedef RealType_    KnotType;
  typedef RealType_    ControlPointType;

private:

  typedef std::vector<KnotType>         OneKnotVector;
  typedef std::vector<ControlPointType> OneCtrPtCoord;

public:

  typedef std::vector<UIntType>      UIntContainer;
  typedef std::vector<OneKnotVector> KnotContainer;
  typedef std::vector<OneCtrPtCoord> ControlPointContainer;

  typedef std::vector<ControlPointType> Vector;

  typedef ::osg::Vec2       Vec2;
  typedef ::osg::Vec3       Vec3;
  typedef ::osg::Vec4       Vec4;
  typedef ::osg::Matrixf    Matrix44;

  typedef Detail::FloatTester<KnotType>           KnotTester;
  typedef Detail::FloatTester<ControlPointType>   ControlPointTester;
  typedef Detail::Translation<Matrix44,Vec3>      Translation;
  typedef Detail::Scale<Matrix44,Vec3>            Scale;
  typedef Detail::Multiply<Matrix44,Vec4,Vec3>    Multiply;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of remaining namespaces.
//
///////////////////////////////////////////////////////////////////////////////

}; // namespace osg
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_OSG_CONFIGURATIONS_H_
