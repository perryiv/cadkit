
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_GDAL_CONVERT_H__
#define __MINERVA_GDAL_CONVERT_H__

#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_error.h"

#include <limits>

namespace Minerva
{
  namespace Detail
  {
    // Generic implementation.
    template<class PixelType>
    void convert ( osg::Image* image, GDALDataset *data, GDALDataType type, int width, int height )
    {
      // Get the number of bands.
      const int bands ( data->GetRasterCount() );
      
      // Total number of pixels.
      const int size ( width * height );
      
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
            
            if ( size == static_cast<int> ( bytes.size() ) )
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
    
    template< class PixelType, int bands> struct Convert;
    
    // One band.
    template<class PixelType>
    struct Convert <PixelType, 1>
    {
      static void convert ( osg::Image* image, GDALDataset *data, GDALDataType type, int width, int height )
      {       
        GDALRasterBand* band ( data->GetRasterBand ( 1 ) );
        if ( 0x0 == band )
          return;
        
        // The no data value.
        const double noDataValue ( band->GetNoDataValue() );
        
        // Number of pixel.
        const int size ( width * height );
        
        std::vector<PixelType> bytes ( size, 0 );
        if ( CE_None == band->RasterIO( GF_Read, 0, 0, width, height, &bytes[0], width, height, type, 0, 0 ) )
        {
          PixelType* data ( reinterpret_cast<PixelType*> ( image->data() ) );
          
          for ( int i = 0; i < size; ++i )
          {
            PixelType value ( bytes.at ( i ) );
            *data++ = value;
            
            // Make transparent if value is equal to no data.
            *data++ = ( value == static_cast<PixelType> ( noDataValue ) ? 0.0 : std::numeric_limits<PixelType>::max() );
          }
        }
      }
    };
    
    // Three bands.
    template<class PixelType>
    struct Convert <PixelType, 3>
    {
      static void convert ( osg::Image* image, GDALDataset *data, GDALDataType type, int width, int height )
      {       
        GDALRasterBand* band0 ( data->GetRasterBand ( 1 ) );
        GDALRasterBand* band1 ( data->GetRasterBand ( 2 ) );
        GDALRasterBand* band2 ( data->GetRasterBand ( 3 ) );
        
        if ( 0x0 == band0 || 0x0 == band1 || 0x0 == band2 )
          return;
        
        // Number of pixels.
        const int size ( width * height );
        
        std::vector<PixelType> bytes0 ( size, 0 );
        std::vector<PixelType> bytes1 ( size, 0 );
        std::vector<PixelType> bytes2 ( size, 0 );
        
        if ( CE_None == band0->RasterIO( GF_Read, 0, 0, width, height, &bytes0[0], width, height, type, 0, 0 ) && 
             CE_None == band1->RasterIO( GF_Read, 0, 0, width, height, &bytes1[0], width, height, type, 0, 0 ) &&
             CE_None == band2->RasterIO( GF_Read, 0, 0, width, height, &bytes2[0], width, height, type, 0, 0 ) )
        {
          // Get the data.
          PixelType* data ( reinterpret_cast<PixelType*> ( image->data() ) );
          
          for ( int i = 0; i < size; ++i )
          {
            PixelType r ( bytes0.at ( i ) );
            PixelType g ( bytes1.at ( i ) );
            PixelType b ( bytes2.at ( i ) );
            
            *data++ = r;
            *data++ = g;
            *data++ = b;
            *data++ = std::numeric_limits<PixelType>::max();
          }
        }
      }
    };
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Convert Gdal data to osg::image.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  template < class PixelType >
  void convert ( osg::Image* image, GDALDataset *data, GDALDataType type )
  {
    // Check for bad input.
    if ( 0x0 == image && 0x0 == data )
      return;
    
    // Get the number of bands.
    const int bands ( data->GetRasterCount() );
    
    // Get the width and height.
    const int width ( data->GetRasterXSize() );
    const int height ( data->GetRasterYSize() );
    
    // Make sure these values are sane.
    if ( bands < 1 || width <= 0 || height <= 0 || width != image->s() || height != image->t() )
      return;
    
    // Redirect.
    switch ( bands )
    {
    case 1:
      Detail::Convert<PixelType, 1>::convert ( image, data, type, width, height );
      break;
    case 3:
      Detail::Convert<PixelType, 3>::convert ( image, data, type, width, height );
      break;
    default:
      Detail::convert<PixelType> ( image, data, type, width, height );
    }
    
    image->flipVertical();
  }
}


#endif // __MINERVA_GDAL_CONVERT_H__
