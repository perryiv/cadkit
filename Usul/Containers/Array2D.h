
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Simple 2D array class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_CONTAINERS_ARRAY_TWO_D_H__
#define __USUL_CONTAINERS_ARRAY_TWO_D_H__

#include <vector>

namespace Usul {
  namespace Containers {

template < class T >
class Array2D
{
public:
  typedef Array2D < T >                   ThisType;
  typedef std::vector < T >               Array;

  Array2D ( unsigned int r, unsigned int c ) :
    _r ( r ),
    _c ( c ),
    _array ( r * c )
  {
  }

  T& at ( unsigned int r, unsigned int c )
  {
    return _array.at( this->_index( r, c) );
  }

  const T& at ( unsigned int r, unsigned int c ) const
  {
    return _array.at( this->_index( r, c) );
  }

  T& operator () ( unsigned int r, unsigned int c )
  {
    return _array.at( this->_index( r, c) );
  }

  const T& operator () ( unsigned int r, unsigned int c ) const
  {
    return _array.at( this->_index( r, c) );
  }

  unsigned int rows () const { return _r; }

  unsigned int columns () const { return _c; }

private:
  typename Array::size_type _index( unsigned int r, unsigned int c ) const
  {
    return ( ( r * _c ) + c );
  }
  
  unsigned int _r;
  unsigned int _c;
  std::vector < T > _array;
};

  }
}

#endif // __USUL_CONTAINERS_ARRAY_TWO_D_H__

