
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

#include "Minerva/Layers/GDAL/MakeImage.h"

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
    inline void convert ( osg::Image* image, GDALDataset *data, GDALDataType type, int width, int height )
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
            *data++ = ( ( value == static_cast<PixelType> ( noDataValue ) ) ? static_cast<PixelType> ( 0.0 ) : std::numeric_limits<PixelType>::max() );
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
  inline void convert ( osg::Image* image, GDALDataset *data, GDALDataType type )
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

    const bool hasColorTable ( 1 == bands && 0x0 != data->GetRasterBand ( 1 )->GetColorTable() );

    if ( hasColorTable && GDT_Byte == type )
    {
      GDALRasterBand* band ( data->GetRasterBand ( 1 ) );
      if ( 0x0 == band )
        return;

      const GDALColorTable* table ( band->GetColorTable() );
      
      // Number of pixel.
      const int size ( width * height );
      
      std::vector<unsigned char> bytes ( size, 0 );
      if ( CE_None == band->RasterIO( GF_Read, 0, 0, width, height, &bytes[0], width, height, type, 0, 0 ) )
      {
        unsigned char* data ( reinterpret_cast<unsigned char*> ( image->data() ) );
        
        for ( int i = 0; i < size; ++i )
        {
          unsigned char value ( bytes.at ( i ) );

          const GDALColorEntry* entry ( table->GetColorEntry ( value ) );

          *data++ = entry->c1;
          *data++ = entry->c2;
          *data++ = entry->c3;
          *data++ = entry->c4;
        }
      }
    }
    else
    {
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
    }
    
    image->flipVertical();
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Convert Gdal data to osg::image.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  inline osg::Image* convert ( GDALDataset * data )
  {
    // Return if no data.
    if ( 0x0 == data )
      return 0x0;

    // Get the number of bands.
    const int bands ( data->GetRasterCount() );

    // Get the data type.  Assume that all bands have the same type.
    GDALDataType type ( data->GetRasterBand ( 1 )->GetRasterDataType() );

    // Get the width and height.
    const unsigned int width ( data->GetRasterXSize() );
    const unsigned int height ( data->GetRasterYSize() );

    const bool hasColorTable ( 1 == bands && 0x0 != data->GetRasterBand ( 1 )->GetColorTable() );

    osg::ref_ptr<osg::Image> image ( Minerva::GDAL::makeImage ( width, height, bands, type, hasColorTable ) );

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

    return image.release();
  }
}


#endif // __MINERVA_GDAL_CONVERT_H__
