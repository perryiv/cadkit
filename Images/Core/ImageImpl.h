
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

// Disable deprecated warning in Visual Studio 8 for std::basic_istream<_Elem,_Traits>::read
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "Images/Core/BaseImage.h"
#include "Images/Core/Iterator.h"
#include "Images/Core/TypeTraits.h"
#include "Images/Algorithms/Grayscale.h"
#include "Images/Algorithms/RedGreenBlue.h"
#include "Images/Algorithms/Alpha.h"

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
  typedef ImageImpl<ValueType> ThisType;
  typedef std::vector<ValueType> Channel;
  typedef std::vector<Channel> Channels;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef typename Images::TypeTraits<ValueType>::IGetImageData IGetImageData;
  typedef typename IGetImageData::Values ImageDataValues;
  typedef Images::Iterator<ThisType> Iterator;
  typedef const Iterator ConstIterator;


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
    _channels()
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
    _channels ( image._channels )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  ImageImpl &operator = ( const ImageImpl &image )
  {
    _channels = image._channels;
    BaseClass::operator = ( image );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the channel.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Channel &channel ( unsigned int which ) const
  {
    return _channels.at ( which );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the channel.
  //
  /////////////////////////////////////////////////////////////////////////////

  Channel &channel ( unsigned int which )
  {
    return _channels.at ( which );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the number of channels.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual unsigned int channels() const
  {
    return _channels.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the number of channels.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void channels ( unsigned int c )
  {
    return _channels.resize ( c );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the number of rows.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual unsigned int rows() const
  {
    return BaseClass::rows();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the number of rows.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void rows ( unsigned int r )
  {
    USUL_ASSERT ( 0 ); // TODO
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the number of columns.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual unsigned int columns() const
  {
    return BaseClass::columns();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the number of columns.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void columns ( unsigned int c )
  {
    USUL_ASSERT ( 0 ); // TODO
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the number of layers.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual unsigned int layers() const
  {
    return BaseClass::layers();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the number of layers.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void layers ( unsigned int l )
  {
    USUL_ASSERT ( 0 ); // TODO
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return iterator to beginning.
  //
  /////////////////////////////////////////////////////////////////////////////

  Iterator begin()
  {
    return Iterator ( *this, false );
  }
  ConstIterator begin() const
  {
    return Iterator ( const_cast < ThisType & > ( *this ), false );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return iterator to end.
  //
  /////////////////////////////////////////////////////////////////////////////

  Iterator end()
  {
    return Iterator ( *this, true );
  }
  ConstIterator end() const
  {
    return Iterator ( const_cast < ThisType & > ( *this ), true );
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
    ImageDataValues v;
    unsigned int rows ( 0 ), columns ( 0 ), layers ( 0 ), channels ( 0 );
    data->getImageValues ( v );
    data->getImageDimensions ( rows, columns, layers, channels );

    // Check data.
    if ( v.size() != rows * columns * layers * channels )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 1598924701: there are ", v.size(), " values in the image array, but ",
          "rows = ", rows, ", columns = ", columns, ", layers = ", layers, ", channels = ", channels,
          ", which means there should be ", rows * columns * layers * channels, " values" );
    }

    // Set our data.
    this->resize ( rows, columns, layers, channels );
    std::copy ( v.begin(), v.end(), this->begin() );
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
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the histogram.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void histogram ( unsigned int whichChannel, OneChannelHistogram &h ) const
  {
    // Get the channel.
    const Channel &channel ( _channels.at ( whichChannel ) );

    // Loop through the values.
    for ( typename Channel::const_iterator i = channel.begin(); i != channel.end(); ++i )
    {
      // Get the value.
      const ValueType &value ( *i );

      // Get reference to the count in the map.
      unsigned int &count = h[value];

      // Increment this value in the map.
      ++count;
    }
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

    // Loop through the channels.
    for ( unsigned int i = 0; i < _channels.size(); ++i )
    {
      // Calculate histogram on this channel.
      this->histogram ( i, h.at(i) );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to grayscale.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void grayScale()
  {
    Images::Algorithms::grayScale ( this->alpha(), _channels );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to rgba.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void redGreenBlue()
  {
    Images::Algorithms::redGreenBlue ( this->alpha(), _channels );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Resize the image.
  //
  /////////////////////////////////////////////////////////////////////////////

  void resize ( unsigned int rows, 
                unsigned int columns, 
                unsigned int layers, 
                unsigned int channels )
  {
    // Set the number of channels.
    _channels.resize ( channels );

    // The number of values.
    const unsigned int size ( rows * columns * layers );

    // Loop through the channels and set their sizes.
    for ( typename Channels::iterator i = _channels.begin(); i != _channels.end(); ++i )
      (*i).resize ( size );

    // Set base class's members.
    this->_set ( rows, columns, layers );
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

  template < class ContainerType > void scalars ( ContainerType &v ) const
  {
    typedef typename ContainerType::iterator Itr;
    typedef typename ContainerType::value_type Type;

    const unsigned int size ( this->size() );
    v.resize ( size );
    ConstIterator from ( this->begin() );

    for ( Itr to = v.begin(); to != v.end(); ++to, ++from )
    {
      (*to) = static_cast < Type > ( *from );
    }
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


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read the raw data.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void read ( std::istream &in )
  {
    Usul::Types::Uint32 rows     ( 0 );
    Usul::Types::Uint32 columns  ( 0 );
    Usul::Types::Uint32 layers   ( 0 );
    Usul::Types::Uint32 channels ( 0 );
    Usul::Types::Uint8  alpha    ( 0 );

    in.read ( reinterpret_cast < char * > ( &rows     ), sizeof ( Usul::Types::Uint32 ) );
    in.read ( reinterpret_cast < char * > ( &columns  ), sizeof ( Usul::Types::Uint32 ) );
    in.read ( reinterpret_cast < char * > ( &layers   ), sizeof ( Usul::Types::Uint32 ) );
    in.read ( reinterpret_cast < char * > ( &channels ), sizeof ( Usul::Types::Uint32 ) );
    in.read ( reinterpret_cast < char * > ( &alpha    ), sizeof ( Usul::Types::Uint8  ) );

    this->resize ( rows, columns, layers, channels );

    for ( unsigned int i = 0; i < _channels.size(); ++i )
    {
      Channel &channel = _channels.at(i);
      const unsigned int size ( channel.size() * this->bytes() );
      in.read ( reinterpret_cast < char * > ( &channel[0] ), size );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Write the dimensions and pixel data to the given stream.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void write ( std::ostream &out ) const
  {
    Usul::Types::Uint32 rows     ( this->rows()     );
    Usul::Types::Uint32 columns  ( this->columns()  );
    Usul::Types::Uint32 layers   ( this->layers()   );
    Usul::Types::Uint32 channels ( this->channels() );
    Usul::Types::Uint8  alpha    ( ( this->alpha() ) ? 1 : 0 );

    out.write ( reinterpret_cast < const char * > ( &rows     ), sizeof ( Usul::Types::Uint32 ) );
    out.write ( reinterpret_cast < const char * > ( &columns  ), sizeof ( Usul::Types::Uint32 ) );
    out.write ( reinterpret_cast < const char * > ( &layers   ), sizeof ( Usul::Types::Uint32 ) );
    out.write ( reinterpret_cast < const char * > ( &channels ), sizeof ( Usul::Types::Uint32 ) );
    out.write ( reinterpret_cast < const char * > ( &alpha    ), sizeof ( Usul::Types::Uint8  ) );

    for ( unsigned int i = 0; i < _channels.size(); ++i )
    {
      const Channel &channel = _channels.at(i);
      const unsigned int size ( channel.size() * this->bytes() );
      out.write ( reinterpret_cast < const char * > ( &channel[0] ), size );
    }
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

  Channels _channels;
};


} // Images


#endif // _IMAGES_TEMPLATE_IMAGE_IMPLEMENTATION_CLASS_H_
