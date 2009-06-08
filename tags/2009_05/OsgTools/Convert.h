
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Converter functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_CONVERTERS_H_
#define _OSG_TOOLS_CONVERTERS_H_

#include "osg/Matrix"

#include "Usul/Convert/Convert.h"
#include "Usul/Math/Matrix44.h"


namespace OsgTools {
namespace Convert {


///////////////////////////////////////////////////////////////////////////////
//
//  Transpose the matrix.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class FromType, class ToType > 
  inline void transpose ( const FromType &f, ToType &t )
  {
    t(0,0) = f(0,0);
    t(1,0) = f(0,1);
    t(2,0) = f(0,2);
    t(3,0) = f(0,3);

    t(0,1) = f(1,0);
    t(1,1) = f(1,1);
    t(2,1) = f(1,2);
    t(3,1) = f(1,3);

    t(0,2) = f(2,0);
    t(1,2) = f(2,1);
    t(2,2) = f(2,2);
    t(3,2) = f(2,3);

    t(0,3) = f(3,0);
    t(1,3) = f(3,1);
    t(2,3) = f(3,2);
    t(3,3) = f(3,3);
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the matrix. An OSG matrix is the transpose of an OpenGL matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class B > 
inline void matrix ( const Usul::Math::Matrix44<T,I,B> &f, osg::Matrixf &t )
{
  Detail::transpose ( f, t );
}
template < class T, class I, class B > 
inline void matrix ( const osg::Matrixf &f, Usul::Math::Matrix44<T,I,B> &t )
{
  Detail::transpose ( f, t );
}
template < class T, class I, class B > 
inline void matrix ( const Usul::Math::Matrix44<T,I,B> &f, osg::Matrixd &t )
{
  Detail::transpose ( f, t );
}
template < class T, class I, class B > 
inline void matrix ( const osg::Matrixd &f, Usul::Math::Matrix44<T,I,B> &t )
{
  Detail::transpose ( f, t );
}


template < class F, class T >
inline void vector ( const F& from, T& to, unsigned int size )
{
  std::copy( &from[0], &from[0] + size, &to[0] );
}


} // namespace Convert
} // namespace OsgTools

#define OSGTOOLS_CONVERT_VEC4(from_type,to_type) \
namespace Usul{ namespace Convert  { \
template <> struct Type < from_type, to_type > \
{ \
typedef Type < from_type, to_type > ThisType; \
static void convert ( const from_type& from, to_type &to ) \
{ \
  to.set ( from[0], from[1], from[2], from[3] ); \
} \
static to_type convert ( const from_type& from )\
{\
  to_type to;\
  ThisType::convert ( from, to ); \
  return to;\
} }; } }

#define OSGTOOLS_CONVERT_VEC3(from_type,to_type) \
namespace Usul{ namespace Convert  { \
template <> struct Type < from_type, to_type > \
{ \
typedef Type < from_type, to_type > ThisType; \
static void convert ( const from_type& from, to_type &to ) \
{ \
  to.set ( from[0], from[1], from[2] ); \
} \
static to_type convert ( const from_type& from )\
{\
  to_type to;\
  ThisType::convert ( from, to ); \
  return to;\
} }; } }

OSGTOOLS_CONVERT_VEC4(Usul::Math::Vec4f,osg::Vec4f)
OSGTOOLS_CONVERT_VEC4(osg::Vec4f,Usul::Math::Vec4f)

OSGTOOLS_CONVERT_VEC4(Usul::Math::Vec4d,osg::Vec4d)
OSGTOOLS_CONVERT_VEC4(osg::Vec4d,Usul::Math::Vec4d)

OSGTOOLS_CONVERT_VEC3(Usul::Math::Vec3f,osg::Vec3f)
OSGTOOLS_CONVERT_VEC3(osg::Vec3f,Usul::Math::Vec3f)

OSGTOOLS_CONVERT_VEC3(Usul::Math::Vec3d,osg::Vec3d)
OSGTOOLS_CONVERT_VEC3(osg::Vec3d,Usul::Math::Vec3d)

#endif // _OSG_TOOLS_CONVERTERS_H_
