
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Template image class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_TEMPLATE_IMAGE_IMPLEMENTATION_CLASS_H_
#define _IMAGES_TEMPLATE_IMAGE_IMPLEMENTATION_CLASS_H_

#include "Images/Core/BaseImage.h"
#include "Images/Core/TypeTraits.h"
#include "Images/Algorithms/Grayscale.h"

#include "Usul/Types/Types.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IGetImageData.h"

#include <vector>
#include <algorithm>
#include <stdexcept>


namespace Images {


template < class ValueType_ > class ImageImpl : public BaseImage
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef BaseImage BaseClass;
  typedef ValueType_ ValueType;
  typedef std::vector<ValueType> Values;
  typedef typename Values::iterator Itr;
  typedef typename Values::const_iterator ConstItr;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Components::Manager PluginManager;
  typedef typename Images::TypeTraits<ValueType>::IGetImageData IGetImageData;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Smart-pointer definitions.
  //
  /////////////////////////////////////////////////////////////////////////////

  USUL_DECLARE_REF_POINTERS ( ImageImpl );


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ImageImpl ( Unknown *pixels = 0x0 ) : BaseClass(),
    _values()
  {
    // Set the pixels.
    this->setPixels ( pixels );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ImageImpl ( const ImageImpl &image ) : BaseClass ( image ),
    _values ( image._values )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  ImageImpl &operator = ( const ImageImpl &image )
  {
    _values = image._values;
    BaseClass::operator = ( image );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set this image's pixels.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void setPixels ( Unknown *pixels )
  {
    // See if there are pixels.
    IGetImageData::QueryPtr data ( pixels );
    if ( !data.valid() )
      return;

    // Get the pixels.
    typedef typename IGetImageData::Values ImageDataValues;
    ImageDataValues v;
    unsigned int w ( 0 ), h ( 0 ), c ( 0 );
    data->getImageValues ( v );
    data->getImageDimensions ( w, h, c );

    // Check data.
    if ( v.size() != w * h * c )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 1598924701: there are ", v.size(), " values in the image array, but ",
          "width = ", w, ", height = ", h, ", and number of channels = ", c,
          ", which means there should be ", w * h * c, " values" );
    }

    // Set our data.
    this->resize ( w, h, c );
    std::copy ( v.begin(), v.end(), _values.begin() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a clone of this image.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual BaseClass *clone() const
  {
    return new ImageImpl ( *this );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the histogram.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void histogram ( Histogram &h ) const
  {
    USUL_ASSERT ( 0 ); // TODO
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to grayscale.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void toGrayScale()
  {
    Itr end ( Images::Algorithms::toGrayScale ( _values, this->channels(), this->alpha() ) );
    _values.erase ( end, _values.end() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read the file.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void read ( const std::string &name )
  {
    typedef Usul::Interfaces::IRead IRead;
    typedef Usul::Interfaces::IReadImage IReadImage;
    typedef Usul::Interfaces::IGetImageProperties IGetImageProperties;

    // Default TIFF reader truncates 16-bit pixels, so we use our own reader.
    IReadTIFF::QueryPtr tiff ( PluginManager::instance().getInterface ( IReadTIFF::IID ) );

    // Get other interfaces.
    IRead::QueryPtr reader ( tiff );
    IGetImageProperties::QueryPtr props ( tiff );

    // See if we have the interfaces we need...
    if ( false == reader.valid() || false == props.valid() )
      return false;

    // Read the image.
    reader->read ( name );

    // See what kind it is.
    unsigned int bytes ( props->getNumBytesPerValue() );
    bool floating ( props->isValueFloatingPoint() );

    // Make image of proper kind.
    Images::BaseImage::ValidRefPtr image ( Images::Factory::create ( bytes, floating, reader ) );

    // Set our image.
    _image = image;

    // It worked.
    return true;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Resize the image.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void resize ( unsigned int w, unsigned int h, unsigned int c )
  {
    _values.resize ( w * h * c );
    BaseClass::resize ( w, h, c );
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Use reference counting.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual ~ImageImpl()
  {
  }

private:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  Values _values;
};


} // Images


#endif // _IMAGES_TEMPLATE_IMAGE_IMPLEMENTATION_CLASS_H_
