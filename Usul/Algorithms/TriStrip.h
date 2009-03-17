
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Tri-stripper algorithm.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_TRI_STRIP_H_
#define _USUL_ALGORITHMS_TRI_STRIP_H_

#include <stdexcept>


namespace Usul {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Generates indices for rows of tri-strips given a mesh size.
//  Use when you want to draw a grid of vertices.
//
///////////////////////////////////////////////////////////////////////////////

template < class SizeType, class TriStripIndices >
inline void triStripIndices ( SizeType rows, SizeType columns, TriStripIndices &allStrips )
{
  typedef typename TriStripIndices::value_type SingleStripIndices;

  if ( ( rows < 2 ) || ( columns < 2 ) )
  {
    throw std::invalid_argument ( "Error 1844606493: Minimum mesh size for tri-strip generation is 2x2" );
  }

  // Initialize the answer.
  allStrips.clear();
  allStrips.resize ( rows - 1 );

  // Loop through the rows.
  for ( SizeType i = 0; i < allStrips.size(); ++i )
  {
    SingleStripIndices &currentStrip ( allStrips.at ( i ) );
    currentStrip.clear();
    currentStrip.reserve ( 2 * columns );

    // Loop through the columns of this row.
    for ( SizeType j = 0; j < columns; ++j )
    {
      currentStrip.push_back ( ( ( i + 1 ) * columns ) + j );
      currentStrip.push_back ( ( ( i     ) * columns ) + j );
    }
  }
}


} // namespace Algorithms
} // namespace Usul


#endif // _USUL_ALGORITHMS_TRI_STRIP_H_
