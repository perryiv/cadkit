
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Tri-stripper algorithm.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_TRI_STRIPPER_H_
#define _USUL_ALGORITHMS_TRI_STRIPPER_H_

#include <stdexcept>


namespace Usul {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Assumes every three vertices are a triangle. Makes one or more tri-strips.
//
///////////////////////////////////////////////////////////////////////////////

template < class Vertices, class Indices > 
void triStrip ( const Vertices &vertices, Indices &indices, Indices &strips )
{
  typedef typename Vertices::value_type Vertex;
  typedef typename Vertices::iterator VI;
  typedef typename Indices::iterator II;

  // Handle trivial case.
  if ( vertices.empty() )
    return;

  // Make sure the vertex data is valid.
  if ( vertices.size() % 3 )
    throw std::runtime_error ( "Error 2366236208, number of vertices is not divisible by three" );

  // Make a copy of the original vertices.
  std::list<Vertex> original ( vertices.begin(), vertices.end() );

  // Get original sizes. This handles repeated calls with the same containers.
  unsigned int numVerts   ( vertices.size() );
  unsigned int numIndices ( indices.size()  );
  unsigned int numStrips  ( strips.size()   );

  // Allocate some memory (for speed).
  indices.reserve ( numVerts + numIndices );
  strips.reserve  ( numVerts + numStrips  );

  // Move the first triangle into the strip.
  unsigned int count ( 0 );
  indices.push_back ( original.front() ); original.pop_front();
  indices.push_back ( original.front() ); original.pop_front();
  indices.push_back ( original.front() ); original.pop_front();
  is.at ( count     ) = 0;
  is.at ( count + 1 ) = 3;

  // Loop until we cannot remove any more.
  while ( !ov.empty() )
  {
    // Loop through the original vertices.
    for ( TI i = ov.begin(); i != ov.end(); ++i )
    {

    }
  }
}


}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_TRI_STRIPPER_H_
