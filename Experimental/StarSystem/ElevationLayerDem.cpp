
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/ElevationLayerDem.h"

#include "Usul/File/Path.h"
#include "Usul/Math/Interpolate.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "ossim/imaging/ossimImageData.h"
#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordList.h"
#include "ossim/projection/ossimProjection.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimUtmProjection.h"
#include "ossim/support_data/ossimDemGrid.h"

#include <fstream>

using namespace StarSystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDem::ElevationLayerDem() : 
  BaseClass(),
  _loaded ( false ),
  _grid( 0x0 ),
  _projection ( 0x0 )
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

  if ( 0x0 != _grid )
    delete _grid;
  _grid = 0x0;
  
  if ( 0x0 != _projection )
    delete _projection;
  _projection = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the grid.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerDem::open( const std::string& filename )
{
  {
    std::ifstream in ( filename.c_str() );
    
    if ( false == in.is_open() )
      return;
    
    Guard guard ( this );
    _grid = new ossimDemGrid;
    _grid->read(in, false);
  }
  
#if 0
  // See if there is a .geom file.
  const std::string directory ( Usul::File::directory ( filename, false ) );
  const std::string basename ( Usul::File::base ( filename ) );
  
  ossimKeywordlist kwl;
  kwl.add( ossimKeywordNames::GEOM_FILE_KW, /*filename.c_str()*/ std::string ( directory + "/" + basename + ".geom" ).c_str() );
  
  Guard guard ( this );
  _projection = ossimProjectionFactoryRegistry::instance()->createProjection ( kwl );
#else
  _projection = new ossimUtmProjection ( 12 );
#endif
  if ( 0x0 == _projection )
    return;
  
  ossimDemHeader header ( _grid->getHeader() );
  ossimDemPointVector corners ( header.getDEMCorners() );
  
  std::cout << corners[0].getX() << " " << corners[0].getY() << std::endl;
  std::cout << corners[1].getX() << " " << corners[1].getY() << std::endl;
  std::cout << corners[2].getX() << " " << corners[2].getY() << std::endl;
  std::cout << corners[3].getX() << " " << corners[3].getY() << std::endl;

  ossimGpt ll ( _projection->inverse ( ossimDpt ( corners[0].getX(), corners[0].getY() ) ) );
  ossimGpt ur ( _projection->inverse ( ossimDpt ( corners[2].getX(), corners[2].getY() ) ) );
  
  Extents extents ( ll.lon, ll.lat, ur.lon, ur.lat );
  this->extents ( extents );
  
  _loaded = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* ElevationLayerDem::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * job )
{
  USUL_TRACE_SCOPE;
  
  // Make the result.
  osg::ref_ptr < osg::Image > result ( this->_createBlankImage( width, height ) );
  
  if ( 0x0 != _projection && true == _loaded )
  {
    // The upper left corner.
    ossimGpt ul ( extents.maximum()[1], extents.minimum()[0] );
    ossimDpt point ( _projection->forward( ul ) );
    
    const double deltaX ( extents.maximum()[0] - extents.minimum()[0] );
    const double deltaY ( extents.maximum()[1] - extents.minimum()[1] );
    
    for ( unsigned int i = 0; i < height; ++i )
    {
      const double v ( static_cast<double> ( i ) / ( height - 1 ) );
      for ( unsigned int j = 0; j < width; ++j )
      {
        const double u ( static_cast<double> ( j ) / ( width - 1 ) );
        
        // Calculate current lat, lon.
        const double longitude ( extents.minimum()[0] + ( u * deltaX ) );
        const double latitude ( extents.maximum()[1] - ( v * deltaY ) );
        
        // Get the elevation data.
        float elevation ( static_cast < float > ( this->value ( longitude, latitude ) ) );
        
        if ( _grid->getMissingDataValue() == elevation )
          elevation = 0.0;
        
        *reinterpret_cast < float* > ( result->data ( i, j ) ) = elevation;
      }
    }
  }
  
  // Return our result.
  return result.release();
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


// Get the value at the lat, lon location.  May return null pixel value.
double ElevationLayerDem::value ( double lon, double lat ) const
{
  USUL_TRACE_SCOPE;
  
  ossimGpt ul ( lat, lon );
  ossimDpt point ( _projection->forward( ul ) );
  
  ossimDemHeader header ( _grid->getHeader() );
  ossimDemPointVector corners ( header.getDEMCorners() );
  
  const double nullValue ( _grid->getMissingDataValue() );
  
  const double deltaX ( corners[2].getX() - corners[1].getX() );
  const double deltaY ( corners[0].getY() - corners[1].getY() );
  
  const double u ( ( point.x - corners[1].getX() ) / deltaX );
  const double v ( ( point.y - corners[1].getY() ) / deltaY );
  if ( u < 0.0 || v < 0.0 || u > 1.0 || v > 1.0 )
    return nullValue;

  const long width ( _grid->getWidth() );
  const long height ( _grid->getHeight() );
  
  const long i ( static_cast <long> ( ::floor ( u * ( width - 1 ) ) ) );
  const long j ( static_cast <long> ( ::floor ( v * ( height - 1 ) ) ) );
  const double a ( _grid->getElevation( i,     j     ) );
  const double b ( _grid->getElevation( i + 1, j     ) );
  const double c ( _grid->getElevation( i,     j + 1 ) );
  const double d ( _grid->getElevation( i + 1, j + 1 ) );
  
  if ( nullValue == a || nullValue == b || nullValue == c || nullValue == d )
    return nullValue;
  
  const double up ( u - ( static_cast < double > ( i ) / width ) );
  const double vp ( v - ( static_cast < double > ( j ) / height ) );
  
  return Usul::Math::Interpolate<double>::bilinear ( up, vp, a, b, c, d );
}
