
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "RasterPolygonLayer.h"

#include "Usul/Adaptors/Random.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Format.h"

#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_alg.h"
//#include "cpl_conv.h"
#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogrsf_frmts.h"
//#include "ogr_srs_api.h"
//#include "cpl_string.h"
#include "cpl_error.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize Gdal.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct Init
  {
    Init()
    {
      GDALAllRegister();
      OGRRegisterAll();
    }
    ~Init()
    {
      GDALDestroyDriverManager();
      OGRCleanupAll();
    }
  } _init;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Error handler.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void errorHandler ( CPLErr, int, const char* text )
  {
    std::cout << text << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::RasterPolygonLayer ( Layer* layer ) : 
  BaseClass(),
  _layer ( dynamic_cast<PolygonLayer*> ( layer ) ),
  _temp (),
  _data ( 0x0 )
{
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::RasterPolygonLayer ( const RasterPolygonLayer& rhs ) :
  BaseClass ( rhs ),
  _layer ( rhs._layer ),
  _temp(),
  _data ( 0x0 )
{
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer& RasterPolygonLayer::operator= ( const RasterPolygonLayer& rhs )
{
  BaseClass::operator= ( rhs );
  _layer = rhs._layer;
  
  this->_init();
  
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::~RasterPolygonLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterPolygonLayer::_init()
{
  // Set an error handler.
  CPLSetErrorHandler ( Detail::errorHandler );
  
  // Return now if we don't have a valid layer to work with.
  if ( false == _layer.valid() )
    return;
  
  // Set the name.
  this->name ( _layer->name() );
  
  // Make a geotiff.
  std::string format ( "GTiff" );
  
  // Find a drive for geotiff.
  GDALDriver *driver ( GetGDALDriverManager()->GetDriverByName( format.c_str() ) );
  
  // Return now if we didn't find a driver.
  if ( 0x0 == driver )
    return;
  
  char **options  ( 0x0 );
  
  const unsigned int width ( 2048 );
  const unsigned int height ( 2048 );
  const unsigned int channels ( 4 );
  
  // Get the extents of the layer.
  Usul::Math::Vec2d ll, ur;
  _layer->extents ( ll, ur );
  
  // Filename.
  std::string name ( Usul::Strings::format ( Usul::File::base ( _temp.name() ), ".tif" ).c_str() );
  
  // Create the file.
  _data = driver->Create( name.c_str(), width, height, channels, GDT_Byte, options );
  
  if ( 0x0 == _data )
    return;
  
  std::vector<char> bytes ( width * height * channels, 0 );
  _data->RasterIO( GF_Write, 0, 0, width, height, &bytes[0], width, height, GDT_Byte, 3, 0x0, 0, 0, 0 );
  
  const double xLength ( ur[0] - ll[0] );
  const double yLength ( ur[1] - ll[1] );
  
  const double xResolution  ( xLength / width );
  const double yResolution  ( yLength / height );
  
  std::vector<double> geoTransform ( 6 );

  geoTransform[0] = ll[0];          // top left x
  geoTransform[1] = xResolution;    // w-e pixel resolution
  geoTransform[2] = 0;              // rotation, 0 if image is "north up"
  geoTransform[3] = ur[1];          // top left y
  geoTransform[4] = 0;              // rotation, 0 if image is "north up"
  geoTransform[5] = -yResolution;   // n-s pixel resolution

  _data->SetGeoTransform( &geoTransform[0] );
  
  _data->SetProjection( _layer->projectionWKT().c_str() );
  
  // Typedefs.
  typedef std::vector<OGRGeometry*> Geometries;
  typedef std::vector<int> Bands;
  typedef std::vector<double> BurnValues;
  
  // Bands to burn.
  Bands bands;
  
  // Fill in the values.
  for ( unsigned int i = 0; i < channels; ++i )
  {
    // GDAL band index starts at 1.
    bands.push_back ( i + 1 );
  }
  
  // Geometries to burn.
  Geometries geometries;
  
  // Values to burn.
  BurnValues burnValues;
  
  // Fill in the geometries.
  {
    // Get the connection.
    Minerva::Core::DB::Connection::RefPtr connection ( _layer->connection() );
    
    // Return if no connection.
    if ( false == connection.valid() )
      return;
    
    const std::string driverName ( "PostgreSQL" );
    OGRSFDriver *driver ( OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( driverName.c_str() ) );
    
    // Return if we didn't find a driver.
    if ( 0x0 == driver )
      return;
    
    // Connection parameters.
    std::string hostname ( Usul::Strings::format (     "host=", connection->hostname() ) );
    std::string database ( Usul::Strings::format (   "dbname=", connection->database() ) );
    std::string username ( Usul::Strings::format (     "user=", connection->username() ) );
    std::string password ( Usul::Strings::format ( "password=", connection->password() ) );
    
    std::string name ( Usul::Strings::format ( "PG:", hostname, " ", username, " ", password, " ", database ) );
    
    // Get the data source.
    OGRDataSource *data ( driver->CreateDataSource( name.c_str(), 0x0 ) );
    
    // Return if no data.
    if ( 0x0 == data )
    {
      //std::cout << ::CPLGetLastErrorMsg() << std::endl;
      return;
    }
    
    // Make a layer.
    OGRLayer *layer ( data->ExecuteSQL( Usul::Strings::format ( "SELECT * FROM ", _layer->tablename() ).c_str() /*_layer->query().c_str()*/, 0x0, 0x0 ) );
    
    if ( 0x0 == layer )
      return;
    
    OGRFeature* feature ( 0x0 );
    
    // Get the color functor.
    typedef Minerva::Core::Functors::BaseColorFunctor ColorFunctor;
    ColorFunctor::RefPtr functor ( _layer->colorFunctor() );
    
    // Get the column for the color.
    const std::string column ( _layer->colorColumn() );
    
    //Usul::Adaptors::Random<double> random ( 0, 255 );
    
    while ( 0x0 != ( feature = layer->GetNextFeature() ) )
    {
      // Add the geometry.
      OGRGeometry *geometry ( feature->GetGeometryRef()->clone() );
      geometries.push_back ( geometry );
      
      // Get the index of the column
      const int index ( feature->GetFieldIndex ( column.c_str() ) );
      
      const double value ( index > 0 ? feature->GetFieldAsDouble ( index ) : 0.0 );
      
      // Burn color.
      const osg::Vec4 color ( functor.valid() ? (*functor) ( value ) : osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );
      
      // Add the burn values.
      burnValues.push_back ( color[0] * 255 );
      burnValues.push_back ( color[1] * 255 );
      burnValues.push_back ( color[2] * 255 );
      burnValues.push_back ( 200 );
      
      OGRFeature::DestroyFeature( feature );
    }
  }
  
  // Burn the raster.
  ::GDALRasterizeGeometries( _data, bands.size(), &bands[0], 
                             geometries.size(), (void**) &geometries[0], 
                             0x0, 0x0, &burnValues[0], 0x0,
                             GDALTermProgress, 0x0 );
  
  // Clean up geometries.
  for ( Geometries::iterator iter = geometries.begin(); iter != geometries.end(); ++iter )
    delete *iter;
  
  OGRSpatialReference src ( _data->GetProjectionRef() ), dst;
  dst.SetWellKnownGeogCS( "EPSG:4326" );
  
  OGRCoordinateTransformation *transform ( OGRCreateCoordinateTransformation( &src, &dst ) );
#if 0
  if ( 0x0 != transform )
  {
    transform->Transform ( 2, &ll[0], &ll[1] );
    transform->Transform ( 2, &ur[0], &ur[1] );
    Extents extents ( Extents::Vertex ( ll[0], ll[1] ), Extents::Vertex ( ur[0], ur[1] ) );
    this->extents ( extents );
  }
#endif
  
  GDALClose( _data );
  
  this->open ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterPolygonLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterPolygonLayer( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::ImagePtr RasterPolygonLayer::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * job, IUnknown *caller )
{
#if 0
  Guard guard ( this );
  

  if ( 0x0 == _data )
    return 0x0;
  
  std::vector<double> geoTransform ( 6 );
  _data->GetGeoTransform( &geoTransform[0] );
  
  OGRSpatialReference src ( _data->GetProjectionRef() ), dst;
  dst.SetWellKnownGeogCS( "EPSG:4326" );

  return 0x0;
#else
  return BaseClass::texture ( extents, width, height, level, job, caller );
#endif
}
