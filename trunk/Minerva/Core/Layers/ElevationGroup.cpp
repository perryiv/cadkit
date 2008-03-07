
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/ElevationGroup.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( ElevationGroup );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::ElevationGroup() : 
  BaseClass ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::ElevationGroup ( const ElevationGroup& rhs ) : BaseClass ( rhs )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::~ElevationGroup()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ElevationGroup::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new ElevationGroup ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a blank image.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::ImagePtr ElevationGroup::_createBlankImage ( unsigned int width, unsigned int height ) const
{
  ImagePtr result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_LUMINANCE, GL_FLOAT );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class SrcType, class DstType >
  void convert ( const osg::Image& data, osg::Image& image )
  {
    const unsigned int width ( data.s() );
    const unsigned int height ( data.t() );
    const unsigned int size ( width * height );
    
    DstType       *dst ( reinterpret_cast < DstType* >       ( image.data() ) );
    const SrcType *src ( reinterpret_cast < const SrcType* > ( data.data()  ) );
    
    // Copy the pixels into the osg image.
    for ( unsigned int i = 0; i < size; ++i )
    {
      SrcType value ( *src );
      *dst = static_cast<DstType> ( value );
      
      ++dst;
      ++src;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void composite ( osg::Image& result, const osg::Image& image )
  {
    const unsigned int width ( image.s() );
    const unsigned int height ( image.t() );
    const unsigned int size ( width * height );
    
    float *      dst ( reinterpret_cast < float* > ( result.data() ) );
    const float *src ( reinterpret_cast < const float* > ( image.data() ) );
    
    const GLenum pixelFormat ( image.getPixelFormat() );
    
    const bool hasAlpha ( GL_RGBA == pixelFormat || GL_LUMINANCE_ALPHA == pixelFormat );
    
    const unsigned int offset ( hasAlpha ? 2 : 1 );
    
    // Copy the pixels into the osg image.
    for ( unsigned int i = 0; i < size; ++i )
    {
      float alpha ( hasAlpha ? src[1] / std::numeric_limits<float>::max() : 1.0 );
        
      *dst = ( *dst * ( 1.0f - alpha ) ) + ( *src * alpha );
      
      ++dst;
      
      src += offset;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Composite the two images.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationGroup::_compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, float alpha, Usul::Jobs::Job * )
{
  USUL_TRACE_SCOPE;

  if ( GL_FLOAT == image.getDataType() )
    Detail::composite ( result, image );
  else
  {
    ImagePtr convert ( new osg::Image );
    convert->allocateImage ( image.s(), image.t(), 1, GL_LUMINANCE, GL_FLOAT );
    ::memset ( convert->data(), 0, convert->getImageSizeInBytes() );
    
    switch ( image.getDataType() )
    {
    case GL_SHORT:
      Detail::convert<short, float> ( image, *convert );
      break;
    case GL_UNSIGNED_SHORT:
      // Treat as shorts.  Any number greater than max short will be treated as a negative number.
      // This is a work around for one earth's wms server.
      Detail::convert<short, float> ( image, *convert );
      break;
    case GL_UNSIGNED_BYTE:
      Detail::convert<unsigned char, float> ( image, *convert );
      break;
    }
    
    Detail::composite ( result, *convert );
  }
}
