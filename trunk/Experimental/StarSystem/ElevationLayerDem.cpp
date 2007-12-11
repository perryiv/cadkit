
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/ElevationLayerDem.h"

#include "Usul/Predicates/Tolerance.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "ossim/imaging/ossimImageData.h"

using namespace StarSystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDem::ElevationLayerDem() : 
  BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDem::~ElevationLayerDem()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* ElevationLayerDem::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * job )
{
  USUL_TRACE_SCOPE;
  return BaseClass::texture ( extents, width, height, level, job );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a blank image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* ElevationLayerDem::_createBlankImage( unsigned int width, unsigned int height ) const
{
  osg::ref_ptr < osg::Image > result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_LUMINANCE, GL_FLOAT );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );

  return result.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class SrcType, class DstType >
  void convert ( const ossimImageData& data, osg::Image& image )
  {
    const unsigned int width ( data.getWidth() );
    const unsigned int height ( data.getHeight() );
    const unsigned int size ( width * height );

    DstType *buffer ( reinterpret_cast < DstType* > ( image.data() ) );

    const SrcType* b1 ( static_cast < const SrcType* > ( data.getBuf( 0 ) ) );
    SrcType np1 ( static_cast < SrcType > ( data.getNullPix( 0 ) ) );

    Usul::Predicates::Tolerance<SrcType,SrcType> tolerance ( 5 );

    // Copy the pixels into the osg image.
    for ( unsigned int i = 0; i < size; ++i )
    {
      DstType value ( tolerance ( *b1, np1 ) ? 0.0 : static_cast < DstType > ( *b1 ) );
      *buffer = value;

      ++buffer;
      ++b1;
    }

    image.flipVertical();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerDem::_convert ( const ossimImageData& data, osg::Image& image )
{
  USUL_TRACE_SCOPE;

  switch ( data.getScalarType () )
  {
  case OSSIM_SINT16:
    Detail::convert < ossim_sint16, float > ( data, image );
    break;
  case OSSIM_UINT16:
    Detail::convert < ossim_uint16, float > ( data, image );
    break;
  case OSSIM_FLOAT32:
    Detail::convert < ossim_float32, float > ( data, image );
    break;
  default:
    break;
  }
}
