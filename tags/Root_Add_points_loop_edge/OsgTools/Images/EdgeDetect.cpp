
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "EdgeDetect.h"

#include "osg/Image"

#include "Matrix.h"

using namespace OsgTools::Images;


osg::Image* OsgTools::Images::edgeDetect ( const osg::Image& image, const Matrix<int>& maskX, const Matrix<int>& maskY )
{
  osg::ref_ptr< osg::Image > copy ( new osg::Image );

  copy->allocateImage( image.s() - 1, image.t() - 1, 1, image.getPixelFormat(), image.getDataType() );

  for( int i = 1; i < image.s() - 1; ++i )
  {
    for( int j = 1; j < image.t() - 1; ++j )
    {
      int valueOne ( 0 );
      valueOne += maskX.at(0,0) * *image.data( i - 1, j - 1 );
      valueOne += maskX.at(1,0) * *image.data( i,     j - 1 );
      valueOne += maskX.at(2,0) * *image.data( i + 1, j - 1 );

      valueOne += maskX.at(0,1) * *image.data( i - 1, j );
      valueOne += maskX.at(1,1) * *image.data( i,     j );
      valueOne += maskX.at(2,1) * *image.data( i + 1, j );

      valueOne += maskX.at(0,2) * *image.data( i - 1, j + 1 );
      valueOne += maskX.at(1,2) * *image.data( i,     j + 1 );
      valueOne += maskX.at(2,2) * *image.data( i + 1, j + 1 );

      int valueTwo ( 0 );
      valueTwo += maskY.at(0,0) * *image.data( i - 1, j - 1 );
      valueTwo += maskY.at(1,0) * *image.data( i,     j - 1 );
      valueTwo += maskY.at(2,0) * *image.data( i + 1, j - 1 );

      valueTwo += maskY.at(0,1) * *image.data( i - 1, j );
      valueTwo += maskY.at(1,1) * *image.data( i,     j );
      valueTwo += maskY.at(2,1) * *image.data( i + 1, j );

      valueTwo += maskY.at(0,2) * *image.data( i - 1, j + 1 );
      valueTwo += maskY.at(1,2) * *image.data( i,     j + 1 );
      valueTwo += maskY.at(2,2) * *image.data( i + 1, j + 1 );

      *copy->data(i,j) = ::abs( valueOne ) + ::abs( valueTwo );
    }
  }

  return copy.release();
}

