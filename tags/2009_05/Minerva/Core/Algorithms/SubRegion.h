
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ALGORITHMS_SUB_REGION_H__
#define __MINERVA_CORE_ALGORITHMS_SUB_REGION_H__

#include "osg/Image"

#include <cstring>

namespace Minerva {
namespace Core {
namespace Algorithms {

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Get sub-region of image.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template<class PixelType>
  inline osg::Image* subRegion ( const osg::Image& image, const Usul::Math::Vec4d& region, GLenum pixelFormat, GLenum dataType )
  {
    osg::ref_ptr < osg::Image > result ( new osg::Image );
    result->allocateImage ( image.s(), image.t(), 1, pixelFormat, dataType );
    ::memset ( result->data(), 0, result->getImageSizeInBytes() );
    
    // Is the image one channel?
    const bool greyscale ( GL_LUMINANCE == pixelFormat );

    // Check for alphas.
    const bool dstHasAlpha ( GL_RGBA == pixelFormat || GL_LUMINANCE_ALPHA == pixelFormat );
    const bool hasAlpha ( GL_RGBA == image.getPixelFormat() || GL_LUMINANCE_ALPHA == image.getPixelFormat() );
    
    // Width and height.
    const int width ( image.s() );
    const int height ( image.t() );
    
    const double du ( region[1] - region[0] );
    const double dv ( region[3] - region[2] );
    
    const int sMin ( static_cast<int> ( region[0] * width  ) );
    const int tMin ( static_cast<int> ( region[2] * height ) );
    
    for ( int s = 0; s < width; ++s )
    {
      const double u ( static_cast<double> ( s ) / ( width ) );
      
      for ( int t = 0; t < height; ++t )
      {
        const double v ( static_cast<double> ( t ) / ( height ) );
        
        const double up ( ( region[0] + ( u * du ) ) );
        const double vp ( ( region[2] + ( v * dv ) ) );
        
        const int sp ( Usul::Math::maximum ( sMin, Usul::Math::minimum ( width - 1,  static_cast<int> ( static_cast<int> ( up * width  ) + 0.5 ) ) ) );
        const int tp ( Usul::Math::maximum ( tMin, Usul::Math::minimum ( height - 1, static_cast<int> ( static_cast<int> ( vp * height ) + 0.5 ) ) ) );

        PixelType *dst ( reinterpret_cast<PixelType*> ( result->data ( s, t ) ) );
        const PixelType* src ( reinterpret_cast<const PixelType*> ( image.data ( sp, tp ) ) );
        
        *dst++ = *src++;

        if ( false == greyscale )
        {
          *dst++ = *src++;
          *dst++ = *src++;
        }

        if ( dstHasAlpha )
          *dst++ = ( hasAlpha ? *src : 255 );
      }
    }
    
    return result.release();
  }

}
}
}

#endif // __MINERVA_CORE_ALGORITHMS_SUB_REGION_H__
