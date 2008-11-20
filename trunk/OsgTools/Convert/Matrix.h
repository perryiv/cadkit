
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String converter for osg::Matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_STRING_MATRIX_CONVERSION_H_
#define _OSG_TOOLS_STRING_MATRIX_CONVERSION_H_

#include "Usul/Convert/Convert.h"
#include "Usul/Math/Matrix44.h"

#include "osg/Matrixf"
#include "osg/Matrixd"


USUL_CONVERT_DEFINE_STRING_CONVERTER_MATRIX_4_4 ( osg::Matrixd );
USUL_CONVERT_DEFINE_STRING_CONVERTER_MATRIX_4_4 ( osg::Matrixf );


namespace Usul {
namespace Convert {


template <> struct Type < osg::Matrixd, Usul::Math::Matrix44d >
{
  typedef Type < osg::Matrixd, Usul::Math::Matrix44d > ThisType;
  static void convert ( const osg::Matrixd &f, Usul::Math::Matrix44d &t )
  {
    // Transpose because the conventions are opposite.
    t.set ( f(0,0), f(1,0), f(2,0), f(3,0),
            f(0,1), f(1,1), f(2,1), f(3,1),
            f(0,2), f(1,2), f(2,2), f(3,2),
            f(0,3), f(1,3), f(2,3), f(3,3) );
  }
  static Usul::Math::Matrix44d convert ( const osg::Matrixd &from )
  {
    Usul::Math::Matrix44d to;
    ThisType::convert ( from, to );
    return to;
  }
};
template <> struct Type < Usul::Math::Matrix44d, osg::Matrixd >
{
  typedef Type < Usul::Math::Matrix44d, osg::Matrixd > ThisType;
  static void convert ( const Usul::Math::Matrix44d &f, osg::Matrixd &t )
  {
    // Transpose because the conventions are opposite.
    t.set ( f(0,0), f(1,0), f(2,0), f(3,0),
            f(0,1), f(1,1), f(2,1), f(3,1),
            f(0,2), f(1,2), f(2,2), f(3,2),
            f(0,3), f(1,3), f(2,3), f(3,3) );
  }
  static osg::Matrixd convert ( const Usul::Math::Matrix44d &from )
  {
    osg::Matrixd to;
    ThisType::convert ( from, to );
    return to;
  }
};


} // namespace Convert
} // namespace Usul


#endif // _OSG_TOOLS_STRING_MATRIX_CONVERSION_H_
