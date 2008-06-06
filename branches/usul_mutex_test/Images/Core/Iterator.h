
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Image iterator class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_TEMPLATE_IMAGE_ITERATOR_CLASS_H_
#define _IMAGES_TEMPLATE_IMAGE_ITERATOR_CLASS_H_

#include <stdexcept>
#include <iterator>
#include <vector>


namespace Images {


template < class ImageType_ > class Iterator
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ImageType_ ImageType;
  typedef typename ImageType::ValueType ValueType;
  typedef typename ImageType::Channel Channel;
  typedef std::vector<unsigned int> Indices;
  typedef typename Indices::iterator::iterator_category iterator_category;
  typedef typename Indices::value_type value_type;
  typedef typename Indices::difference_type difference_type;
  typedef typename Indices::pointer pointer;
  typedef typename Indices::reference reference;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Iterator ( ImageType &image, bool end ) : 
    _image       ( image ), 
    _channel     ( 0 ),
    _numChannels ( image.channels() ),
    _indices     ( image.channels() )
  {
    for ( unsigned int i = 0; i < _numChannels; ++i )
      _indices.at(i) = ( end ) ? _image.channel(i).size() : 0;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Iterator ( const Iterator &itr ) : 
    _image       ( itr._image   ), 
    _channel     ( itr._channel ),
    _numChannels ( itr._numChannels ),
    _indices     ( itr._indices )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Iterator()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment
  //
  /////////////////////////////////////////////////////////////////////////////

  Iterator &operator = ( const Iterator &itr )
  {
    _image       = itr._image;
    _channel     = itr._channel;
    _numChannels = itr._numChannels;
    _indices     = itr._indices;

    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Equality
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class IteratorType_ > bool equals ( const IteratorType_ &itr ) const
  {
    return ( _image   == itr._image   &&
             _channel == itr._channel &&
             _indices == itr._indices );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Increment
  //
  /////////////////////////////////////////////////////////////////////////////

  const Iterator &operator ++ () const
  {
    this->_increment();
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Increment
  //
  /////////////////////////////////////////////////////////////////////////////

  Iterator &operator ++ ()
  {
    this->_increment();
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return current value.
  //
  /////////////////////////////////////////////////////////////////////////////

  const ValueType & operator * () const
  {
    const unsigned int index ( _indices.at ( _channel ) );
    const Channel &channel = _image.channel ( _channel );
		return channel.at ( index );
  }
  ValueType & operator * ()
  {
    const unsigned int index ( _indices.at ( _channel ) );
    Channel &channel = _image.channel ( _channel );
		return channel.at ( index );
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  No default construction.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Iterator();


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Increment
  //
  /////////////////////////////////////////////////////////////////////////////

  void _increment() const
  {
    // Get the appropriate index and channel.
    unsigned int &index ( _indices.at ( _channel ) );
    const Channel &channel = _image.channel ( _channel );

    // Increment if we can. Allow it to go to the end.
    const unsigned int size ( channel.size() );
    if ( index < size )
      ++index;

    // Otherwise, we are at the end.
    else
      throw std::runtime_error ( "Error 3905738367: Incrementing image iterator that is already at the end" );

    // Increment the channel.
    _channel = ( _channel + 1 < _numChannels ) ? _channel + 1 : 0;
  }

private:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  ImageType &_image;
  mutable unsigned int _channel;
  unsigned int _numChannels;
  mutable Indices _indices;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Equality
//
/////////////////////////////////////////////////////////////////////////////

template < class ImageType_ > inline bool operator == ( const Images::Iterator < ImageType_ > &left, const Images::Iterator < ImageType_ > &right )
{
  return left.equals ( right );
}
template < class ImageType_ > inline bool operator != ( const Images::Iterator < ImageType_ > &left, const Images::Iterator < ImageType_ > &right )
{
  return !left.equals ( right );
}


} // Images


#endif // _IMAGES_TEMPLATE_IMAGE_ITERATOR_CLASS_H_
