
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Image models.
//
///////////////////////////////////////////////////////////////////////////////

#include "ImageDocument.h"

#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/Image.h"
#include "FoxTools/Headers/Vec4f.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Policies/Update.h"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ImageDocument, ImageDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ImageDocument::ImageDocument() : BaseClass ( "Image Document" ),
_image ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ImageDocument::~ImageDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ImageDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IImage::IID:
    return static_cast < Usul::Interfaces::IImage* > ( this );
  case Usul::Interfaces::IFoxImage::IID:
    return static_cast < Usul::Interfaces::IFoxImage* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ImageDocument::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ImageDocument::canInsert ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ImageDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "jpeg" || ext == "jpg" || ext == "tiff" || ext == "tif" || ext == "gif" || ext == "png" || ext == "bmp" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ImageDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "jpeg" || ext == "jpg" || ext == "tiff" || ext == "tif" || ext == "gif" || ext == "png" || ext == "bmp" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void ImageDocument::read ( const std::string &name, Unknown *caller )
{
  ImagePtr image ( osgDB::readImageFile( name ) );

  if( !image.valid() )
    throw std::runtime_error ("Error 1350090608: Could not load image file: " + name );

  image->setFileName( name );

  _image = image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ImageDocument::write ( const std::string &name, Unknown *caller ) const
{
  switch ( _image->getPixelFormat() )
  {
  case GL_LUMINANCE:
    {
      osg::ref_ptr< osg::Image > image ( new osg::Image );
      image->allocateImage( _image->s(), _image->t(), 1, GL_RGB, _image->getDataType() );

      for( int t = 0; t < _image->t(); ++t )
      {
        unsigned char* dest   = image->data(0,t,0 );
        const unsigned char* source = _image->data(0,t,0);

        for( int i = 0; i < _image->s(); ++i )
        {
          *dest++ = *source;
          *dest++ = *source;
          *dest++ = *source++;
        }
      }
      osgDB::writeImageFile( *image, name );
    }
    break;
  case GL_LUMINANCE_ALPHA:
    {
      osg::ref_ptr< osg::Image > image ( new osg::Image );
      image->allocateImage( _image->s(), _image->t(), 1, GL_RGBA, _image->getDataType() );

      for( int t = 0; t < _image->t(); ++t )
      {
        unsigned char* dest   = image->data(0,t,0 );
        const unsigned char* source = _image->data(0,t,0);

        for( int i = 0; i < _image->s(); ++i )
        {
          *dest++ = *source;
          *dest++ = *source;
          *dest++ = *source++;
          *dest++ = *source++;
        }
      }
      osgDB::writeImageFile( *image, name );
    }
    break;
  default:
    osgDB::writeImageFile( *_image, name );
  }

  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ImageDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ImageDocument::Filters ImageDocument::filtersExport() const
{
  Filters filters;
  filters.push_back ( Filter ( "All Images (*.jpeg *.jpg *.tiff *.tif *.png *.gif *.bmp)", "*.jpeg;*.jpg;*.tiff;*.tif;*.png;*.gif;*.bmp" ) );
  filters.push_back ( Filter ( "JPEG (*.jpeg *.jpg)", "*.jpeg;*.jpg" ) );
  filters.push_back ( Filter ( "TIFF (*.tiff *.tif)", "*.tiff;*.tif" ) );
  filters.push_back ( Filter ( "PNG  (*.png)",        "*.png"        ) );
  filters.push_back ( Filter ( "GIF  (*.gif)",        "*.gif"        ) );
  filters.push_back ( Filter ( "BMP  (*.bmp)",        "*.bmp"        ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

ImageDocument::Filters ImageDocument::filtersInsert() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

ImageDocument::Filters ImageDocument::filtersOpen() const
{
  return this->filtersExport();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ImageDocument::Filters ImageDocument::filtersSave() const
{
  return this->filtersExport();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert image to Fox image
//
///////////////////////////////////////////////////////////////////////////////

FX::FXImage* ImageDocument::getFoxImage()
{
  FX::FXImage *image ( new FX::FXImage ( FoxTools::Functions::application(), 0x0, FX::IMAGE_OWNED, _image->t(), _image->s() ) );

  switch ( _image->getPixelFormat() )
  {
  case GL_LUMINANCE:
    {
      for( int t = 0; t < _image->t(); ++t )
      {
        unsigned char* source = _image->data(0,t,0);

        for( int i = 0; i < _image->s(); ++i )
        {
          image->setPixel ( i, _image->t() - t - 1, FXRGB ( *source, *source, *source++ ) );
        }
      }
    }
    break;
  case GL_LUMINANCE_ALPHA:
    {
      for( int t = 0; t < _image->t(); ++t )
      {
        unsigned char* source = _image->data(0,t,0);

        for( int i = 0; i < _image->s(); ++i )
        {
          image->setPixel ( i, _image->t() - t - 1, FXRGBA ( *source, *source, *source++, *source++ ) );
        }
      }
    }
    break;
  case GL_RGB:
    {
      for( int t = 0; t < _image->t(); ++t )
      {
        unsigned char* source = _image->data(0,t,0);

        for( int i = 0; i < _image->s(); ++i )
        {
          image->setPixel ( i, _image->t() - t - 1, FXRGB ( *source++, *source++, *source++ ) );
        }
      }
    }
    break;
  case GL_RGBA:
    {
      for( int t = 0; t < _image->t(); ++t )
      {
        unsigned char* source = _image->data(0,t,0);

        for( int i = 0; i < _image->s(); ++i )
        {
          image->setPixel ( t, i, FXRGB ( *source++, *source++, *source++  ) );

          source++;
        }
      }
    }
    break;
  }

  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify this document of the message.
//
///////////////////////////////////////////////////////////////////////////////

void ImageDocument::notify ( unsigned short message )
{
  BaseClass::notify ( message );
}
