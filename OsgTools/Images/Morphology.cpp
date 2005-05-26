
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/Morphology.h"

#include "osg/Image"

///////////////////////////////////////////////////////////////////////////////
//
//  Erode the image
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::erode ( osg::Image& image, unsigned int r, unsigned int c )
{
  // make a copy to work from
  osg::ref_ptr< osg::Image > copy ( new osg::Image ( image, osg::CopyOp::DEEP_COPY_ALL ) );

  const unsigned int centerX ( r/2 );
  const unsigned int centerY ( c/2 );

  for( int i = 1; i < copy->s() - 1; ++ i )
  {
    for ( int j = 1; j < copy->t() - 1; ++j )
    {
      if ( *copy->data( i - 1, j - 1 ) == 0 &&
          *copy->data( i,     j - 1 ) == 0 &&
          *copy->data( i + 1, j - 1 ) == 0 &&
          *copy->data( i - 1, j )     == 0 &&
          *copy->data( i,     j )     == 0 &&
          *copy->data( i + 1, j )     == 0 &&
          *copy->data( i - 1, j + 1 ) == 0 &&
          *copy->data( i,     j + 1 ) == 0 &&
          *copy->data( i + 1, j + 1 ) == 0 )
      {
        *image.data( i, j ) = 0;
        
      }
      else
      {
        *image.data( i, j ) = 255;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dilate the image
//  TODO should have option on whether or not to expand the borders
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::dilate ( osg::Image& image, unsigned int r, unsigned int c )
{
  // make a copy to work from
  osg::ref_ptr< osg::Image > copy ( new osg::Image ( image, osg::CopyOp::DEEP_COPY_ALL ) );

  const int centerX ( r/2 );
  const int centerY ( c/2 );

  for( int i = centerX; i < copy->s() - centerX; ++ i )
  {
    for ( int j = centerY; j < copy->t() - centerY; ++j )
    {
      if ( *copy->data( i, j ) == 0 )
      {
#if 0
        *image.data( i - 1, j - 1 ) = 0;
        *image.data( i,     j - 1 ) = 0;
        *image.data( i + 1, j - 1 ) = 0;
        *image.data( i - 1, j )     = 0;
        *image.data( i,     j )     = 0;
        *image.data( i + 1, j )     = 0;
        *image.data( i - 1, j + 1 ) = 0;
        *image.data( i,     j + 1 ) = 0;
        *image.data( i + 1, j + 1 ) = 0;
#endif
        for( int s = -centerX; s < centerX; ++s )
        {
          for( int t = -centerY; t < centerY; ++t )
          {
            *image.data( i + s, j + t )     = 0;
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the image
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::open ( osg::Image& image, unsigned int r, unsigned int c )
{
  erode ( image, r, c );
  dilate ( image, r, c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the image
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::close ( osg::Image& image, unsigned int r, unsigned int c )
{
  dilate ( image, r, c );
  erode ( image, r, c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convolve the image
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::convolve ( osg::Image& image, const Matrix<int>& mask )
{
  // make a copy to work from
  osg::ref_ptr< osg::Image > copy ( new osg::Image ( image, osg::CopyOp::DEEP_COPY_ALL ) );

  const unsigned int centerX ( 1 );
  const unsigned int centerY ( 1 );

  // Resize the image
  image.scaleImage( image.s() - centerX, image.t() - centerY, image.r(), image.getDataType() );

  for( int i = 1; i < copy->s() - 1; ++ i )
  {
    for ( int j = 1; j < copy->t() - 1; ++j )
    {
      int value ( 0 );
      value += mask.at(0,0) * *copy->data( i - 1, j - 1 );
      value += mask.at(1,0) * *copy->data( i,     j - 1 );
      value += mask.at(2,0) * *copy->data( i + 1, j - 1 );

      value += mask.at(0,1) * *copy->data( i - 1, j );
      value += mask.at(1,1) * *copy->data( i,     j );
      value += mask.at(2,1) * *copy->data( i + 1, j );

      value += mask.at(0,2) * *copy->data( i - 1, j + 1 );
      value += mask.at(1,2) * *copy->data( i,     j + 1 );
      value += mask.at(2,2) * *copy->data( i + 1, j + 1 );

      *image.data( i - 1, j - 1 ) = value;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth image
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::smooth    ( osg::Image& image, const Matrix<int>& mask )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply a mask to the image
//
///////////////////////////////////////////////////////////////////////////////

template < class Mask >
void OsgTools::Images::applyMask ( osg::Image& image, const Mask& mask )
{
  // make a copy to work from
  osg::ref_ptr< osg::Image > ( new osg::Image ( image, osg::CopyOp::DEEP_COPY_ALL ) );


}
