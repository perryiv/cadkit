
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/GdalLayer.h"
#include "Minerva/Layers/GDAL/Convert.h"
#include "Minerva/Layers/GDAL/Common.h"
#include "Minerva/Layers/GDAL/MakeImage.h"

#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/File/Temp.h"
#include "Usul/MPL/StaticAssert.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_alg.h"
#include "gdalwarper.h"
#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogrsf_frmts.h"
#include "cpl_error.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < GdalLayer > > _creator_for_ArcAscii  ( "Arc Ascii (*.asc)", "*.asc" );
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < GdalLayer > > _creator_for_ArcBinary ( "Arc Binary (*.adf)", "*.adf" );
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < GdalLayer > > _creator_for_DEM       ( "Digital Elevation Model (*.dem)", "*.dem" );
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < GdalLayer > > _creator_for_SRTM      ( "NASA SRTM (*.hgt)", "*.hgt" );
}


USUL_FACTORY_REGISTER_CREATOR ( GdalLayer );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS( GdalLayer, GdalLayer::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Default Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GdalLayer::GdalLayer () : 
  BaseClass(),
  _data ( 0x0 ),
  _filename()
{
  this->_addMember ( "filename", _filename );
  
  // Sanity check.  TODO: Change from using built in types directy to Usul::Types
  USUL_STATIC_ASSERT ( sizeof ( GByte )   == sizeof ( unsigned char ) );
  USUL_STATIC_ASSERT ( sizeof ( GUInt16 ) == sizeof ( unsigned short ) );
  USUL_STATIC_ASSERT ( sizeof ( GInt16 )  == sizeof ( short ) );
  USUL_STATIC_ASSERT ( sizeof ( GInt32 )  == sizeof ( int ) );
  USUL_STATIC_ASSERT ( sizeof ( GUInt32 ) == sizeof ( unsigned int ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GdalLayer::GdalLayer ( const GdalLayer& rhs ) :
  BaseClass ( rhs ),
  _data ( rhs._data ),
  _filename ( rhs._filename )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GdalLayer::~GdalLayer()
{
  // Clean up.
  if ( 0x0 != _data )
    ::GDALClose ( _data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* GdalLayer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IRead::IID:
    return static_cast< Usul::Interfaces::IRead * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* GdalLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new GdalLayer( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

GdalLayer::ImagePtr GdalLayer::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * job, IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Let the base class go first.
  {
    ImagePtr answer ( BaseClass::texture ( extents, width, height, level, job, caller ) );
    if ( true == answer.valid() )
      return answer;
  }

  // Now guard.
  Guard guard ( this );
  
  Minerva::Detail::PushPopErrorHandler error;
  
  // Return if no data.
  if ( 0x0 == _data )
    return 0x0;
  
  // Get the number of bands.
  const int bands ( _data->GetRasterCount() );
  
  // Return if we don't have any bands.
  if ( 0 == bands )
    return 0x0;
  
  // Get the data type.  Assume that all bands have the same type.
  GDALDataType type ( _data->GetRasterBand ( 1 )->GetRasterDataType() );
  
  // Make an in memory raster.
  std::string format ( "MEM" );
  
  // Find a driver for in memory raster.
  GDALDriver *driver ( GetGDALDriverManager()->GetDriverByName ( format.c_str() ) );
  
  // Return now if we didn't find a driver.
  if ( 0x0 == driver )
    return 0x0;
  
  Usul::File::Temp temp;
  
  // Create the file.
  GDALDataset *data ( driver->Create( temp.name().c_str(), width, height, bands, type, 0x0 ) );
  
  if ( 0x0 == data )
    return 0x0;
  
  // Make sure data set is closed.
  Usul::Scope::Caller::RefPtr closeDataSet ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( data, GDALClose ) ) );
  
  // Make the transform.
  OGRSpatialReference src ( _data->GetProjectionRef() );
  OGRSpatialReference dst;
  dst.SetWellKnownGeogCS ( "WGS84" );
  
  // Create the geo transform.
  std::vector<double> geoTransform ( 6 );
  GdalLayer::_createGeoTransform ( geoTransform, extents, width, height );
  
  if ( CE_None != data->SetGeoTransform( &geoTransform[0] ) )
    return 0x0;
  
  char *wkt ( 0x0 );
  if ( CE_None != dst.exportToWkt ( &wkt ) )
    return 0x0;
    
  if ( CE_None != data->SetProjection( wkt ) )
    return 0x0;
  
  const double noDataValue ( _data->GetRasterBand ( 1 )->GetNoDataValue() );

  for ( int i = 1; i <= bands; ++i )
  {
    GDALRasterBand* band1 (  data->GetRasterBand ( i ) );
    
    band1->SetNoDataValue( noDataValue );
  }
  
  // Print info.
  //this->_print( data );
  
  GDALWarpOptions *options ( GDALCreateWarpOptions() );

  // Initialize with no data.
  char ** warpOptions = 0x0;
  warpOptions = ::CSLSetNameValue( warpOptions, "INIT_DEST", "NO_DATA" );
  options->papszWarpOptions = warpOptions;

  // We want bilinear interpolation.
  options->eResampleAlg = GRA_Bilinear;

  // Make sure the options are destroyed.
  Usul::Scope::Caller::RefPtr destroyOptions     ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( options, ::GDALDestroyWarpOptions ) ) );
  
  options->hSrcDS = _data;
  options->hDstDS = data;
  
  options->nBandCount = bands;
  options->panSrcBands = (int *) CPLMalloc(sizeof(int) * options->nBandCount );
  options->panDstBands = (int *) CPLMalloc(sizeof(int) * options->nBandCount );
  options->padfDstNoDataReal = (double*) ( CPLMalloc(sizeof(double) * options->nBandCount ) );
  options->padfDstNoDataImag = (double*) ( CPLMalloc(sizeof(double) * options->nBandCount ) );
  options->padfSrcNoDataReal = (double*) ( CPLMalloc(sizeof(double) * options->nBandCount ) );
  options->padfSrcNoDataImag = (double*) ( CPLMalloc(sizeof(double) * options->nBandCount ) );

  for ( int i = 0; i < bands; ++i )
  {
    options->panSrcBands[i] = i + 1;
    options->panDstBands[i] = i + 1;

    options->padfSrcNoDataReal[i] = noDataValue;
    options->padfDstNoDataImag[i] = noDataValue;
    options->padfDstNoDataReal[i] = noDataValue;
    options->padfSrcNoDataImag[i] = noDataValue;
  }
  
  options->pfnProgress = GDALTermProgress;
  
  // Establish reprojection transformer. 
  options->pTransformerArg = GDALCreateGenImgProjTransformer( _data, 
                                                              GDALGetProjectionRef( _data ), 
                                                              data, 
                                                              GDALGetProjectionRef( data ), 
                                                              FALSE, 0.0, 0 );
  
  // Make sure we got a transformer.
  if ( 0x0 == options->pTransformerArg )
    return 0x0;
  
  // Make sure the transformer is destroyed.
  Usul::Scope::Caller::RefPtr destroyTransformer ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( options->pTransformerArg, GDALDestroyGenImgProjTransformer ) ) );
  
  options->pfnTransformer = GDALGenImgProjTransform;
  
  // Initialize and execute the warp operation.   
  GDALWarpOperation operation;
  
  operation.Initialize( options );
  operation.ChunkAndWarpImage( 0, 0, width, height );

	ImagePtr image ( Minerva::GDAL::makeImage ( width, height, bands, type ) );
  
  // Return if we couldn't create the proper image type.
  if ( false == image.valid() )
    return 0x0;
  
  switch ( type )
  {
    case GDT_Byte:
      Minerva::convert<unsigned char> ( image.get(), data, type );
      break;
    case GDT_UInt16:
      Minerva::convert<unsigned short> ( image.get(), data, type );
      break;
    case GDT_Int16:
      Minerva::convert<short> ( image.get(), data, type );
      break;
    case GDT_UInt32:
      Minerva::convert<unsigned int> ( image.get(), data, type );
      break;
    case GDT_Int32:
      Minerva::convert<int> ( image.get(), data, type );
      break;
    case GDT_Float32:
      Minerva::convert<float> ( image.get(), data, type );
      break;
    case GDT_Float64:
      Minerva::convert<double> ( image.get(), data, type );
      break;
    default:
      return 0x0; // We don't handle this data type.
  }
  
  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void GdalLayer::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  _dataMemberMap.deserialize ( node );
  
  // Read.
  this->read ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read.
//
///////////////////////////////////////////////////////////////////////////////

void GdalLayer::read ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _filename = filename;
  
  // Open the dataset.
  _data = static_cast< GDALDataset* > ( ::GDALOpen ( filename.c_str(), GA_ReadOnly ) );
  
  if ( 0x0 != _data )
  {
    std::vector<double> geoTransform ( 6 );

    if( CE_None == _data->GetGeoTransform( &geoTransform[0] ) )
    {
      const char* projection ( _data->GetProjectionRef() );

      // Make the transform.
      OGRSpatialReference src ( projection );
      OGRSpatialReference dst;
      dst.SetWellKnownGeogCS ( "WGS84" );
      std::auto_ptr<OGRCoordinateTransformation> transform ( ::OGRCreateCoordinateTransformation( &src, &dst ) );
      
      // Set the extents.
      if ( 0x0 != transform.get() )
      {
        const unsigned int width ( _data->GetRasterXSize() );
        const unsigned int height ( _data->GetRasterYSize() );
        
        const double x ( geoTransform[0] ), y ( geoTransform[3] );
        
        const double xLength ( width * geoTransform[1] );
        const double yLength ( height * geoTransform[5] );
        
        Extents::Vertex ll ( x, y + yLength ), ur ( x + xLength, y );
        transform->Transform ( 1, &ll[0], &ll[1] );
        transform->Transform ( 1, &ur[0], &ur[1] );
        Extents extents ( ll, ur );
        
        std::cout << "Location of " << filename << std::endl;
        std::cout << "Lower left: "  << ll[0] << " " << ll[1] << std::endl;
        std::cout << "Upper right: " << ur[0] << " " << ur[1] << std::endl;
        
        this->extents ( extents );
      }
    }
    else
    {
      const int width ( _data->GetRasterXSize() );
      const int height ( _data->GetRasterYSize() );

      GdalLayer::_createGeoTransform( geoTransform, this->extents(), width, height );

      _data->SetGeoTransform ( &geoTransform[0] );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print.
//
///////////////////////////////////////////////////////////////////////////////

void GdalLayer::_print ( GDALDataset * data )
{
  if ( 0x0 == data )
    return;
  
  const unsigned int size ( 2024 );
  
  {
    std::vector<char> buffer ( size, '\0' );
    sprintf( &buffer[0], "Driver: %s/%s\n",
           data->GetDriver()->GetDescription(), 
           data->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
    std::cout << &buffer[0] << std::endl;
  }
  
  {
    std::vector<char> buffer ( size, '\0' );
    sprintf( &buffer[0], "Size is %dx%dx%d\n", 
           data->GetRasterXSize(), data->GetRasterYSize(),
           data->GetRasterCount() );
    std::cout << &buffer[0] << std::endl;
  }
  
  const char* projection ( data->GetProjectionRef() );
  if( 0x0 != projection )
    std::cout << "Projection is: " << projection << std::endl;
  
  std::vector<double> geoTransform ( 6 );
  if( CE_None == data->GetGeoTransform( &geoTransform[0] ) )
  {
    std::vector<char> buffer ( size, '\0' );
    sprintf( &buffer[0], "Origin = (%.6f,%.6f)\nPixel Size = (%.6f,%.6f)\n",
           geoTransform[0], geoTransform[3], geoTransform[1], geoTransform[5] );
    std::cout << &buffer[0] << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string GdalLayer::_cacheDirectory() const
{
  USUL_TRACE_SCOPE;

  const std::string file ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
  const std::size_t hashValue ( BaseClass::_hashString ( Usul::File::fullPath ( file ) ) );
  const std::string dir ( BaseClass::_buildCacheDir ( this->cacheDirectory(), "file_system", hashValue ) );

  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension for the cached files.
//
///////////////////////////////////////////////////////////////////////////////

std::string GdalLayer::_cacheFileExtension() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  const std::string ext ( Usul::File::extension ( _filename ) );
  return ext;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a geo transform.
//
///////////////////////////////////////////////////////////////////////////////

void GdalLayer::_createGeoTransform ( GeoTransform &transfrom, const Extents& extents, unsigned int width, unsigned int height )
{
  USUL_TRACE_SCOPE_STATIC;

  // Get the extents lower left and upper right.
  Extents::Vertex ll ( extents.minimum() );
  Extents::Vertex ur ( extents.maximum() );
    
  // Get the length in x and y.
  const double xLength ( ur[0] - ll[0] );
  const double yLength ( ur[1] - ll[1] );
  
  // Figure out the pixel resolution.
  const double xResolution  ( xLength / width );
  const double yResolution  ( yLength / height );

  // Make sure there is enough room.
  transfrom.resize ( 6 );
  
  transfrom[0] = ll[0];          // top left x
  transfrom[1] = xResolution;    // w-e pixel resolution
  transfrom[2] = 0;              // rotation, 0 if image is "north up"
  transfrom[3] = ur[1];          // top left y
  transfrom[4] = 0;              // rotation, 0 if image is "north up"
  transfrom[5] = -yResolution;   // n-s pixel resolution
}
