
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/ElevationGroup.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

using namespace StarSystem;

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
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::~ElevationGroup()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a blank image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image * ElevationGroup::_createBlankImage ( unsigned int width, unsigned int height ) const
{
  osg::ref_ptr < osg::Image > result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_LUMINANCE, GL_FLOAT );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );

  return result.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Composite the two images.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationGroup::_compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, Usul::Jobs::Job * )
{
  USUL_TRACE_SCOPE;

  const unsigned int width ( image.s() );
  const unsigned int height ( image.t() );
  const unsigned int size ( width * height );

  float *      dst ( reinterpret_cast < float* > ( result.data() ) );
  const float *src ( reinterpret_cast < const float* > ( image.data() ) );
  
  // Copy the pixels into the osg image.
  for ( unsigned int i = 0; i < size; ++i )
  {
    *dst = *src;

    ++dst;
    ++src;
  }
}
