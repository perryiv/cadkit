
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_MORPHOLOGY_H__
#define __IMAGES_MORPHOLOGY_H__

#include "Images/Matrix.h"

namespace Images {

///////////////////////////////////////////////////////////////////////////////
//
//  Erode the image
//  TODO should have option on whether or not to expand the borders
//
///////////////////////////////////////////////////////////////////////////////

template < class Image > void  erode   ( Image& image, unsigned int r, unsigned int c )
{
  // Make a copy to work from
  Image copy ( image );

  const unsigned int centerX ( r/2 );
  const unsigned int centerY ( c/2 );

  for( unsigned int i = 1; i < copy.width() - 1; ++ i )
  {
    for ( unsigned int j = 1; j < copy.height() - 1; ++j )
    {
      if ( copy( i - 1, j - 1 ) == 0 &&
           copy( i,     j - 1 ) == 0 &&
           copy( i + 1, j - 1 ) == 0 &&
           copy( i - 1, j )     == 0 &&
           copy( i,     j )     == 0 &&
           copy( i + 1, j )     == 0 &&
           copy( i - 1, j + 1 ) == 0 &&
           copy( i,     j + 1 ) == 0 &&
           copy( i + 1, j + 1 ) == 0 )
      {
        image( i, j ) = 0;
        
      }
      else
      {
        image( i, j ) = 1.0;
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

template < class Image > void  dilate  ( Image& image, unsigned int r, unsigned int c )
{
  // Make a copy to work from
  Image copy ( image );

  const int centerX ( r/2 );
  const int centerY ( c/2 );

  for( unsigned int i = centerX; i < copy.width() - centerX; ++ i )
  {
    for ( unsigned int j = centerY; j < copy.height() - centerY; ++j )
    {
      if ( copy( i, j ) == 0 )
      {
        for( int s = -centerX; s < centerX; ++s )
        {
          for( int t = -centerY; t < centerY; ++t )
          {
            image( i + s, j + t ) = 0;
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convolve the image
//  TODO should have option on whether or not to expand the borders
//
///////////////////////////////////////////////////////////////////////////////

template < class Image > void  convolve  ( Image& image, const Matrix<int>& mask )
{
  // make a copy to work from
  Image copy ( image );

  const unsigned int centerX ( 1 );
  const unsigned int centerY ( 1 );

  // Resize the image
  image.allocate( image.width() - centerX, image.height() - centerY, image.pixelFormat() );

  for( unsigned int i = 1; i < copy.width() - 1; ++ i )
  {
    for ( unsigned int j = 1; j < copy.height() - 1; ++j )
    {
      Image::ValueType value ( 0 );
      value += mask(0,0) * copy( i - 1, j - 1 );
      value += mask(1,0) * copy( i,     j - 1 );
      value += mask(2,0) * copy( i + 1, j - 1 );

      value += mask(0,1) * copy( i - 1, j );
      value += mask(1,1) * copy( i,     j );
      value += mask(2,1) * copy( i + 1, j );

      value += mask(0,2) * copy( i - 1, j + 1 );
      value += mask(1,2) * copy( i,     j + 1 );
      value += mask(2,2) * copy( i + 1, j + 1 );

      image( i - 1, j - 1 ) = value;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth image
//
///////////////////////////////////////////////////////////////////////////////

template < class Image > void  smooth    ( Image& image, const Matrix<int>& mask )
{
}


}




#endif // __IMAGES_MORPHOLOGY_H__

