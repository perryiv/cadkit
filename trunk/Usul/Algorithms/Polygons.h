
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

  namespace Detail 
  {
    template < class Iterator, class VertexSequence > 
    void makePolygon ( Iterator &iter, unsigned int numVertsPerPoly, const VertexSequence &vertices, VertexSequence &polygon )
    {
      for ( unsigned int i = 0; i < numVertsPerPoly; ++i)
      {
        polygon.push_back( *iter );
        ++iter
      }
    }

    template < class VertexSequence > 
    void makePolygon ( unsigned int startIndex, unsigned int numVertsPerPoly, const VertexSequence &vertices, VertexSequence &polygon )
    {
      for ( unsigned int i = startIndex; i < startIndex + numVertsPerPoly; ++i)
      {
        polygon.push_back( vertices.at( i ) );
      }
    }
  }; //namespace Detail

namespace Polygons {

  template < class VertexSequence >
  struct TriangleText
  {
    bool operator() ( const VertexSequence &polygonOne, const VertexSequence &polygonTwo ) const
    {
      typedef VertexSequence::const_iterator Iterator;
      bool foundAdjacent = false;
      for( Iterator i = polygonOne.begin(); i != polygonOne.end(); ++i )
      {
        for ( Iterator j = polygonTwo.begin(); j != polygonTwo.end(); ++j )
        {
          if( *i == *j )
          {
            if( foundAdjacent )
              return true;
            else
              foundAdjacent = true;
          }
        }
      }
      return false;
    }
  };

///////////////////////////////////////////////////////////////////////////////
//
//  Input:
//
//     adjacentTest:  Predicate that determines if two polygons are adjacent.
//         vertices:  Sequence of vertices that define polygons.
//  selectedPolygon:  Initial polygon to start checking against.
//  numVertsPerPoly:  Number of vertices in a polygon. Triangle = 3.
//
//  Output:
//
//          indices:  List of indices into "vertices". These vertices make 
//                    up polygons that are adjacent, either directly or 
//                    recursively, to the initial polygon "selectedPolygon".
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
  if ( vertices.empty() )
    return;

  // Make sure polygon has enough vertices.
  if ( vertices.size() < numVertsPerPoly )
  {
    std::ostringstream message;
    message << "Error 2591175031, " << vertices.size() << " vertices given when the polygons are supposed to have " << numVertsPerPoly << " vertices";
    throw std::runtime_error ( message.str() );
  }

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

  // Make sure selected polygon is in range.
  SizeType numPolyons ( vertices.size() / numVertsPerPoly );
  if ( selectedPolygon >= numPolyons )
  {
    std::ostringstream message;
    message << "Error 3575595664, selected polygon is " << selectedPolygon << " but there are only " << numPolyons << " polygons";
    throw std::runtime_error ( message.str() );
  }

  //IndexSequence orginals ( vertices.size() / numVertsPerPoly );
  //for(unsigned int i = 0; i < vertices.size() / numVertsPerPoly; ++i)
  //  orginals.push_back( i );

  // Fill the sequence of indices.
  indices.resize ( vertices.size() / numVertsPerPoly );
  {
    SizeType count ( 0 );
    for ( IndexIterator i = indices.begin(); i != indices.end(); ++i )
      *i = count++;
  }

  // Put the first polygon in the list of keepers.
  IndexSequence keepers;
  keepers.insert ( keepers.end(), selectedPolygon );

  // Initialize the current keeper.
  IndexIterator currentItr = keepers.begin();

  // While there are still keepers to test...
  while ( keepers.end() != currentItr )
  {
    // Define current polygon.
    VertexSequence currentPolygon;
    Detail::makePolygon ( *currentItr * numVertsPerPoly, numVertsPerPoly, vertices, currentPolygon );

    // Loop through all vertices.
    //VertexIterator vi = vertices.begin();
    IndexIterator current = indices.begin();
    while ( indices.end() != current )
    {
      // Define polygon to test against current.
      VertexSequence testMe;
      Detail::makePolygon ( *current * numVertsPerPoly, numVertsPerPoly, vertices, testMe );

      // See if the current keeper is adjacent to this polygon.
      if ( true == adjacentTest ( currentPolygon, testMe ) )
      {
        // Put the polygon into the list of keepers.
        keepers.push_back ( *current );
        indices.erase ( current );
      }

      ++current;
    }

    // Go to the next keeper.
    ++currentItr;
  }
}


}; // namespace Polygons
}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_POLYGONS_H_
