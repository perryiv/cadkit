
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach and Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ALGORITHMS_COMPOSITE_H__
#define __MINERVA_CORE_ALGORITHMS_COMPOSITE_H__

#include "Minerva/Core/Algorithms/SubRegion.h"

#include "Usul/Functions/Color.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Trace/Trace.h"

#include "osg/Image"

namespace Minerva {
namespace Core {
namespace Algorithms {
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
      dst[3] = 255;
    }
    else
    {
      // Get the current alpha.
      const unsigned char currentAlpha ( hasAlpha ? src[3] : 255 );
      
      // Get correct alpha.
      const unsigned char useThisAlpha ( ( hasExtraAlpha )   ? ( static_cast < unsigned char > ( iter->second ) ) : 
                                       ( ( hasOverallAlpha ) ? ( static_cast < unsigned char > ( alpha * currentAlpha ) ) : ( currentAlpha ) ) );
      
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
    osg::ref_ptr<osg::Image> sub ( Minerva::Core::Algorithms::subRegion<unsigned char> ( image, region, GL_RGBA, GL_UNSIGNED_BYTE ) );
    if ( sub.valid() )
      raster ( result, *sub, alphas, alpha, brightness, job );
  }
}


}
}
}
}

#endif // __MINERVA_CORE_ALGORITHMS_COMPOSITE_H__
