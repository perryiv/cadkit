
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
#include "Usul/MPL/SameType.h"

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <limits>


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
  //  Get extreme values in all the channels.
  //
  /////////////////////////////////////////////////////////////////////////////

  void extremes ( ValueCount &low, ValueCount &high ) const
  {
    USUL_ASSERT ( 0 ); // Heads up...
#if 0
    // The login of this function relies on the histogram being sorted.
    typedef std::map < double, unsigned int > OneChannelHistogramType;
    typedef std::vector < unsigned int, OneChannelHistogram > HistogramType;
    USUL_ASSERT_SAME_TYPE ( Histogram, HistogramType );

    // Must be at least two values.
    if ( _values.size() < 2 )
      return;

    // Get the histogram.
    Histogram h;
    this->histogram ( h );

    // Iterators to first and last.
    Histogram::iterator a ( h.begin() );
    Histogram::iterator b ( h.end()   );

    // For each channel...
    for ( unsigned int i = 0; i < this->channels(); ++i )
    {
      // Find the first non-zero value.
      low = *a;
      if ( 0 == low.first )
      {
        ++a;
        low = *a;
      }

      // The last values should be the high.
      --b;
      high = *b;
    }
#endif
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the histogram.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void histogram ( Histogram &h ) const
  {
    // Make sure there is room for every channel.
    h.resize ( this->channels() );

    // Loop through the values.
    for ( unsigned int i = 0; i < _values.size(); ++i )
    {
      // Get the value.
      const ValueType value ( _values[i] );

      // Get the channel.
      const unsigned int channel ( i % this->channels() );

      // Get reference to the count in the map.
      unsigned int &count = h.at(channel)[value];

      // Increment this value in the map.
      ++count;
    }
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
  //  Resize the image.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void resize ( unsigned int w, unsigned int h, unsigned int c )
  {
    _values.resize ( w * h * c );
    BaseClass::resize ( w, h, c );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of bytes per scalar value.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int bytes() const
  {
    return sizeof ( ValueType );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Are the scalar values integers?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool integer() const
  {
    return std::numeric_limits<ValueType>::is_integer;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the scalar values. Need to cast to eliminate warnings.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class ContainerType > scalars ( ContainerType &v ) const
  {
    typedef typename ContainerType::value_type ContainerValueType;
    const unsigned int num ( _values.size() );
    v.resize ( num );
    for ( unsigned int i = 0; i < num; ++i )
      v[i] = static_cast < ContainerValueType > ( _values[i] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the image values.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void values ( DataUint8 &v ) const
  {
    this->scalars ( v );
  }
  virtual void values ( DataUint16 &v ) const
  {
    this->scalars ( v );
  }
  virtual void values ( DataUint32 &v ) const
  {
    this->scalars ( v );
  }
  virtual void values ( DataFloat32 &v ) const
  {
    this->scalars ( v );
  }
  virtual void values ( DataFloat64 &v ) const
  {
    this->scalars ( v );
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
