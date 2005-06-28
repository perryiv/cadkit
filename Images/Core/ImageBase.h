
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_IMAGE_BASE_H__
#define __IMAGES_IMAGE_BASE_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace Images {


  template < class T > Matrix;

  typedef std::vector< unsigned int > Histogram;
  

struct ImageBase : public Usul::Base::Referenced
{
  virtual unsigned short bytesPerPixel() = 0

  virtual void           rgbToGrayscale ( ) = 0;

  virtual Histogram      calculateHistogram ( ) = 0;

  virtual void           invert ( ) = 0;

  virtual void           erode     ( unsigned int r, unsigned int c ) = 0;
  virtual void           dilate    ( unsigned int r, unsigned int c ) = 0;
  virtual void           convolve  ( const Matrix<int>& mask ) = 0;
  virtual void           smooth    ( const Matrix<int>& mask ) = 0;

  virtual void           scale ( double value ) = 0;

  virtual void           threshold ( double t ) = 0;
};

}

#endif // __IMAGES_IMAGE_BASE_H__