
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/RasterLayerGDAL.h"
#include "Minerva/Layers/GDAL/Convert.h"
#include "Minerva/Layers/GDAL/Common.h"
#include "Minerva/Layers/GDAL/MakeImage.h"

#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
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

using namespace Minerva::Layers::GDAL;

///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  namespace MF = Minerva::Core::Factory;
  MF::RegisterReader < MF::TypeCreator < RasterLayerGDAL > > _creator_for_ArcAscii  ( "Arc Ascii (*.asc)", "*.asc" );
  MF::RegisterReader < MF::TypeCreator < RasterLayerGDAL > > _creator_for_ArcBinary ( "Arc Binary (*.adf)", "*.adf" );
  MF::RegisterReader < MF::TypeCreator < RasterLayerGDAL > > _creator_for_DEM       ( "Digital Elevation Model (*.dem)", "*.dem" );
  MF::RegisterReader < MF::TypeCreator < RasterLayerGDAL > > _creator_for_SRTM      ( "NASA SRTM (*.hgt)", "*.hgt" );
  //MF::RegisterReader < MF::TypeCreator < RasterLayerGDAL > > _creator_for_TIFF      ( "TIFF (*.tiff *.tif)", "*.tiff,*.tif" );
  MF::RegisterReader < MF::TypeCreator < RasterLayerGDAL > > _creator_for_VRT       ( "GDAL Virtual Format (*.vrt)", ".vrt" );
}


// Register creators.  "GdalLayer" is for backwards compatibilty.
namespace { Usul::Factory::RegisterCreator < Usul::Factory::TypeCreator < RasterLayerGDAL > > _creator0 ( "GdalLayer" ); }
namespace { Usul::Factory::RegisterCreator < Usul::Factory::TypeCreator < RasterLayerGDAL > > _creator1 ( "RasterLayerGDAL" ); }


USUL_IMPLEMENT_IUNKNOWN_MEMBERS( RasterLayerGDAL, RasterLayerGDAL::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Default Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerGDAL::RasterLayerGDAL () : 
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
//  Construct from an osg Image.  Assume WGS 84.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerGDAL::RasterLayerGDAL ( ImagePtr image, const Extents& extents ) : BaseClass(),
  _data ( 0x0 ),
  _filename()
{
  if ( image.valid() )
  {
    image->flipVertical();

    const unsigned int width ( image->s() );
    const unsigned int height ( image->t() );

    // Create the dataset ( hack for now... )
    _data = RasterLayerGDAL::_createDataset ( extents, width, height, 1, GDT_Float32 );

    if ( 0x0 != _data )
    {
      GDALRasterBand* band ( _data->GetRasterBand ( 1 ) );
      if ( 0x0 != band )
      {
        band->RasterIO( GF_Write, 0, 0, width, height, image->data(), width, height, GDT_Float32, 0, 0 );
      }
    }
  }

  this->extents ( extents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerGDAL::RasterLayerGDAL ( const RasterLayerGDAL& rhs ) :
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

RasterLayerGDAL::~RasterLayerGDAL()
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

Usul::Interfaces::IUnknown* RasterLayerGDAL::queryInterface ( unsigned long iid )
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

Usul::Interfaces::IUnknown* RasterLayerGDAL::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterLayerGDAL( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerGDAL::ImagePtr RasterLayerGDAL::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * job, IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Let the base class go first.
  {
    ImagePtr answer ( BaseClass::texture ( extents, width, height, level, job, caller ) );
    if ( true == answer.valid() )
      return answer;
  }

  Minerva::Detail::PushPopErrorHandler error;
  
  // Now guard.
  Guard guard ( this );

  // Get the data set.
  GDALDataset *data ( _data );

  // Return if no data.
  if ( 0x0 == data )
    return 0x0;
  
  // Get the number of bands.
  const int bands ( data->GetRasterCount() );
  
  // Return if we don't have any bands.
  if ( 0 == bands )
    return 0x0;
  
  // Get the data type.  Assume that all bands have the same type.
  GDALDataType type ( data->GetRasterBand ( 1 )->GetRasterDataType() );
  
  // Create the dataset.
  GDALDataset *tile ( RasterLayerGDAL::_createDataset ( extents, width, height, bands, type ) );
  
  if ( 0x0 == tile )
    return 0x0;
  
  // Make sure data set is closed.
  Usul::Scope::Caller::RefPtr closeDataSet ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( tile, GDALClose ) ) );

  // Create the options.  Make sure the options are destroyed.
  GDALWarpOptions *options ( GDALCreateWarpOptions() );
  Usul::Scope::Caller::RefPtr destroyOptions ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( options, ::GDALDestroyWarpOptions ) ) );

  int hasNoData ( FALSE );

  // Get the no data value.
  const double noDataValue ( data->GetRasterBand ( 1 )->GetNoDataValue( &hasNoData ) );

  if ( TRUE == hasNoData )
  {
    // Set the no data value.
    for ( int i = 1; i <= bands; ++i )
    {
      GDALRasterBand* band1 ( tile->GetRasterBand ( i ) );
      band1->SetNoDataValue( noDataValue );
    }

    // Initialize with no data.
    char ** warpOptions = 0x0;
    warpOptions = ::CSLSetNameValue( warpOptions, "INIT_DEST", "NO_DATA" );
    options->papszWarpOptions = warpOptions;

    options->padfDstNoDataReal = (double*) ( CPLMalloc(sizeof(double) * bands ) );
    options->padfDstNoDataImag = (double*) ( CPLMalloc(sizeof(double) * bands ) );
    options->padfSrcNoDataReal = (double*) ( CPLMalloc(sizeof(double) * bands ) );
    options->padfSrcNoDataImag = (double*) ( CPLMalloc(sizeof(double) * bands ) );
  }

  // We want cubic B-spline interpolation.
  options->eResampleAlg = GRA_CubicSpline;
  
  options->hSrcDS = data;
  options->hDstDS = tile;

  options->nBandCount = bands;
  options->panSrcBands = (int *) CPLMalloc(sizeof(int) * options->nBandCount );
  options->panDstBands = (int *) CPLMalloc(sizeof(int) * options->nBandCount );

  for ( int i = 0; i < bands; ++i )
  {
    options->panSrcBands[i] = i + 1;
    options->panDstBands[i] = i + 1;

    if ( TRUE == hasNoData )
    {
      options->padfSrcNoDataReal[i] = noDataValue;
      options->padfDstNoDataImag[i] = noDataValue;
      options->padfDstNoDataReal[i] = noDataValue;
      options->padfSrcNoDataImag[i] = noDataValue;
    }
  }
  
  options->pfnProgress = GDALTermProgress;

  // Establish reprojection transformer. 
  options->pTransformerArg = GDALCreateGenImgProjTransformer( data, 
                                                              GDALGetProjectionRef( data ), 
                                                              tile, 
                                                              GDALGetProjectionRef( tile ), 
                                                              FALSE, 0.0, 0 );
  
  // Make sure we got a transformer.
  if ( 0x0 == options->pTransformerArg )
    return 0x0;
  
  // Make sure the transformer is destroyed.
  Usul::Scope::Caller::RefPtr destroyTransformer ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( options->pTransformerArg, GDALDestroyGenImgProjTransformer ) ) );
  
  options->pfnTransformer = GDALGenImgProjTransform;
  
  // Check for canceled before starting long running task below...
  BaseClass::_checkForCanceledJob ( job );

  // Initialize and execute the warp operation.   
  GDALWarpOperation operation;
  operation.Initialize( options );
  operation.ChunkAndWarpImage( 0, 0, width, height );
  
  // Convert to an osg image.
  ImagePtr image ( Minerva::convert ( tile ) );

  // Save the image to the cache.
#ifndef __APPLE__
  #ifndef _DEBUG
  BaseClass::_writeImageToCache ( extents, width, height, level, image );
  #endif
#endif
  
  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerGDAL::deserialize ( const XmlTree::Node &node )
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

void RasterLayerGDAL::read ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  USUL_TRACE_SCOPE;
  
  // Add an error handler.
  Minerva::Detail::PushPopErrorHandler error;
  
  if ( true == this->name().empty() )
    this->name ( filename );
  
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

      RasterLayerGDAL::_createGeoTransform( geoTransform, this->extents(), width, height );

      _data->SetGeoTransform ( &geoTransform[0] );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerGDAL::_print ( GDALDataset * data )
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

std::string RasterLayerGDAL::_cacheDirectory() const
{
  USUL_TRACE_SCOPE;

  const std::string file ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
  const std::size_t hashValue ( BaseClass::_hashString ( Usul::File::fullPath ( file ) ) );
  const std::string dir ( BaseClass::_buildCacheDir ( this->baseCacheDirectory(), "file_system", hashValue ) );

  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension for the cached files.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerGDAL::_cacheFileExtension() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  //const std::string ext ( Usul::File::extension ( _filename ) );
  //return ext;

  // Always save as tiff.
  return ".tif";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create an in memory dataset.
//
///////////////////////////////////////////////////////////////////////////////

GDALDataset * RasterLayerGDAL::_createDataset ( const Extents& e, unsigned int width, unsigned int height, int bands, unsigned int type )
{
  // Make an in memory raster.
  std::string format ( "MEM" );
  
  // Find a driver for in memory raster.
  GDALDriver *driver ( GetGDALDriverManager()->GetDriverByName ( format.c_str() ) );
  
  // Return now if we didn't find a driver.
  if ( 0x0 == driver )
    return 0x0;
  
  // Create the file.
  GDALDataset *data ( driver->Create( Usul::File::Temp::file().c_str(), width, height, bands, static_cast<GDALDataType> ( type ), 0x0 ) );
  
  if ( 0x0 == data )
    return 0x0;
  
  // Make the transform.
  OGRSpatialReference dst;
  dst.SetWellKnownGeogCS ( "WGS84" );
  
  // Create the geo transform.
  std::vector<double> geoTransform ( 6 );
  RasterLayerGDAL::_createGeoTransform ( geoTransform, e, width, height );
  
  if ( CE_None != data->SetGeoTransform( &geoTransform[0] ) )
    return 0x0;
  
  char *wkt ( 0x0 );
  if ( CE_None != dst.exportToWkt ( &wkt ) )
    return 0x0;
    
  if ( CE_None != data->SetProjection( wkt ) )
    return 0x0;
  
  return data;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a geo transform.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerGDAL::_createGeoTransform ( GeoTransform &transfrom, const Extents& extents, unsigned int width, unsigned int height )
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