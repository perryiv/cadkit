
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
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ElevationLayerDem, ElevationLayerDem::BaseClass );

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
  
  this->_addMember ( "filename", _filename );
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

ElevationLayerDem::ImagePtr ElevationLayerDem::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // Make the result.
  ImagePtr result ( this->_createBlankImage ( width, height ) );
  
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
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a blank image.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDem::ImagePtr ElevationLayerDem::_createBlankImage ( unsigned int width, unsigned int height ) const
{
  ImagePtr result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_LUMINANCE, GL_FLOAT );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value at the lat, lon location.  May return null pixel value.
//
///////////////////////////////////////////////////////////////////////////////
    
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
  
  const long i ( static_cast <long> ( ::floor ( x ) ) );
  const long j ( static_cast <long> ( ::floor ( y ) ) );
  
  const long ip ( static_cast <long> ( x + 1 ) );
  const long jp ( static_cast <long> ( y + 1 ) );
  
  double a ( _grid->getElevation( i,  j  ) );
  double b ( _grid->getElevation( ip, j  ) );
  double c ( _grid->getElevation( i,  jp ) );
  double d ( _grid->getElevation( ip, jp ) );

  Usul::Predicates::Tolerance<double,double> tolerance ( 5 );
  
  if ( tolerance ( nullValue, a ) )
    a = 0.0;
  if ( tolerance ( nullValue, b ) )
    b = 0.0;
  if ( tolerance ( nullValue, c ) )
    c = 0.0;
  if ( tolerance ( nullValue, d ) )
    d = 0.0;

  const double u ( x - static_cast < double > ( i ) );
  const double v ( y - static_cast < double > ( j ) );
  
  const double t0 ( Usul::Math::Interpolate<double>::linear ( u, a, b ) );
  const double t1 ( Usul::Math::Interpolate<double>::linear ( u, c, d ) );
  return Usul::Math::Interpolate<double>::linear ( v, t0, t1 );
  //return Usul::Math::Interpolate<double>::bilinear ( u, v, a, b, c, d );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////
    
void ElevationLayerDem::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _dataMemberMap.deserialize ( node );
      
  // Open ourselfs.
  this->open ( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get elevation at lat long.
//
///////////////////////////////////////////////////////////////////////////////
   
double ElevationLayerDem::elevationAtLatLong ( double lat, double lon ) const
{
  return this->value ( lon, lat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface id..
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ElevationLayerDem::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IElevationDatabase::IID:
    return static_cast < Usul::Interfaces::IElevationDatabase* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
