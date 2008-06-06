
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IMAGES_MATRIX_H__
#define __OSG_TOOLS_IMAGES_MATRIX_H__

#include <vector>

namespace OsgTools {
  namespace Images {

template < class T >
class Matrix
{
public:
  typedef Matrix < T >                    ThisType;
  typedef std::vector < T >               Array;
  typedef typename Array::iterator        Iterator;
  typedef typename Array::const_iterator  ConstIterator;

  Matrix ( unsigned int r, unsigned int c ) :
    _r ( r ),
    _c ( c ),
    _array ( r * c )
  {
  }

  T& at ( unsigned int r, unsigned int c )
  {
    return _array.at( r * _r + c );
  }

  const T& at ( unsigned int r, unsigned int c ) const
  {
    return _array.at( r * _r + c );
  }

  T& operator () ( unsigned int r, unsigned int c )
  {
    return _array.at( r * _r + c );
  }

  const T& operator () ( unsigned int r, unsigned int c ) const
  {
    return _array.at( r * _r + c );
  }

  unsigned int rows () const { return _r; }

  unsigned int columns () const { return _c; }

  // Make the input the transpose of this matrix.
  void         transpose ( ThisType& in )
  {
    const typename Array::size_type rows    ( this->rows () );
    const typename Array::size_type columns ( in.columns () );

    for ( unsigned int c = 0; c < columns; ++c )
    {
      for ( unsigned int r = 0; r < rows; ++r )
      {
        in( c, r ) = (*this) ( r, c );
      }
    }
  }

  Iterator begin () { return _array.begin(); }
  Iterator end ()   { return _array.end(); }

  ConstIterator begin () const { return _array.begin(); }
  ConstIterator end ()   const { return _array.end(); }

private:
  unsigned int _r;
  unsigned int _c;
  std::vector < T > _array;
};

  }
}

#endif // __OSG_TOOLS_IMAGES_MATRIX_H__

