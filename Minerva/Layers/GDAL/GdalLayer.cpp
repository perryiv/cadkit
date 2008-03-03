
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/GdalLayer.h"
#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Temp.h"
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
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < GdalLayer > > _creator_for_ArcAscii ( "Arc Ascii (*.asc)", "*.asc" );
}


USUL_FACTORY_REGISTER_CREATOR ( GdalLayer );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS( GdalLayer, GdalLayer::BaseClass );

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
      CPLPushErrorHandler( (CPLErrorHandler) Detail::errorHandler );
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

GdalLayer::GdalLayer () : 
  BaseClass(),
  _data ( 0x0 ),
  _filename()
{
  this->_addMember ( "filename", _filename );
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
//  Make an osg::Image.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Image* makeImage ( unsigned int width, unsigned int height, int bands, GDALDataType type )
  {
    // Only handle 1, 3, and 4 bands.
    if ( 1 != bands || 3 != bands || 4 != bands )
      return 0x0;
    
    osg::ref_ptr<osg::Image> result ( new osg::Image );
    
    GLenum pixelFormat ( GL_LUMINANCE );
    
    if ( 3 == bands )
      pixelFormat = GL_RGB;
    else if ( 4 == bands )
      pixelFormat = GL_RGBA;
    
    GLenum dataType ( GL_UNSIGNED_BYTE );
    
    switch ( type )
    {
      case GDT_Byte:
        dataType = GL_UNSIGNED_BYTE;
        break;
      case GDT_UInt16:
        dataType = GL_UNSIGNED_SHORT;
        break;
      case GDT_Int16:
        dataType = GL_SHORT;
        break;
      case GDT_UInt32:
        dataType = GL_UNSIGNED_INT;
        break;
      case GDT_Int32:
        dataType = GL_INT;
        break;
      case GDT_Float32:
        dataType = GL_FLOAT;
        break;
      case GDT_Float64:
        dataType = GL_DOUBLE;
        break;
      default:
        return 0x0; // We don't handle this data type.
    }
    
    result->allocateImage ( width, height, 1, pixelFormat, dataType );
    
    return result.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert Gdal data to osg::image.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class PixelType >
  void convert ( osg::Image* image, GDALDataset *data, GDALDataType type )
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
              std::vector<PixelType> bytes ( width * height, 0 );
              if ( CE_None == band->RasterIO( GF_Read, 0, 0, width, height, &bytes[0], width, height, type, 0, 0 ) )
              {
                PixelType* data ( reinterpret_cast<PixelType*> ( image->data() ) );
                const int size ( width * height );
                
                if ( width == image->s() && height == image->t() && 0x0 != data && size == static_cast<int> ( bytes.size() ) )
                {
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
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

GdalLayer::ImagePtr GdalLayer::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * job, IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  Detail::PushPopErrorHandler error;
  
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
  
  // Find a drive for geotiff.
  GDALDriver *driver ( GetGDALDriverManager()->GetDriverByName ( format.c_str() ) );
  
  // Return now if we didn't find a driver.
  if ( 0x0 == driver )
    return 0x0;
  
  Usul::File::Temp temp;
  
  // Create the file.
  GDALDataset *data ( driver->Create( temp.name().c_str(), width, height, bands, type, 0x0 ) );
  
  if ( 0x0 == data )
    return 0x0;
  
  // Get the extents lower left and upper right.
  Extents::Vertex ll ( extents.minimum() );
  Extents::Vertex ur ( extents.maximum() );
  
  // Make the transform.
  OGRSpatialReference src ( _data->GetProjectionRef() );
  OGRSpatialReference dst;
  dst.SetWellKnownGeogCS ( "WGS84" );
  
  // Get the length in x and y.
  const double xLength ( ur[0] - ll[0] );
  const double yLength ( ur[1] - ll[1] );
  
  // Figure out the pixel resolution.
  const double xResolution  ( xLength / width );
  const double yResolution  ( yLength / height );
  
  std::vector<double> geoTransform ( 6 );
  
  geoTransform[0] = ll[0];          // top left x
  geoTransform[1] = xResolution;    // w-e pixel resolution
  geoTransform[2] = 0;              // rotation, 0 if image is "north up"
  geoTransform[3] = ur[1];          // top left y
  geoTransform[4] = 0;              // rotation, 0 if image is "north up"
  geoTransform[5] = -yResolution;   // n-s pixel resolution
  
  if ( CE_None != data->SetGeoTransform( &geoTransform[0] ) )
    return 0x0;
  
  char *wkt ( 0x0 );
  if ( CE_None != dst.exportToWkt ( &wkt ) )
    return 0x0;
    
  if ( CE_None != data->SetProjection( wkt ) )
    return 0x0;
  
  GDALWarpOptions *options = GDALCreateWarpOptions();
  
  options->hSrcDS = _data;
  options->hDstDS = data;
  
  options->nBandCount = bands;
  options->panSrcBands = (int *) CPLMalloc(sizeof(int) * options->nBandCount );
  options->panDstBands = (int *) CPLMalloc(sizeof(int) * options->nBandCount );
  
  for ( int i = 0; i < bands; ++i )
  {
    options->panSrcBands[i] = i + 1;
    options->panDstBands[i] = i + 1;
  }
  
  options->pfnProgress = GDALTermProgress;   
  
  // Establish reprojection transformer. 
  
  options->pTransformerArg = 
  GDALCreateGenImgProjTransformer( _data, 
                                  GDALGetProjectionRef( _data ), 
                                  data,
                                  GDALGetProjectionRef( data ), 
                                  FALSE, 0.0, 1 );
  options->pfnTransformer = GDALGenImgProjTransform;
  
  // Initialize and execute the warp operation. 
  
  GDALWarpOperation operation;
  
  operation.Initialize( options );
  operation.ChunkAndWarpImage( 0, 0, 
                               GDALGetRasterXSize( data ), 
                               GDALGetRasterYSize( data ) );
  
  GDALDestroyGenImgProjTransformer( options->pTransformerArg );
  GDALDestroyWarpOptions( options );
  
  ImagePtr image ( Detail::makeImage ( width, height, bands, type ) );
  
  // Return if we couldn't create the proper image type.
  if ( false == image.valid() )
    return 0x0;
  
  switch ( type )
  {
    case GDT_Byte:
      Detail::convert<unsigned char> ( image.get(), data, type );
      break;
    case GDT_UInt16:
      Detail::convert<unsigned short> ( image.get(), data, type );
      break;
    case GDT_Int16:
      Detail::convert<short> ( image.get(), data, type );
      break;
    case GDT_UInt32:
      Detail::convert<unsigned int> ( image.get(), data, type );
      break;
    case GDT_Int32:
      Detail::convert<int> ( image.get(), data, type );
      break;
    case GDT_Float32:
      Detail::convert<float> ( image.get(), data, type );
      break;
    case GDT_Float64:
      Detail::convert<double> ( image.get(), data, type );
      break;
    default:
      return 0x0; // We don't handle this data type.
  }
  
  ::GDALClose ( data );
  
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
    
    printf( "Driver: %s/%s\n",
           _data->GetDriver()->GetDescription(), 
           _data->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
    
    printf( "Size is %dx%dx%d\n", 
           _data->GetRasterXSize(), _data->GetRasterYSize(),
           _data->GetRasterCount() );
    
    const char* projection ( _data->GetProjectionRef() );
    if( 0x0 != projection )
      printf( "Projection is `%s'\n", projection );
    
    if( CE_None == _data->GetGeoTransform( &geoTransform[0] ) )
    {
      printf( "Origin = (%.6f,%.6f)\n",
             geoTransform[0], geoTransform[3] );
      
      printf( "Pixel Size = (%.6f,%.6f)\n",
             geoTransform[1], geoTransform[5] );
      
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
        
        std::cout << "Lower left: "  << ll[0] << " " << ll[1] << std::endl;
        std::cout << "Upper right: " << ur[0] << " " << ur[1] << std::endl;
        
        this->extents ( extents );
      }
    }
  }
}
