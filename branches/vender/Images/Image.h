
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_IMAGE_H__
#define __IMAGES_IMAGE_H__

#include "Images/ImageBase.h"

#include <vector>


namespace Images {

template < class Policy > 
class Image : public ImageBase
{
public:

  // Useful typedefs
  typedef ImageBase BaseClass;
  typedef typename Policy::PixelType   PixelType;
  typedef typename Policy::PixelFormat PixelFormat;
  typedef typename Policy::Pixel       Pixel;
  typedef std::vector< Pixel > Data;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Image );


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor
  //
  ///////////////////////////////////////////////////////////////////////////////

  Image() :
  _r( 0 ),
  _c( 0 ),
  _pixelFormat ( Image::GRAYSCALE ),
  _data()
  {
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Copy Constructor
  //
  ///////////////////////////////////////////////////////////////////////////////

  Image ( const Image& ):
  _r( image._r ),
  _c( image._c ),
  _pixelFormat ( image._pixelFormat ),
  _data( image._data )
  {
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor
  //
  ///////////////////////////////////////////////////////////////////////////////

  Image( unsigned int r, unsigned int c, PixelFormat pf ) :
  _r( r ),
  _c( c ),
  _pixelFormat ( pf ),
  _data( r * c * bytesPerPixel() )
  {
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor
  //
  ///////////////////////////////////////////////////////////////////////////////

  virtual ~Image()
  {
  }

  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Make enough space
  //
  ///////////////////////////////////////////////////////////////////////////////

  void             allocate ( unsigned int r, unsigned int c, PixelFormat pf )
  {
    _r = r;
    _c = c;
    _pixelFormat = pf;
    _data.resize( r * c * pf );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //   Get the bytes per pixel
  //
  ///////////////////////////////////////////////////////////////////////////////

  unsigned int     bytesPerPixel () const
  {
    return sizeof ( PixelType );
  }

  // Get the Height
  unsigned int     height() const { return _r; }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment
  //
  ///////////////////////////////////////////////////////////////////////////////

  Image&           operator= ( const Image& )
  {
    _r = image._r;
    _c = image._c;
    _pixelFormat = image._pixelFormat;
    _data = image._data;
    return *this;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Get the pixel
  //
  ///////////////////////////////////////////////////////////////////////////////

  Pixel&       operator () ( unsigned int r, unsigned int c )
  {
    return _data.at( r * _r + c );
  }

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Get the pixel
  //
  ///////////////////////////////////////////////////////////////////////////////

  const Pixel& operator () ( unsigned int r, unsigned int c ) const
  {
    return _data.at( r * _r + c );
  }

  // Get the width
  unsigned int     width() const { return _c; }

  virtual ImageBase *           makeGrayscale ( )
  {
    ImageBase *img = new Image< GPolicy < PixelType > >();
  }

  virtual Images::Histogram      calculateHistogram ( )
  {
    Images::Histogram histogram;
    return histogram;
  }

  virtual void           invert ( )
  {
  }

  virtual void           erode     ( unsigned int r, unsigned int c )
  {
  }

  virtual void           dilate    ( unsigned int r, unsigned int c )
  {
  }

  virtual void           convolve  ( const Matrix<int>& mask )
  {
  }

  virtual void           smooth    ( const Matrix<int>& mask )
  {
  }

  virtual void           scale ( double value )
  {
  }

  virtual void           threshold ( double t ) 
  {
  }

private:

  unsigned int _r;
  unsigned int _c;
  Data _data;
};

}

#endif // __IMAGES_IMAGE_H__

