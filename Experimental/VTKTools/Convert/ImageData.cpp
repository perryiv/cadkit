
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "VTKTools/Convert/ImageData.h"

#include "vtkImageData.h"

#include "osg/Image"

using namespace VTKTools::Convert;


///////////////////////////////////////////////////////////////////////////////
//
//  Convert osg image to image data.
//
///////////////////////////////////////////////////////////////////////////////

void ImageData::osgImageToImageData ( const osg::Image &image, vtkImageData &vtkImage )
{
  vtkImage.SetSpacing ( 1.0, 1.0, 1.0 );
  vtkImage.SetOrigin ( 0.0, 0.0, 0.0 );
  vtkImage.SetDimensions ( image.s(), image.t(), image.r() );
  vtkImage.SetNumberOfScalarComponents(1);
  vtkImage.SetScalarTypeToUnsignedChar();
  vtkImage.AllocateScalars();

  // Not sure if this will work.
  //::memcpy ( vtkImage->GetScalarPointer( 0, 0, 0 ), image3d->data( 0, 0, 0 ), image3d->getTotalSizeInBytes() );

  for ( int x = 0; x < image.s(); ++x )
  {
    for ( int y = 0; y < image.t(); ++y )
    {
      for ( int z = 0; z < image.r(); ++z )
      {
        unsigned char *data = static_cast < unsigned char* > ( vtkImage.GetScalarPointer( x, y , z ) );
        
        *data = *image.data( x, y , z );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert image data to osg image.
//
///////////////////////////////////////////////////////////////////////////////

void ImageData::imageDataToOsgImage ( vtkImageData& vtkImage, osg::Image &image )
{
  int dims [3]; 
  vtkImage.GetDimensions ( dims );

  // Need to figure out pixel format and pixel type
  image.allocateImage( dims[0], dims[1], dims[2], GL_RGBA, GL_UNSIGNED_BYTE );

  for ( int x = 0; x < dims[0]; ++x )
  {
    for ( int y = 0; y < dims[1]; ++y )
    {
      for ( int z = 0; z < dims[2]; ++z )
      {
        unsigned char *data = image.data( x, y, z );
        
        *data = *( static_cast < unsigned char* > ( vtkImage.GetScalarPointer( x, y , z ) ) );
      }
    }
  }
}
