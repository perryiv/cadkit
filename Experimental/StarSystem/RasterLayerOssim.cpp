
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/RasterLayerOssim.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "ossim/imaging/ossimImageRenderer.h"
#include "ossim/imaging/ossimImageHandler.h"
#include "ossim/imaging/ossimImageHandlerRegistry.h"

#include "ossim/projection/ossimEquDistCylProjection.h"
#include "ossim/projection/ossimImageViewTransform.h"
#include "ossim/projection/ossimImageViewProjectionTransform.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"

using namespace StarSystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerOssim::RasterLayerOssim() : 
  BaseClass(),
  _handler ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerOssim::~RasterLayerOssim()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &RasterLayerOssim::_destroy ), "3809846167" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_destroy()
{
  USUL_TRACE_SCOPE;

  Usul::Pointers::unreference ( _handler ); _handler = 0x0;
  Usul::Pointers::unreference ( _projection ); _projection = 0x0;
  Usul::Pointers::unreference ( _renderer ); _renderer = 0x0;
  _viewInterface = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the image.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::open ( const std::string& filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _handler = ossimImageHandlerRegistry::instance()->open ( ossimFilename ( filename.c_str() ) );
  Usul::Pointers::reference ( _handler );

  _projection = new ossimEquDistCylProjection;
  Usul::Pointers::reference ( _projection );

  _renderer = new ossimImageRenderer;
  Usul::Pointers::reference ( _renderer );

  _viewInterface = _renderer;

  _renderer->setView ( _projection, false );
  _renderer->getResampler()->setFilterType ( "nearest neighbor" );

  _renderer->connectMyInputTo ( 0, _handler );

  ossimImageViewTransform* ivt = _renderer->getImageViewTransform();
  if ( ivt )
  {
    ossimImageViewProjectionTransform* projectionTransform = PTR_CAST ( ossimImageViewProjectionTransform, ivt );
    if ( projectionTransform )
    {
       ossimMapProjection* mapProj = PTR_CAST ( ossimMapProjection, projectionTransform->getImageProjection() );
       if ( mapProj )
       {
          mapProj->setElevationLookupFlag ( false );
          _projection->setElevationLookupFlag ( false );
       }
    }
  }

  this->_updateExtents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* RasterLayerOssim::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Create the answer.
  osg::ref_ptr < osg::Image > result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );

  if ( 0x0 != _projection && 0x0 != _viewInterface && 0x0 != _renderer )
  {
    ossimIrect requestRect(0, 0, width - 1, height - 1 );

    const double deltaX ( extents.maximum()[0] - extents.minimum()[0] );
    const double deltaY ( extents.maximum()[1] - extents.minimum()[1] );
     
    const double deltaLat ( deltaY / height );
    const double deltaLon ( deltaX / width );

    _projection->setDecimalDegreesPerPixel ( ossimDpt ( deltaLon, deltaLat ) );

    _projection->setUlGpt ( ossimGpt ( extents.maximum()[1], extents.minimum()[0] ) ); // Max lat and min lon.
    _viewInterface->setView ( _projection, false );
    ossimRefPtr<ossimImageData> data ( _renderer->getTile ( requestRect ) );

    // Convert to osg image.
    if ( data.valid() && data->getBuf() && ( data->getDataObjectStatus() != OSSIM_EMPTY ) )
      RasterLayerOssim::_convert ( *data, *result );
  }

  return result.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_convert ( const ossimImageData& data, osg::Image& image )
{
  USUL_TRACE_SCOPE_STATIC;

  // Only handle 8 byte pixels for now.
  if ( OSSIM_UINT8 != data.getScalarType() && data.getSize() > 0 )
    return;

  const unsigned int width ( data.getWidth() );
  const unsigned int height ( data.getHeight() );
  const unsigned int size ( width * height );

  unsigned char *buffer ( image.data() );

  bool greyscale ( data.getNumberOfBands() < 3 );
  
  const unsigned char* b1 ( static_cast < const unsigned char* > ( data.getBuf( 0 ) ) );
  const unsigned char* b2 ( static_cast < const unsigned char* > ( data.getBuf( ( greyscale ? 0 : 1 ) ) ) );
  const unsigned char* b3 ( static_cast < const unsigned char* > ( data.getBuf( ( greyscale ? 0 : 2 ) ) ) ); 
  unsigned char np1 ( static_cast < unsigned char > ( data.getNullPix( 0 ) ) );
  unsigned char np2 ( static_cast < unsigned char > ( data.getNullPix( ( greyscale ? 0 : 1 ) ) ) );
  unsigned char np3 ( static_cast < unsigned char > ( data.getNullPix( ( greyscale ? 0 : 2 ) ) ) );

  // Copy the pixels into the osg image.
  for ( unsigned int i = 0; i < size; ++i )
  {
    // If the pixel is null, make transparent.
    if ( ( *b1 == np1 ) && ( *b2 == np2 ) && ( *b3 == np3 ) )
    {
       buffer[3] = 0;
    }
    else
    {
       buffer[3] = 255;
    }
    buffer[0] = *b1;
    buffer[1] = *b2;
    buffer[2] = *b3;
    buffer +=4;
    ++b1;
    ++b2;
    ++b3;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the extents.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_updateExtents()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _handler )
  {
    ossimKeywordlist kwl;
    _handler->getImageGeometry( kwl );
    
    ossimRefPtr<ossimProjection> projection ( ossimProjectionFactoryRegistry::instance()->createProjection( kwl ) );

    ossimIrect rect ( _handler->getBoundingRect() );

    if ( projection.valid () )
    {
      ossimGpt ulGpt;
      ossimGpt urGpt;
      ossimGpt lrGpt;
      ossimGpt llGpt;

      projection->lineSampleToWorld( rect.ul(), ulGpt );
      projection->lineSampleToWorld( rect.ur(), urGpt );
      projection->lineSampleToWorld( rect.lr(), lrGpt );
      projection->lineSampleToWorld( rect.ll(), llGpt );

      Extents extents ( Extents::Vertex ( llGpt.lon, llGpt.lat ), Extents::Vertex ( urGpt.lon, urGpt.lat ) );
      this->extents ( extents );
    }
  }
}
