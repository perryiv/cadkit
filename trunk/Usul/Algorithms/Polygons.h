
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms for polygons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_POLYGONS_H_
#define _USUL_ALGORITHMS_POLYGONS_H_

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <sstream>


namespace Usul {
namespace Algorithms {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Fills sequence "indices" with .
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class AdjacentTest, 
  class VertexSequence,
  class IndexSequence
> 
void findAdjacent ( const AdjacentTest &adjacentTest, 
                    const VertexSequence &vertices, 
                    unsigned int selectedPolygon,
                    unsigned int numVertsPerPoly,
                    IndexSequence &indices )
{
  typedef typename VertexSequence::value_type Vertex;
  typedef typename VertexSequence::size_type SizeType;
  typedef typename VertexSequence::const_iterator VertexIterator;
  typedef typename IndexSequence::iterator IndexIterator;

  // Initialize.
  indices.erase ( indices.begin(), indices.end() );

  // Handle trivial case.
  if ( vertices.empty() || vertices.size() < numVertsPerPoly )
    return;

  // Make sure the vertex data is valid.
  if ( vertices.size() % numVertsPerPoly || 0 == numVertsPerPoly )
  {
    std::ostringstream message;
    message << "Error 1684390442, number of vertices is not divisible by " << numVertsPerPoly;
    throw std::runtime_error ( message.str() );
  }

  // Make sure polygon has enough vertices.
  if ( numVertsPerPoly < 3 )
  {
    std::ostringstream message;
    message << "Error 3953474238, number of vertices per polygon = " << numVertsPerPoly;
    throw std::runtime_error ( message.str() );
  }

  // Fill the sequence of indices.
  SizeType numPolyons ( vertices.size() / numVertsPerPoly );
  indices.resize ( numPolyons );
  {
    SizeType count ( 0 );
    for ( IndexIterator i = indices.begin(); i < indices.end(); ++i )
      *indices = count++;
  }

  // Put the first polygon in the list of keepers.
  IndexSequence keepers;
  keepers.insert ( keepers.end(), selectedPolygon );

  // Initialize the current keeper to test.
  IndexIterator current = keepers.begin();

  // Define some polygons.
  VertexSequence currentPolygon, testThisPolygon;
  currentPolygon.resize  ( numVertsPerPoly );
  testThisPolygon.resize ( numVertsPerPoly );

  // While there are still keepers to test...
  while ( keepers.end() != current )
  {
    // Set polygon for the current keeper.
    {
      wrong!
      IndexIterator last ( current );
      std::advance ( last, numVertsPerPoly );
      std::copy ( current, last, currentPolygon.begin() );
    }

    // Loop through all vertices.
    VertexIterator vi = vertices.begin();
    while (  vertices.end() != vi )
    {
      // Collect vertices for this polygon.
      {
        VertexIterator last ( vi );
        std::advance ( last, numVertsPerPoly );
        std::copy ( vi, last, testThisPolygon.begin() );
      }

      // See if the current keeper is adjacent to this polygon.
      if ( true == adjacentTest ( currentPolygon, testThisPolygon )
      {
        // Put the polygon into the list of keepers.
        keepers.insert ( keepers.end(), 
      }
    }
  }
}


}; // namespace Polygons
}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_POLYGONS_H_
