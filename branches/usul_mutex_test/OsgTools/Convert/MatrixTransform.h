
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String converter for osg::MatrixTransform.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_STRING_MATRIX_TRANSFORM_CONVERSION_H_
#define _OSG_TOOLS_STRING_MATRIX_TRANSFORM_CONVERSION_H_

#include "OsgTools/Convert/Matrix.h"

#include "osg/MatrixTransform"


namespace Usul {
namespace Convert {


typedef osg::ref_ptr < osg::MatrixTransform > MatrixTransformPtr;


template <> struct Type < MatrixTransformPtr, std::string >
{
  typedef Type < MatrixTransformPtr, std::string > ThisType;
  static void convert ( const MatrixTransformPtr &from, std::string &to )
  {
    to.clear();
    if ( true == from.valid() )
    {
      Usul::Convert::Type<osg::Matrixd,std::string>::convert ( from->getMatrix(), to );
    }
  }
  static std::string convert ( const MatrixTransformPtr &from )
  {
    std::string to;
    ThisType::convert ( from, to );
    return to;
  }
};
template <> struct Type < std::string, MatrixTransformPtr >
{
  typedef Type < std::string, MatrixTransformPtr > ThisType;
  static void convert ( const std::string &from, MatrixTransformPtr &to )
  {
    osg::Matrixd matrix;
    Usul::Convert::Type<std::string,osg::Matrixd>::convert ( from, matrix );
    if ( false == to.valid() )
    {
      to = new osg::MatrixTransform;
    }
    to->setMatrix ( matrix );
  }
  static MatrixTransformPtr convert ( const std::string &from )
  {
    MatrixTransformPtr to ( 0x0 );
    ThisType::convert ( from, to );
    return to;
  }
};


} // namespace Convert
} // namespace Usul


#endif // _OSG_TOOLS_STRING_MATRIX_TRANSFORM_CONVERSION_H_
