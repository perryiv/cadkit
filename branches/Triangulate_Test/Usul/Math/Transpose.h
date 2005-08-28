
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The transpose function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_TRANSPOSE_FUNCTION_H_
#define _USUL_MATH_TRANSPOSE_FUNCTION_H_

#include <stdexcept>


namespace Usul {
namespace Math {


/////////////////////////////////////////////////////////////////////////////
//
//  Transpose the given 2D container.
//
/////////////////////////////////////////////////////////////////////////////

template < class Matrix1_, class Matrix2_ > inline void transpose ( const Matrix1_ &m, Matrix2_ &t )
{
  const unsigned int rows    ( m.size() );
  const unsigned int columns ( ( rows ) ? m.at(0).size() : 0 );

  t.resize ( columns );
  for ( unsigned int c = 0; c < columns; ++c )
  {
    t.at(c).resize ( rows );
    for ( unsigned int r = 0; r < rows; ++r )
    {
      if ( columns != m.at(r).size() )
        throw std::runtime_error ( "Error 3987300086: inconsistant sizes in given matrix" );
      t.at(c).at(r) = m.at(r).at(c);
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Transpose the given 2D container.
//
/////////////////////////////////////////////////////////////////////////////

template < class Matrix_ > inline void transpose ( Matrix_ &m )
{
  Matrix_ t;
  Usul::Math::transpose ( m, t );
  m = t;
}


}; // namespace Math
}; // namespace Usul


#endif // _USUL_MATH_TRANSPOSE_FUNCTION_H_
