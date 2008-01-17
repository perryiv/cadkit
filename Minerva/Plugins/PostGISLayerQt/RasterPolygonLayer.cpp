
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
#include "Usul/App/Application.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_alg.h"
#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogrsf_frmts.h"
#include "cpl_error.h"

USUL_FACTORY_REGISTER_CREATOR ( RasterPolygonLayer );

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
//  Push/Pop error handler.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct PushPopErrorHandler
  {
    PushPopErrorHandler()
    {
      CPLPushErrorHandler( Detail::errorHandler );
    }
    ~PushPopErrorHandler()
    {
      CPLPopErrorHandler();
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::RasterPolygonLayer () : 
  BaseClass(),
  _layer ( 0x0 ),
  _dir ( Usul::File::Temp::directory ( false ) ),
  _srid ( -1 ),
  _projectionText(),
  _latLonProjectionText(),
  _initialized ( false ),
  _geometries(),
  _burnValues()
{
  this->_addMember ( "Layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::RasterPolygonLayer ( Layer* layer ) : 
  BaseClass(),
  _layer ( dynamic_cast<PolygonLayer*> ( layer ) ),
  _dir ( Usul::File::Temp::directory ( false ) ),
  _srid ( -1 ),
  _projectionText(),
  _latLonProjectionText(),
  _initialized ( false ),
  _geometries(),
  _burnValues()
{
  this->_addMember ( "Layer", _layer );
  
  if ( _layer.valid() )
    this->name ( _layer->name() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::RasterPolygonLayer ( const RasterPolygonLayer& rhs ) :
  BaseClass ( rhs ),
  _layer ( rhs._layer ),
  _dir ( rhs._dir ),
  _srid ( rhs._srid ),
  _projectionText( rhs._projectionText ),
  _latLonProjectionText ( rhs._latLonProjectionText ),
  _initialized ( false ),
  _geometries(),
  _burnValues()
{
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
  _dir = rhs._dir;
  _latLonProjectionText = rhs._latLonProjectionText;
  _initialized = false;
  
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::~RasterPolygonLayer()
{
  // Clean up geometries.
  for ( Geometries::iterator iter = _geometries.begin(); iter != _geometries.end(); ++iter )
    delete *iter;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterPolygonLayer::_init()
{
  bool initialized ( Usul::Threads::Safe::get( this->mutex(), _initialized ) );

  // Return if we are already initialized.
  if ( true == initialized )
    return;

  // Set an error handler.
  Detail::PushPopErrorHandler handler;
  
  Guard guard ( this );

  // Return now if we don't have a valid layer to work with.
  if ( false == _layer.valid() )
    return;
  
  // Set the name.
  this->name ( _layer->name() );
  
  // Get the srid.
  _srid = _layer->srid();
  
  // Get the projection text.
  _projectionText = _layer->projectionWKT( _srid );
  
  // Get the extents of the layer.
  Usul::Math::Vec2d ll, ur;
  _layer->extents ( ll, ur );
  
  // Get the well known text for lat lon projection.
  _latLonProjectionText = _layer->projectionWKT ( 4326 );
  
  // Make the transform.
  OGRSpatialReference src ( _projectionText.c_str() ), dst ( _latLonProjectionText.c_str() );
  OGRCoordinateTransformation *transform ( OGRCreateCoordinateTransformation( &src, &dst ) );
  
  // Set the extents.
  if ( 0x0 != transform )
  {
    transform->Transform ( 1, &ll[0], &ll[1] );
    transform->Transform ( 1, &ur[0], &ur[1] );
    Extents extents ( Extents::Vertex ( ll[0], ll[1] ), Extents::Vertex ( ur[0], ur[1] ) );
    this->extents ( extents );
  }

  this->_initGeometries();

  // We are initialized.
  Usul::Threads::Safe::set( this->mutex(), true, _initialized );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the geometries from the server..
//
///////////////////////////////////////////////////////////////////////////////

void RasterPolygonLayer::_initGeometries()
{
  // Set an error handler.
  Detail::PushPopErrorHandler handler;

  Guard guard ( this );

  // Return if no layer.
  if ( false == _layer.valid() )
    return;

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

  std::string name ( Usul::Strings::format ( "PG:", connection->connectionString() ) );
    
  // Get the data source.
  OGRDataSource *vectorData ( driver->CreateDataSource( name.c_str(), 0x0 ) );
    
  // Return if no data.
  if ( 0x0 == vectorData )
    return;

  // Make the query.
  std::string query ( Usul::Strings::format ( "SELECT * ", " FROM ", _layer->tablename() ) );
  
  // Make a layer.
  OGRLayer *layer ( vectorData->ExecuteSQL( query.c_str(), 0x0, 0x0 ) );
  
  if ( 0x0 == layer )
    return;
  
  OGRFeature* feature ( 0x0 );
  
  // Get the color functor.
  typedef Minerva::Core::Functors::BaseColorFunctor ColorFunctor;
  ColorFunctor::RefPtr functor ( _layer->colorFunctor() );
  
  // Get the column for the color.
  const std::string column ( _layer->colorColumn() );
  
  // Random numbers.
  Usul::Adaptors::Random<double> random ( 0.0, 255.0 );
  
  // Get all geometries.
  while ( 0x0 != ( feature = layer->GetNextFeature() ) )
  {
    // Get the index of the column
    const int index ( feature->GetFieldIndex ( column.c_str() ) );
    
    const double value ( index > 0 ? feature->GetFieldAsDouble ( index ) : 0.0 );
    
    // Burn color.
    const osg::Vec4 color ( functor.valid() ? (*functor) ( value ) : osg::Vec4 ( random(), random(), random(), 255 ) );
    
    // Add the burn values.
    _burnValues.push_back ( color[0] * 255 );
    _burnValues.push_back ( color[1] * 255 );
    _burnValues.push_back ( color[2] * 255 );
    _burnValues.push_back ( 200 );
    
    // Add the geometry.
    //OGRGeometry *geometry ( feature->StealGeometry() );
    OGRGeometry *geometry ( feature->GetGeometryRef() );
    
    if ( 0x0 != geometry )
      _geometries.push_back ( geometry->clone() );
    
    // Cleanup.
    OGRFeature::DestroyFeature( feature );
  }
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
  // Make sure we are initialized.
  this->_init();

  // Make the directory and base file name.
  const std::string baseDir ( this->_directory ( width, height, level ) );
  Usul::File::make ( baseDir );
  std::string file ( Usul::Strings::format ( baseDir, this->_baseFileName ( extents ), ".png" ) );  
  
  // Initialize.
  ImagePtr image ( 0x0 );
  
  // Pull it down if it does not exist...
  if ( false == Usul::Predicates::FileExists::test ( file ) )
  {
    // Rasterize.
    image = this->_rasterize ( file, extents, width, height, level );
    
    // Cache the file.
    if ( image.valid() )
    {
      image->flipVertical();
      osgDB::writeImageFile ( *image, file );
    }
  }
  else
  {
    // Read from cache.
    image = osgDB::readImageFile ( file );
  }
  
  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert Gdal data to osg::image.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void convert ( osg::Image* image, GDALDataset *data )
  {
    if ( 0x0 != image && 0x0 != data )
    {
      const int bands ( data->GetRasterCount() );
      if ( bands >= 1 )
      {
        const int width ( data->GetRasterXSize() );
        const int height ( data->GetRasterYSize() );
        
        if ( width > 0 && height > 0 )
        {
          // Loop through the bands.
          for ( int i = 1; i <= bands; ++i )
          {
            GDALRasterBand* band ( data->GetRasterBand ( i ) );
            if ( 0x0 != band )
            {
              std::vector<unsigned char> bytes ( width * height, 0 );
              if ( CE_None == band->RasterIO( GF_Read, 0, 0, width, height, &bytes[0], width, height, GDT_Byte, 0, 0 ) )
              {
                if ( width == image->s() && height == image->t() )
                {
                  unsigned char* data ( image->data() );
                  const int size ( width * height );
                  const int offset ( i - 1 );
                  for ( int i = 0; i < size; ++i )
                  {
                    *(data + offset) = bytes.at ( i );
                    data += bands;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rasterize.
//
///////////////////////////////////////////////////////////////////////////////

RasterPolygonLayer::ImagePtr RasterPolygonLayer::_rasterize ( const std::string& filename, const Extents& extents, unsigned int width, unsigned int height, unsigned int level )
{
  // Typedefs.
  typedef std::vector<int> Bands;
  
  ImagePtr image ( this->_createBlankImage( width, height ) );
  
  // How many channels do we want.
  const unsigned int channels ( 4 );
  
  Guard guard ( this );
  
  // Make a geotiff.
  std::string format ( "GTiff" );
  
  // Find a drive for geotiff.
  GDALDriver *driver ( GetGDALDriverManager()->GetDriverByName( format.c_str() ) );
  
  // Return now if we didn't find a driver.
  if ( 0x0 == driver )
    return 0x0;
  
#if 1
  std::string file ( Usul::Strings::format ( Usul::File::directory ( filename, true ), Usul::File::base ( filename ), ".tif" ) );
  Usul::Scope::RemoveFile remove ( file );
#else
  std::string file ( filename );
#endif
  
  // Create the file.
  GDALDataset *data ( driver->Create( file.c_str(), width, height, channels, GDT_Byte, 0x0 ) );
  
  if ( 0x0 == data )
    return 0x0;
  
  std::vector<char> bytes ( width * height * channels, 0 );
  data->RasterIO( GF_Write, 0, 0, width, height, &bytes[0], width, height, GDT_Byte, channels, 0x0, 0, 0, 0 );
  
  // Get the extents lower left and upper right.
  Extents::Vertex ll ( extents.minimum() );
  Extents::Vertex ur ( extents.maximum() );
  
  // Make the transform.
  OGRSpatialReference dst ( _projectionText.c_str() ), src ( _latLonProjectionText.c_str() );
  OGRCoordinateTransformation *transform ( OGRCreateCoordinateTransformation( &src, &dst ) );
  
  // Set the extents.
  if ( 0x0 != transform )
  {
    transform->Transform ( 1, &ll[0], &ll[1] );
    transform->Transform ( 1, &ur[0], &ur[1] );
  }
  
  // Get the length in x and y.
  const double xLength ( ur[0] - ll[0] );
  const double yLength ( ur[1] - ll[1] );
  
  // Figure out the degrees per pixel.
  const double xResolution  ( xLength / width );
  const double yResolution  ( yLength / height );
  
  std::vector<double> geoTransform ( 6 );
  
  geoTransform[0] = ll[0];          // top left x
  geoTransform[1] = xResolution;    // w-e pixel resolution
  geoTransform[2] = 0;              // rotation, 0 if image is "north up"
  geoTransform[3] = ur[1];          // top left y
  geoTransform[4] = 0;              // rotation, 0 if image is "north up"
  geoTransform[5] = -yResolution;   // n-s pixel resolution
  
  data->SetGeoTransform( &geoTransform[0] );
  
  data->SetProjection( Usul::Threads::Safe::get ( this->mutex(), _projectionText ).c_str() );
  
  // Bands to burn.
  Bands bands;
  
  // Fill in the values.
  for ( unsigned int i = 0; i < channels; ++i )
  {
    // GDAL band index starts at 1.
    bands.push_back ( i + 1 );
  }
    
  // Set an error handler.
  Detail::PushPopErrorHandler handler;
  
  // Burn the raster.
  if ( CE_None == ::GDALRasterizeGeometries( data, bands.size(), &bands[0], 
                            _geometries.size(), (void**) &_geometries[0], 
                            0x0, 0x0, &_burnValues[0], 0x0,
                            GDALTermProgress, 0x0 ) )
  {
    Detail::convert ( image.get(), data );
  }
  
  GDALClose( data ); data = 0x0;

  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterPolygonLayer::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  _dataMemberMap.deserialize ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the string for the value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  std::string makeString ( unsigned int value )
  {
    std::ostringstream out;
    out << std::setw ( 3 ) << std::setfill ( '0' ) << value;
    return out.str();
  }
  std::string makeString ( double value )
  {
    std::ostringstream out;
    
    const double positive ( Usul::Math::absolute ( value ) );
    
    const unsigned long integer ( static_cast < unsigned long > ( positive ) );
    const double decimal ( positive - static_cast < double > ( integer ) );
    
    const unsigned int bufSize ( 2047 );
    char buffer[bufSize + 1];
    ::sprintf ( buffer, "%0.15f", decimal );
    
    std::string temp ( buffer );
    boost::replace_first ( temp, "0.", " " );
    boost::trim_left ( temp );
    
    out << ( ( value >= 0 ) ? 'P' : 'N' ) << std::setfill ( '0' ) << std::setw ( 3 ) << integer << '_' << temp;
    return out.str();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the base file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterPolygonLayer::_baseFileName ( Extents extents ) const
{
  USUL_TRACE_SCOPE;
  
  std::string file ( Usul::Strings::format ( 
                                            Helper::makeString ( extents.minimum()[0] ), '_', 
                                            Helper::makeString ( extents.minimum()[1] ), '_', 
                                            Helper::makeString ( extents.maximum()[0] ), '_', 
                                            Helper::makeString ( extents.maximum()[1] ) ) );
  std::replace ( file.begin(), file.end(), '.', '-' );
  
  return file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterPolygonLayer::_directory ( unsigned int width, unsigned int height, unsigned int level ) const
{
  USUL_TRACE_SCOPE;
  
  std::string name ( this->name() );
  boost::trim_left ( name );
  std::replace ( name.begin(), name.end(), ' ', '_' );
  std::replace ( name.begin(), name.end(), ':', '_' );
  std::replace ( name.begin(), name.end(), '/', '_' );
  std::replace ( name.begin(), name.end(), '&', '_' );
  std::replace ( name.begin(), name.end(), '?', '_' );
  std::replace ( name.begin(), name.end(), '.', '_' );
  
  const std::string resolution ( Usul::Strings::format ( 'W', width, '_', 'H', height ) );
  const std::string levelString ( Usul::Strings::format ( 'L', Helper::makeString ( level ) ) );
  
  std::string dir ( Usul::Strings::format ( Usul::Threads::Safe::get ( this->mutex(), _dir ), '/', 
                                           Usul::App::Application::instance().program(), '/',
                                           name, '/', resolution, '/', levelString, '/' ) );
  std::replace ( dir.begin(), dir.end(), '\\', '/' );
  return dir;
}
