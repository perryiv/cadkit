
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

#include "Usul/Containers/OctTree.h"
#include "Usul/Math/Vector4.h"

namespace Usul {
namespace Algorithms {
namespace Polygons {

  template < class VertexSequence >
  struct Triangle
  {
    typedef void * CubeType;
    typedef unsigned int IndexType;
    Triangle() : v0(0), v1(0), v2(0), cube(0){}
    Triangle ( unsigned int startPosition ) : 
      v0( startPosition ), 
      v1( startPosition + 1 ), 
      v2( startPosition + 2 ), 
      cube(0) 
      {}
    IndexType v0;
    IndexType v1;
    IndexType v2;
    CubeType cube;

    bool operator==( const Triangle& tri )
    {
      return (this->v0 == tri.v0 && this->v1 == tri.v1 && this->v2 == tri.v2 );
    }
  };

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
  class Polygon,
  class AdjacentTest,
  class Updater,
  class Vertex,
  class VertexSequence,
  class IndexSequence
> 
inline void findAdjacent ( const AdjacentTest &adjacentTest, 
                    Updater &updater,
                    const Vertex &center,
                    float radius,
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
  /*IndexSequence indices;
  indices.resize ( numPolygons - 1 );
  {
    SizeType count ( 0 );
    for ( IndexIterator i = indices.begin(); i != indices.end(); ++i )
    {
      *i = count;
      ++count;
      if( count == selectedPolygon )
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
    }*/

    typedef float Real;
    typedef Usul::Math::Vector4<Real> Vec4;
    typedef Usul::Math::Vector3<Real> Vec3;
    typedef std::vector<Polygon> Polygons;
    typedef VertexSequence Vertices;
    typedef Usul::Containers::Detail::AdjacentChildren Adjacent;
    typedef Usul::Containers::Detail::TriangleInside<Vertices> Inside;
    typedef Usul::Containers::Detail::AddIfInside Add;
    typedef Usul::Containers::OctTree<Polygons,Adjacent,Inside,Add,Real,Vec3> OctTree;

    const Vertex mn ( center[0] - radius, center[1] - radius, center[2] - radius );

    // Declare the oct-tree and set its members.
    OctTree root ( 2 * radius, mn[0], mn[1], mn[2] );

    root.tester().vertices ( &vertices );

    // The number of polygons.
    const unsigned int numVertices ( vertices.size() );

    // Add the triangles to the oct-tree.
    Polygons polygons;
    polygons.reserve ( numPolygons );
    unsigned int count ( 0 );
    while ( count < numVertices )
    {
      Polygon polygon ( count );
      count += numVertsPerPoly;

      // Try to add it to the oct-tree.
      if ( !root.add ( polygon ) )
      {
        std::ostringstream message;
        message << "Failed to add polygon " << count / numVertsPerPoly << " of " << numPolygons <<  std::endl;
        throw std::runtime_error ( message.str() );
      }

      // Save this triangle.
      polygons.push_back ( polygon );
    }


    // Initialize the keepers.
    keepers.push_back ( selectedPolygon );

    // Vector of adjacent counts.
    typedef std::vector<unsigned int> AdjacentCount;
    AdjacentCount adjacentCounts ( numPolygons, 0 );

    //vector of bool to indicate if triangle is in the keeper list
    std::vector<bool> keepersFound ( numPolygons, false );
    keepersFound[ selectedPolygon ] = true;

    // Initialize the current keeper.
    IndexIterator k = keepers.begin();

    // While there are still keepers to test...
    while ( keepers.end() != k )
    {
      // Get the index of this keeper.
      const unsigned int keeperIndex ( *k );

      // Get the Polygon.
      Polygon &keep = polygons.at ( keeperIndex );

      // Get the cube it resides in.
      OctTree *cube = reinterpret_cast < OctTree * > ( keep.cube );

      // See if we already have all the adjacent triangles for this one...
      while ( adjacentCounts.at ( keeperIndex ) < numVertsPerPoly )
      {
        // Get this cube's triangles.
        Polygons &cubesPolygons = cube->objects();

        // Loop through the contained triangles.
        for ( Polygons::iterator t = cubesPolygons.begin(); t != cubesPolygons.end(); ++t )
        {
          // If we found all possible adjacent triangles then stop.
          if ( numVertsPerPoly <= adjacentCounts.at ( keeperIndex ) )
            break;

          // Get the contained triangle.
          Polygon &contained = *t;
          const unsigned int polygonIndex ( contained.v0 / numVertsPerPoly );

          USUL_ASSERT ( contained.v0 < vertices.size() );
          USUL_ASSERT ( polygonIndex < polygons.size() );

          // See if this contained triangle is adjacent to the keeper.
          // Make sure we don't test the same triangle with itself.
          if ( ( contained.v0 != keep.v0 ) && !keepersFound[ polygonIndex ] && ( true == adjacentTest ( vertices, contained.v0, keep.v0 ) ) )
          {
            // Add this to the keepers.
            keepers.push_back ( polygonIndex );

            keepersFound[ polygonIndex ] = true;

            // Increment the adjacent-count.
            adjacentCounts.at(keeperIndex) += 1;
            adjacentCounts.at(polygonIndex) += 1;
          }
        }

        cube = cube->parent();
        if( 0x0 == cube )
          break;
      }

      // Go to the next keeper.
      ++k;
      updater ( keepers );
    }

}

namespace Detail
{

///////////////////////////////////////////////////////////////////////////////
//
//  Shared Vertex Class
//
///////////////////////////////////////////////////////////////////////////////

struct Triangle;

struct SharedVertex
{
  typedef std::list< Triangle * >::iterator Iterator;
  SharedVertex( const osg::Vec3& vec) : _triangles(), _vertex( vec ), _visited( false ) { }
  void append( Triangle *t ) { _triangles.push_back( t ); }

  std::list< Triangle * > _triangles;
  osg::Vec3 _vertex;
  bool _visited;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Triangle Class
//
///////////////////////////////////////////////////////////////////////////////

struct Triangle
{
  Triangle() : _v1( 0x0 ), _v2 ( 0x0 ), _v3 ( 0x0 ), _visited( false ) { }
  SharedVertex *_v1, *_v2, *_v3;
  bool _visited;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Base Functor
//
///////////////////////////////////////////////////////////////////////////////
struct Functor
{
  virtual void operator() () = 0;
};

///////////////////////////////////////////////////////////////////////////////
//
//  VisitVertex Functor
//
///////////////////////////////////////////////////////////////////////////////
template < class VisitTriangle >
struct VisitVertex : public Functor
{
  VisitVertex ( SharedVertex *sv, std::list< Functor * > &todoStack ) : _sharedVertex ( sv ), _todoStack( todoStack ) {}
  virtual void operator() ()
  {
    //_sharedVertex->_visited = true;
    for(SharedVertex::Iterator i = _sharedVertex->_triangles.begin(); i != _sharedVertex->_triangles.end(); ++i)
    {
      if( !(*i)->_visited )
      {
        _todoStack.push_back( new VisitTriangle ( *i, _todoStack ) );
        (*i)->_visited = true;
      }
    }
  }
private:
  SharedVertex *_sharedVertex;
  std::list< Functor *> &_todoStack;
};

///////////////////////////////////////////////////////////////////////////////
//
//  VisitTriangle Functor
//
///////////////////////////////////////////////////////////////////////////////
struct VisitTriangle : public Functor
{
  VisitTriangle ( Triangle *t, std::list< Functor* > &todoStack ) : _triangle ( t ), _todoStack( todoStack ) { }
  virtual void operator()()
  {
    //_triangle->_visited = true;
    if( !_triangle->_v1->_visited )
    {
      _todoStack.push_back( new VisitVertex< VisitTriangle > ( _triangle->_v1, _todoStack ) );
      _triangle->_v1->_visited = true;
    }
    if( !_triangle->_v2->_visited )
    {
      _todoStack.push_back( new VisitVertex< VisitTriangle > ( _triangle->_v2, _todoStack ) );
      _triangle->_v2->_visited = true;
    }
    if( !_triangle->_v3->_visited )
    {
      _todoStack.push_back( new VisitVertex< VisitTriangle > ( _triangle->_v3, _todoStack ) );
      _triangle->_v3->_visited = true;
    }
  }
private:
  Triangle *_triangle;
  std::list< Functor *> &_todoStack;
};

};

///////////////////////////////////////////////////////////////////////////////
//
//  Function for getting a iter to the given key.
//
///////////////////////////////////////////////////////////////////////////////
template < class Iter, class Map >
Iter getMapIter( Map& map, const osg::Vec3& key )
{
  Map::iterator iter = map.find( key );
  if( iter == map.end() )
  {
    //Add a new shared vertex
    map.insert( Map::value_type( key, new Detail::SharedVertex( key ) ) );
    iter = map.find( key );
  }
  return iter;
}

///////////////////////////////////////////////////////////////////////////////
//
//  new findAdjacent algoritm
//
///////////////////////////////////////////////////////////////////////////////

void newFindAdjacent( const osg::Vec3Array& vertices, std::vector< unsigned int > &answer, unsigned int selectedPolygon )
{
  //typedef typename VertexSequence::value_type Vertex;
  //typedef typename VertexSequence::size_type SizeType;
  //typedef typename VertexSequence::const_iterator VertexIterator;
  typedef osg::Vec3Array::const_iterator VertexIterator;
  typedef unsigned int SizeType;
  
  unsigned int numVertsPerPoly = 3;

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
  
  typedef std::vector< Detail::Triangle* > Triangles;

  Triangles triangles( numPolygons, 0x0 );

  typedef std::map< osg::Vec3, Detail::SharedVertex * > Map;
  Map sharedVertsMap;

  Triangles::iterator currentTriangle = triangles.begin();
  for( VertexIterator i = vertices.begin(); i != vertices.end(); i+=3 )
  {
    *currentTriangle = new Detail::Triangle();

    Map::iterator v1 = getMapIter< Map::iterator, Map > ( sharedVertsMap, *i );
    Map::iterator v2 = getMapIter< Map::iterator, Map > ( sharedVertsMap, *(i + 1 ));
    Map::iterator v3 = getMapIter< Map::iterator, Map > ( sharedVertsMap, *(i + 2 ));

    (*currentTriangle)->_v1 = v1->second;
    (*currentTriangle)->_v2 = v2->second;
    (*currentTriangle)->_v3 = v3->second;

    v1->second->append( *currentTriangle );
    v2->second->append( *currentTriangle );
    v3->second->append( *currentTriangle );

    ++currentTriangle;
  }

  std::list< Detail::Functor* > todoStack;

  Detail::Functor *first = new Detail::VisitTriangle( triangles.at( selectedPolygon ), todoStack );
  triangles.at( selectedPolygon )->_visited = true;

  todoStack.push_back( first );

  std::list< Detail::Functor *>::iterator todoIterator = todoStack.begin();

  while( todoIterator != todoStack.end() )
  {
    (*(*todoIterator))();
    Detail::Functor *functor = *todoIterator;
    todoStack.pop_front();
    delete functor;
    todoIterator = todoStack.begin();
  }

  for(unsigned int i = 0; i < triangles.size(); ++i )
  {
    if( triangles.at(i)->_visited )
      answer.push_back( i );
    delete triangles.at(i);
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
