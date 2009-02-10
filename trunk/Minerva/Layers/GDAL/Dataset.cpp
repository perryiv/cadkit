
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around GDALDataset.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/Dataset.h"
#include "Minerva/Layers/GDAL/Convert.h"
#include "Minerva/Core/ElevationData.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/File/Remove.h"
#include "Usul/Scope/Caller.h"
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
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::Dataset ( GDALDataset * data ) : BaseClass(),
  _data ( data )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::Dataset ( const std::string& filename, const Extents& extents, unsigned int width, unsigned int height, int bands, unsigned int type ) : BaseClass(),
  _data ( 0x0 )
{
  USUL_TRACE_SCOPE;
  // Make an in memory raster.
  //const std::string format ( "MEM" );
  const std::string format ( "GTiff" );

  // Find a driver for in memory raster.
  GDALDriver *driver ( GetGDALDriverManager()->GetDriverByName ( format.c_str() ) );

  // Return now if we didn't find a driver.
  if ( 0x0 == driver )
    return;

  // Create the file.
  GDALDataset *data ( driver->Create ( filename.c_str(), width, height, bands, static_cast<GDALDataType> ( type ), 0x0 ) );

  if ( 0x0 == data )
    return;

  // Make the transform.
  OGRSpatialReference dst;
  dst.SetWellKnownGeogCS ( "WGS84" );

  // Create the geo transform.
  std::vector<double> geoTransform ( 6 );
  Dataset::createGeoTransform ( geoTransform, extents, width, height );

  if ( CE_None != data->SetGeoTransform( &geoTransform[0] ) )
    return;

  char *wkt ( 0x0 );
  if ( CE_None != dst.exportToWkt ( &wkt ) )
    return;

  if ( CE_None != data->SetProjection( wkt ) )
    return;

  // If we get here, everything succeeded.  Set the internal pointer.
  _data = data;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::~Dataset()
{
  USUL_TRACE_SCOPE;
  this->close();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Close the file.
//
///////////////////////////////////////////////////////////////////////////////

void Dataset::close()
{
  USUL_TRACE_SCOPE;
  if ( 0x0 != _data )
    ::GDALClose ( _data );

  _data = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the underlying dataset.
//
///////////////////////////////////////////////////////////////////////////////

GDALDataset* Dataset::dataset() const
{
  USUL_TRACE_SCOPE;
  return _data;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all files of the dataset.
//
///////////////////////////////////////////////////////////////////////////////

void Dataset::deleteFiles()
{
  USUL_TRACE_SCOPE;
  if ( 0x0 != _data )
  {
    char** files = _data->GetFileList();
    Usul::Scope::Caller::RefPtr scopeFiles ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( files, ::CSLDestroy ) ) );
    while ( 0x0 != files && 0x0 != *files )
    {
      Usul::File::remove ( *files );
      ++files;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to elevation data.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::ElevationDataPtr Dataset::convertToElevationData() const
{
  USUL_TRACE_SCOPE;
  if ( 0x0 == _data )
    return 0x0;

  GDALRasterBand* band ( _data->GetRasterBand ( 1 ) );
  if ( 0x0 == band )
    return 0x0;

  // Get the width and height.
  const int width ( _data->GetRasterXSize() );
  const int height ( _data->GetRasterYSize() );

  // Read the values.
  const int size ( width * height );
  std::vector<IElevationData::ValueType> bytes ( size, 0 );
  if ( CE_None == band->RasterIO ( GF_Read, 0, 0, width, height, &bytes[0], width, height, GDT_Float32, 0, 0 ) )
  {
    Minerva::Core::ElevationData::RefPtr elevationData ( new Minerva::Core::ElevationData ( width, height ) );

    // The no data value.
    const double noDataValue ( band->GetNoDataValue() );
    elevationData->noDataValue ( static_cast<IElevationData::ValueType> ( noDataValue ) );

    for ( int row = 0; row < height; ++row )
    {
      for ( int column = 0; column < width; ++column )
      {
        const unsigned int index ( column + ( row * width ) );
        elevationData->value ( column, ( height - row - 1 ), bytes.at ( index ) ); 
      }
    }

    return ElevationDataPtr ( elevationData );
  }

  return 0x0;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a geo transform.
//
///////////////////////////////////////////////////////////////////////////////

void Dataset::createGeoTransform ( GeoTransform &transfrom, const Extents& extents, unsigned int width, unsigned int height )
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
