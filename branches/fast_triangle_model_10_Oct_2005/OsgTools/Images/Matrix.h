
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

private:
  unsigned int _r;
  unsigned int _c;
  std::vector < T > _array;
};

  }
}

#endif // __OSG_TOOLS_IMAGES_MATRIX_H__
