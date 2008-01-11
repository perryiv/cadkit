
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/ElevationLayerDem.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Math/Interpolate.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "ossim/imaging/ossimImageData.h"
#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimProjection.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimUtmProjection.h"
#include "ossim/support_data/ossimDemGrid.h"

#include <fstream>

using namespace StarSystem;

USUL_FACTORY_REGISTER_CREATOR ( ElevationLayerDem );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDem::ElevationLayerDem() : 
  BaseClass(),
  _filename (),
  _loaded ( false ),
  _grid( 0x0 ),
  _projection ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDem::ElevationLayerDem ( const ElevationLayerDem& rhs ) :
  BaseClass ( rhs ),
  _filename (),
  _loaded ( false ),
  _grid( 0x0 ),
  _projection ( 0x0 )
{
  this->open( rhs._filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDem& ElevationLayerDem::operator= ( const ElevationLayerDem& rhs )
{
  BaseClass::operator= ( rhs );
  
  this->open ( rhs._filename );
  
  return *this;
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
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ElevationLayerDem::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new ElevationLayerDem ( *this ) );
  return clone.release();
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
    _filename = filename;
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
  ossimUtmProjection *projection = new ossimUtmProjection ( *(ossimDatumFactory::instance()->
                                                              create(ossimString("NAS-C"))->
                                                              ellipsoid()) );
  projection->setDatum(ossimDatumFactory::instance()->
                 create(ossimString("NAS-C")));
  projection->setZone(12);
  projection->setHemisphere('N');
  _projection = projection;
#endif
  if ( 0x0 == _projection )
    return;
  
  ossimDemHeader header ( _grid->getHeader() );
  ossimDemPointVector corners ( header.getDEMCorners() );

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
  
  Guard guard ( this );

  if ( 0x0 != _projection && true == _loaded )
  {
    // The upper left corner.
    ossimGpt ul ( extents.maximum()[1], extents.minimum()[0] );
    ossimDpt point ( _projection->forward( ul ) );

#if 0
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
#else
        // Shortcuts.
        const Extents::Vertex &mn ( extents.minimum() );
        const Extents::Vertex &mx ( extents.maximum() );

        for ( int i = height - 1; i >= 0; --i )
        {
          const double u ( 1.0 - static_cast<double> ( i ) / ( height - 1 ) );
          for ( unsigned int j = 0; j < width; ++j )
          {
            const double v ( static_cast<double> ( j ) / ( width - 1 ) );
            const double longitude ( mn[0] + u * ( mx[0] - mn[0] ) );
            const double latitude ( mn[1] + v * ( mx[1] - mn[1] ) );
#endif
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


// Get the value at the lat, lon location.  May return null pixel value.
double ElevationLayerDem::value ( double lon, double lat ) const
{
  USUL_TRACE_SCOPE;
  
  ossimGpt ul ( lat, lon );
  ossimDpt point ( _projection->forward( ul ) );
  
  ossimDemHeader header ( _grid->getHeader() );
  
  const double nullValue ( _grid->getMissingDataValue() );
  
  double northWestX ( 0 ), northWestY ( 0 );
  _grid->getGroundCoords(0, 0, northWestX, northWestY);
  
  // Y is the row number, x is the column number.
  const double x ( ( point.x - northWestX ) / header.getSpatialResX() );
  const double y ( ( northWestY - point.y ) / header.getSpatialResY() );
  
  const long i ( static_cast <long> ( x ) );
  const long j ( static_cast <long> ( y ) );
  
  double a ( _grid->getElevation( i,     j     ) );
  double b ( _grid->getElevation( i + 1, j     ) );
  double c ( _grid->getElevation( i,     j + 1 ) );
  double d ( _grid->getElevation( i + 1, j + 1 ) );

  Usul::Predicates::Tolerance<double,double> tolerance ( 5 );
  
  if ( tolerance ( nullValue, a ) )
    a = header.getMinimumElev();
  if ( tolerance ( nullValue, b ) )
    b = header.getMinimumElev();
  if ( tolerance ( nullValue, c ) )
    c = header.getMinimumElev();
  if ( tolerance ( nullValue, d ) )
    d = header.getMinimumElev();

  const double u ( x - static_cast < double > ( i ) );
  const double v ( y - static_cast < double > ( j ) );
  
  return Usul::Math::Interpolate<double>::bilinear ( u, v, a, b, c, d );
}
