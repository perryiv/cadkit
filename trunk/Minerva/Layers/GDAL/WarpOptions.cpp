
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around GDALWarpOptions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/WarpOptions.h"

#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_alg.h"
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

WarpOptions::WarpOptions ( 
    GDALDataset* src, 
    GDALDataset* dst, 
    bool useAlpha,
    double defaultNoDataValue ) :
  _options ( ::GDALCreateWarpOptions() )
{
  const int numSourceBands ( src->GetRasterCount() );
  const int numDestinationBands ( dst->GetRasterCount() );

  int hasNoData ( FALSE );

  // Get the no data value.
  double noDataValue ( src->GetRasterBand ( 1 )->GetNoDataValue( &hasNoData ) );

  if ( FALSE == hasNoData )
  {
    noDataValue = defaultNoDataValue;
  }

  // Set the no data value.
  for ( int i = 1; i <= numDestinationBands; ++i )
  {
    GDALRasterBand* band1 ( dst->GetRasterBand ( i ) );
    band1->SetNoDataValue ( noDataValue );
  }

  // Initialize with no data.
  char ** warpOptions = 0x0;
  warpOptions = ::CSLSetNameValue ( warpOptions, "INIT_DEST", "NO_DATA" );
  warpOptions = ::CSLSetNameValue ( warpOptions, "UNIFIED_SRC_NODATA", "YES" );
  _options->papszWarpOptions = warpOptions;

  _options->padfDstNoDataReal = (double*) ( CPLMalloc(sizeof(double) * numDestinationBands ) );
  _options->padfDstNoDataImag = (double*) ( CPLMalloc(sizeof(double) * numDestinationBands ) );

  _options->padfSrcNoDataReal = (double*) ( CPLMalloc(sizeof(double) * numSourceBands ) );
  _options->padfSrcNoDataImag = (double*) ( CPLMalloc(sizeof(double) * numSourceBands ) );

  // We want cubic B-spline interpolation.
  _options->eResampleAlg = GRA_Bilinear;

  _options->hSrcDS = src;
  _options->hDstDS = dst;

  _options->nBandCount = numSourceBands; // Number of bands to process.
  _options->panSrcBands = (int *) CPLMalloc(sizeof(int) * numSourceBands );
  _options->panDstBands = (int *) CPLMalloc(sizeof(int) * numDestinationBands );

  for ( int i = 0; i < numSourceBands; ++i )
  {
    _options->panSrcBands[i] = i + 1;

    // Setting the imaginary no data value causes the warping to create bad images.
    _options->padfSrcNoDataReal[i] = noDataValue;
    _options->padfSrcNoDataImag[i] = 0.0;
  }

  for ( int i = 0; i < numDestinationBands; ++i )
  {
    _options->panDstBands[i] = i + 1;

    // Setting the imaginary no data value causes the warping to create bad images.
    _options->padfDstNoDataReal[i] = noDataValue;
    _options->padfDstNoDataImag[i] = 0.0;
  }

  if ( useAlpha && 4 == numSourceBands )
  {
    _options->nSrcAlphaBand = 4;
  }

  if ( useAlpha && 4 == numDestinationBands )
  {
    _options->nDstAlphaBand = 4;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WarpOptions::~WarpOptions()
{
  ::GDALDestroyWarpOptions ( _options );
}
