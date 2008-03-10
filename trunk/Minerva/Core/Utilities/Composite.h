
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach and Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_COMPOSITE_H__
#define __STAR_SYSTEM_COMPOSITE_H__

#include "Usul/Functions/Color.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Trace/Trace.h"

#include "osg/Image"

namespace Minerva {
namespace Core {
namespace Utilities {
namespace Composite {

  
///////////////////////////////////////////////////////////////////////////////
//
//  Composite two raster images.
//
///////////////////////////////////////////////////////////////////////////////

template < class Alphas >
inline void raster ( osg::Image& result, const osg::Image& image, const Alphas &alphas, float alpha, float brightness, Usul::Jobs::Job *job )
{
  USUL_TRACE_SCOPE_STATIC;

  // We only handle these cases.
  const GLenum format ( image.getPixelFormat() );
  if ( GL_RGBA != format && GL_RGB != format )
    return;
  
  unsigned char       *dst ( result.data() );
  const unsigned char *src ( image.data()  );
  
  const unsigned int width  ( result.s() );
  const unsigned int height ( result.t() );
  
  // We only composite images of the same size.
  if ( ( static_cast<int> ( width ) != image.s() ) || ( static_cast<int> ( height ) != image.t() ) )
    return;
  
  const unsigned int size ( width * height );
  const bool alphaMapEmpty ( alphas.empty() );
  bool hasExtraAlpha ( false );
  typename Alphas::const_iterator iter ( alphas.end() );
  
  const bool hasOverallAlpha ( alpha < 1.0f );
  
  const bool hasAlpha ( GL_RGBA == format );
  const unsigned int offset ( ( hasAlpha ) ? 4 : 3 );
  
  for ( unsigned int i = 0; i < size; ++i )
  {
    // Have we been cancelled?
    if ( 0 == ( i % 10 ) )
    {
      if ( ( 0x0 != job ) && ( true == job->canceled() ) )
        job->cancel();
    }
    
    // Copy the color channels. Multiply by the overall (normalized) brightness.
    // The order of the variables is important; the float has to come first.
    unsigned char r ( static_cast<unsigned char> ( brightness * src[0] ) );
    unsigned char g ( static_cast<unsigned char> ( brightness * src[1] ) );
    unsigned char b ( static_cast<unsigned char> ( brightness * src[2] ) );
    
    // Is the color in the alpha table?
    if ( false == alphaMapEmpty )
    {
      iter = alphas.find ( Usul::Functions::Color::pack ( r, g, b, 0 ) );
      hasExtraAlpha = ( alphas.end() != iter );
    }
    
    // If the image is competely opaque then set the values.
    if ( ( false == hasAlpha ) && ( false == hasExtraAlpha ) && ( false == hasOverallAlpha ) )
    {
      dst[0] = r;
      dst[1] = g;
      dst[2] = b;
    }
    else
    {
      // Get correct alpha.
      const unsigned char useThisAlpha ( ( hasExtraAlpha )   ? ( static_cast < unsigned char > ( iter->second ) ) : 
                                       ( ( hasOverallAlpha ) ? ( static_cast < unsigned char > ( alpha * 255 ) ) : ( src[3] ) ) );
      
      // Normalize between zero and one.
      const float a ( static_cast < float > ( useThisAlpha ) / 255.5f );
      
      // Composite.
      dst[0] = static_cast < unsigned char > ( dst[0] * ( 1 - a ) + ( r * a ) );
      dst[1] = static_cast < unsigned char > ( dst[1] * ( 1 - a ) + ( g * a ) );
      dst[2] = static_cast < unsigned char > ( dst[2] * ( 1 - a ) + ( b * a ) );
      
      // Since the alpha has been accounted for above, make the pixel completely opaque.
      if ( a > 0.0 )
        dst[3] = 255;
    }
    
    dst += 4;
    src += offset;
  }
}


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Get sub-region of image.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  namespace Detail
  {
    inline osg::Image* subRegion ( const osg::Image& image, const Usul::Math::Vec4d& region )
    {
      osg::ref_ptr < osg::Image > result ( new osg::Image );
      result->allocateImage ( image.s(), image.t(), 1, GL_RGBA, GL_UNSIGNED_BYTE );
      ::memset ( result->data(), 0, result->getImageSizeInBytes() );
      
      const bool hasAlpha ( GL_RGBA == image.getPixelFormat() );
      
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
          
          const int sp ( Usul::Math::maximum ( sMin, Usul::Math::minimum ( width,  static_cast<int> ( static_cast<int> ( up * width  ) + 0.5 ) ) ) );
          const int tp ( Usul::Math::maximum ( tMin, Usul::Math::minimum ( height, static_cast<int> ( static_cast<int> ( vp * height ) + 0.5 ) ) ) );

          unsigned char *dst (  result->data ( s, t ) );
          const unsigned char* src ( image.data ( sp, tp  ) );
          
          dst[0] = src[0];
          dst[1] = src[1];
          dst[2] = src[2];
          dst[3] = ( hasAlpha ? src[3] : 255 );
        }
      }
      
      return result.release();
    }
  }
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Is the region the whole image?
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  namespace Detail
  {
    inline bool regionIsWholeImage ( const Usul::Math::Vec4d& region )
    {
      return ( 0.0 == region[0] && 1.0 == region[1] && 0.0 == region[2] && 1.0 == region[3] );
    }
  }
  
///////////////////////////////////////////////////////////////////////////////
//
//  Composite two raster images.
//
///////////////////////////////////////////////////////////////////////////////

template < class Alphas >
inline void raster ( osg::Image& result, const osg::Image& image, const Usul::Math::Vec4d& region, const Alphas &alphas, float alpha, float brightness, Usul::Jobs::Job *job )
{
  USUL_TRACE_SCOPE_STATIC;
  
  if ( Detail::regionIsWholeImage ( region ) )
    raster ( result, image, alphas, alpha, brightness, job );
  else
  {
    osg::ref_ptr<osg::Image> sub ( Detail::subRegion ( image, region ) );
    if ( sub.valid() )
      raster ( result, *sub, alphas, alpha, brightness, job );
  }
}


}
}
}
}

#endif // __STAR_SYSTEM_COMPOSITE_H__
