
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
#include <vector>
#include <list>


namespace Usul {
namespace Algorithms {
namespace Polygons {

  template < class VertexSequence >
  struct TriangleTest
  {
    bool operator() ( const VertexSequence &vertices, unsigned int p1, unsigned int p2 ) const
    {
      typedef typename VertexSequence::value_type Vertex;

      unsigned int count ( 0 );

      const Vertex &polyOneP1 = vertices.at( p1 );
      const Vertex &polyOneP2 = vertices.at( p1 + 1 );
      const Vertex &polyOneP3 = vertices.at( p1 + 2 );

      const Vertex &polyTwoP1 = vertices.at( p2 );
      const Vertex &polyTwoP2 = vertices.at( p2 + 1 );
      const Vertex &polyTwoP3 = vertices.at( p2 + 2 );

      if( polyOneP1 != polyTwoP1 && polyOneP1 != polyTwoP2 && polyOneP1 != polyTwoP3 )
        ++count;
      if( polyOneP2 != polyTwoP1 && polyOneP2 != polyTwoP2 && polyOneP2 != polyTwoP3 )
        ++count;
      if( count == 2 )
        return false;
      if( count == 0 )
        return true;
      if( polyOneP3 != polyTwoP1 && polyOneP3 != polyTwoP2 && polyOneP3 != polyTwoP3 )
        ++count;
      if( count >= 2 )
        return false;
      return true;
    }
  };

 template < class VertexSequence >
  struct TriangleSorter
  {
    typedef typename VertexSequence::value_type Vertex;
    typedef typename Vertex::value_type RealType;
    TriangleSorter ( const VertexSequence& vertices ) : _vertices ( vertices ) { }
    bool operator() ( unsigned int p1, unsigned int p2 )
    {
      const RealType oneThird ( 1.0f / 3.0f );
      p1 *= 3;
      p2 *= 3;
      const Vertex &polyOneP1 = _vertices.at( p1 );
      const Vertex &polyOneP2 = _vertices.at( p1 + 1 );
      const Vertex &polyOneP3 = _vertices.at( p1 + 2 );

      Vertex polyOneCenter;
      polyOneCenter[0] = ( polyOneP1[0] + polyOneP2[0] + polyOneP3[0] ) * oneThird;
      polyOneCenter[1] = ( polyOneP1[1] + polyOneP2[1] + polyOneP3[1] ) * oneThird;
      polyOneCenter[2] = ( polyOneP1[2] + polyOneP2[2] + polyOneP3[2] ) * oneThird;

      const Vertex &polyTwoP1 = _vertices.at( p2 );
      const Vertex &polyTwoP2 = _vertices.at( p2 + 1 );
      const Vertex &polyTwoP3 = _vertices.at( p2 + 2 );

      Vertex polyTwoCenter;
      polyTwoCenter[0] = ( polyTwoP1[0] + polyTwoP2[0] + polyTwoP3[0] ) * oneThird;
      polyTwoCenter[1] = ( polyTwoP1[1] + polyTwoP2[1] + polyTwoP3[1] ) * oneThird;
      polyTwoCenter[2] = ( polyTwoP1[2] + polyTwoP2[2] + polyTwoP3[2] ) * oneThird;

      return polyOneCenter < polyTwoCenter;
    }
  private:
    const VertexSequence& _vertices;
  };


template < class IndexSequence, class Iter >
inline void removeFromIndexSequence(IndexSequence &indices, Iter& iter)
{
  Iter temp ( iter );
  --iter;
  indices.erase ( temp );
}

template < >
inline void removeFromIndexSequence < std::list< unsigned int> , std::list< unsigned int>::reverse_iterator> 
( std::list< unsigned int> &indices, std::list< unsigned int>::reverse_iterator& iter)
{
  std::list< unsigned int>::iterator temp ( iter.base() );
  --iter;
  indices.erase ( temp );
}


template < class KeeperIter, class Iter, class VertexSequence, class IndexSequence, class AdjacentTest >
inline bool isPolygonAdjacent ( KeeperIter& keeperItr,
                           Iter& indexItr,
                           const VertexSequence& vertices,
                           IndexSequence& keepers,
                           IndexSequence& indices,
                           const AdjacentTest& adjacentTest,
                           unsigned int numVertsPerPoly,
                           std::vector< unsigned int >& adjacentCount )
{
  // See if the current keeper is adjacent to this polygon.
  if ( true == adjacentTest ( vertices, *keeperItr * numVertsPerPoly, *indexItr * numVertsPerPoly ) )
  {
    adjacentCount[ *keeperItr ] ++;
    adjacentCount[ *indexItr ] ++;
    // Put the polygon into the list of keepers.
    keepers.push_back ( *indexItr );
    removeFromIndexSequence( indices, indexItr );
    return true;
  }

  return false;
}


template < class Iter, class VertexSequence, class IndexSequence, class AdjacentTest >
inline Iter searchPolygonList ( Iter& keeperItr,
                           Iter& indexItr,
                           const VertexSequence& vertices,
                           IndexSequence& keepers,
                           IndexSequence& indices,
                           const AdjacentTest& adjacentTest,
                           unsigned int numVertsPerPoly,
                           std::vector< unsigned int >& adjacentCount )
{
  IndexSequence::reverse_iterator rIter ( indexItr );
  IndexSequence::iterator fIter( indexItr );

  while ( adjacentCount [ *keeperItr ] != numVertsPerPoly )
  {
    if( rIter != indices.rend() )
    {
      if ( isPolygonAdjacent ( keeperItr, rIter, vertices, keepers, indices, adjacentTest, numVertsPerPoly, adjacentCount) )
      {
        ++rIter;
        return rIter.base();
      }
      ++rIter;
    }
     
    if( fIter != indices.end() )
    {
      if ( isPolygonAdjacent ( keeperItr, fIter, vertices, keepers, indices, adjacentTest, numVertsPerPoly, adjacentCount) )
      {
        ++fIter;
        return fIter;
      }
      ++fIter;
    }
    if( rIter == indices.rend() && fIter == indices.end() )
      break;
  }
  return indices.end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input:
//
//     adjacentTest:  Predicate that determines if two polygons are adjacent.
//          updater:  Functor that updates something with a new percentage.
//           sorter:  Predicate that determeines the order of the polygon indices.
//         vertices:  Sequence of vertices that define polygons.
//  selectedPolygon:  Initial polygon to start checking against.
//  numVertsPerPoly:  Number of vertices in a polygon. Triangle = 3.
//
//  Output:
//
//          keepers:  List of indices into "vertices". These vertices make 
//                    up polygons that are adjacent, either directly or 
//                    recursively, to the initial polygon "selectedPolygon".
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class AdjacentTest,
  class Updater,
  class Sorter,
  class VertexSequence,
  class IndexSequence
> 
inline void findAdjacent ( const AdjacentTest &adjacentTest, 
                    Updater &updater,
                    const Sorter &sorter,
                    const VertexSequence &vertices, 
                    unsigned int selectedPolygon,
                    unsigned int numVertsPerPoly,
                    IndexSequence &keepers )
{
  typedef typename VertexSequence::value_type Vertex;
  typedef typename VertexSequence::size_type SizeType;
  typedef typename VertexSequence::const_iterator VertexIterator;
  typedef typename IndexSequence::iterator IndexIterator;

  // Initialize.
  keepers.erase ( keepers.begin(), keepers.end() );

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
  SizeType numPolygons ( vertices.size() / numVertsPerPoly );
  if ( selectedPolygon >= numPolygons )
  {
    std::ostringstream message;
    message << "Error 3575595664, selected polygon is " << selectedPolygon << " but there are only " << numPolygons << " polygons";
    throw std::runtime_error ( message.str() );
  }

  // Fill the sequence of indices.
  IndexSequence indices;
  indices.resize ( numPolygons ); // numPolygons - 1 ?
  {
    SizeType count ( 0 );
    for ( IndexIterator i = indices.begin(); i != indices.end(); ++i )
    {
      if( count != selectedPolygon )
        *i = count;
      ++count;
    }
  }

  updater.statusMessage ( "Sorting..." );
  indices.sort ( sorter );

  // Put the first polygon in the list of keepers.
  keepers.insert ( keepers.end(), selectedPolygon );

  // Initialize the current keeper.
  IndexIterator keeperItr = keepers.begin();

  // Make a vector that contains the number of adjacent polygons found
  std::vector < unsigned int > adjacentCount ( numPolygons, 0 );

  // While there are still keepers to test...
  while ( keepers.end() != keeperItr )
  {
    // Loop through all polygons
    IndexIterator indexItr = indices.begin();  
    while ( indices.end() != indexItr )
    {
      indexItr = searchPolygonList <IndexIterator, VertexSequence, IndexSequence, AdjacentTest > 
        ( keeperItr, indexItr, vertices, keepers, indices, adjacentTest, numVertsPerPoly, adjacentCount); 
    }

    // Go to the next keeper.
    ++keeperItr;
    updater ( keepers );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input:
//
//     adjacentTest:  Predicate that determines if two polygons are adjacent.
//         vertices:  Sequence of vertices that define polygons.
//  numVertsPerPoly:  Number of vertices in a polygon. Triangle = 3.
//
//  Output:
//
//     adjacentList:  Vector of Lists that are the polygons that are adjacent
//                    to the index that the list resides in.
//
///////////////////////////////////////////////////////////////////////////////
template < class VertexSequence, class AdjacentList, class AdjacentTest >
inline void buildAdjacentList( AdjacentList& adjacentList, const VertexSequence& vertices, const AdjacentTest& adjacentTest, unsigned int numVertsPerPoly )
{
  //TODO add error checking

  unsigned int numPolygons ( vertices.size() / numVertsPerPoly );
  adjacentList.resize( numPolygons );

  for( AdjacentList::iterator i = adjacentList.begin(); i != adjacentList.end() !=; ++i )
  {
    for( AdjacentList::iterator j = i + 1; j != adjacentList.end(); ++j )
    {
      if( i->size() == numVertsPerPoly)
        break;
      if( true == adjacentTest ( vertices, *i * numVertsPerPoly, *j * numVertsPerPoly ) )
      {
        i->push_back( *j );
        j->push_back( *i );
      }
    }
  }
}

}; // namespace Polygons
}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_POLYGONS_H_
