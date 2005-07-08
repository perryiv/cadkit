
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Image class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Images/Core/Image.h"
#include "Images/Core/ImageImpl.h"
#include "Images/Core/Factory.h"

#include "Usul/Strings/Case.h"
#include "Usul/File/Path.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/IReadTIFF.h"
#include "Usul/Interfaces/IReadImage.h"
#include "Usul/Interfaces/IGetImageProperties.h"

using namespace Images;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Image, Image::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Components::Manager PluginManager;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Image::Image ( unsigned int bytes, bool floating ) : BaseClass(),
  _image ( Images::Factory::create ( bytes, floating ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Image::Image ( const Image &image ) : BaseClass ( image ),
  _image ( image._image->clone() )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Image::~Image()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Image::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IGetImageProperties::IID:
    return static_cast < Usul::Interfaces::IGetImageProperties*>(this);
  case Usul::Interfaces::IGetImageDataUint8::IID:
    return static_cast < Usul::Interfaces::IGetImageDataUint8*>(this);
  case Usul::Interfaces::IGetImageDataUint16::IID:
    return static_cast < Usul::Interfaces::IGetImageDataUint16*>(this);
  case Usul::Interfaces::IGetImageDataUint32::IID:
    return static_cast < Usul::Interfaces::IGetImageDataUint32*>(this);
  case Usul::Interfaces::IGetImageDataFloat32::IID:
    return static_cast < Usul::Interfaces::IGetImageDataFloat32*>(this);
  case Usul::Interfaces::IGetImageDataFloat64::IID:
    return static_cast < Usul::Interfaces::IGetImageDataFloat64*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the width.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Image::width() const
{
  return _image->width();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the height.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Image::height() const
{
  return _image->height();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of channels.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Image::channels() const
{
  return _image->channels();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for reading an image.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class InterfaceType_ > struct Reader
  {
    static ::Images::BaseImage *read ( const std::string &name )
    {
      typedef Usul::Interfaces::IRead IRead;
      typedef InterfaceType_ InterfaceType;
      typedef Usul::Interfaces::IGetImageProperties IGetImageProperties;

      // Default TIFF reader truncates 16-bit pixels, so we use our own reader.
      InterfaceType::QueryPtr tiff ( PluginManager::instance().getInterface ( InterfaceType::IID ) );

      // Get other interfaces.
      IRead::QueryPtr reader ( tiff );
      IGetImageProperties::QueryPtr props ( tiff );

      // See if we have the interfaces we need...
      if ( false == reader.valid() || false == props.valid() )
        return 0x0;

      // Read the image.
      reader->read ( name );

      // See what kind it is.
      unsigned int bytes ( props->getNumBytesPerValue() );
      bool integer ( props->isValueInteger() );

      // Make image of proper kind.
      return Images::Factory::create ( bytes, integer, reader );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the image.
//
///////////////////////////////////////////////////////////////////////////////

void Image::read ( const std::string &name )
{
  typedef Usul::Interfaces::IRead IRead;
  typedef Usul::Interfaces::IReadTIFF IReadTIFF;
  typedef Usul::Interfaces::IReadImage IReadImage;
  typedef Usul::Interfaces::IGetImageProperties IGetImageProperties;

  // Get extension.
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  // Declare here.
  Images::BaseImage::RefPtr image ( 0x0 );

  // Do we have a tiff image?
  if ( ext == "tif" || ext == "tiff" )
    image = Detail::Reader<IReadTIFF>::read ( name );

  // Assign our image. Use default reader is needed.
  _image = ( image.valid() ) ? image : Detail::Reader<IReadImage>::read ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to grayscale.
//
///////////////////////////////////////////////////////////////////////////////

void Image::toGrayScale()
{
  _image->toGrayScale();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the histogram.
//
///////////////////////////////////////////////////////////////////////////////

void Image::histogram ( Histogram &h ) const
{
  _image->histogram ( h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get extreme values in all the channels.
//
///////////////////////////////////////////////////////////////////////////////

void Image::extremes ( ValueCount &low, ValueCount &high ) const
{
  _image->extremes ( low, high );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of bytes per channel-value.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Image::bytes() const
{
  return this->type().first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get whether or not the pixel values are floating point.
//
///////////////////////////////////////////////////////////////////////////////

bool Image::floating() const
{
  return !this->integer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set whether or not the pixel values are integer.
//
///////////////////////////////////////////////////////////////////////////////

void Image::integer ( bool state )
{
  this->type ( TypeInfo ( this->bytes(), state ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get whether or not the pixel values are integer.
//
///////////////////////////////////////////////////////////////////////////////

bool Image::integer() const
{
  return _image->integer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the type of image.
//
///////////////////////////////////////////////////////////////////////////////

void Image::type ( unsigned int bytes, bool integer )
{
  this->type ( TypeInfo ( bytes, integer ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the type of image.
//
///////////////////////////////////////////////////////////////////////////////

void Image::type ( const TypeInfo &info )
{
  if ( info != this->type() )
  {
    // Declare a new image. Pass in existing image as the pixel source.
    Usul::Interfaces::IUnknown::ValidQueryPtr unknown ( this );
    Images::BaseImage::ValidRefPtr image ( Images::Factory::create ( info.first, info.second, unknown ) );

    // Assign to our image smart-pointer.
    _image = image;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the type of image.
//
///////////////////////////////////////////////////////////////////////////////

Image::TypeInfo Image::type() const
{
  return TypeInfo ( _image->bytes(), _image->integer() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of bytes per value.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Image::getNumBytesPerValue() const
{
  return this->bytes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the scalar values are integers.
//
///////////////////////////////////////////////////////////////////////////////

bool Image::isValueInteger() const
{
  return this->integer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image values.
//
///////////////////////////////////////////////////////////////////////////////

void Image::getImageValues ( DataUint8 &v ) const
{
  _image->values ( v );
}
void Image::getImageValues ( DataUint16 &v ) const
{
  _image->values ( v );
}
void Image::getImageValues ( DataUint32 &v ) const
{
  _image->values ( v );
}
void Image::getImageValues ( DataFloat32 &v ) const
{
  _image->values ( v );
}
void Image::getImageValues ( DataFloat64 &v ) const
{
  _image->values ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image dimensions.
//
///////////////////////////////////////////////////////////////////////////////

void Image::getImageDimensions ( unsigned int &w, unsigned int &h, unsigned int &c ) const
{
  w = this->width();
  h = this->height();
  c = this->channels();
}
