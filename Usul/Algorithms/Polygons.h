
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

#include <numeric>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <list>


namespace Usul {
namespace Algorithms {
namespace Polygons {

  
namespace Detail
{

///////////////////////////////////////////////////////////////////////////////
//
//  Shared Vertex Class
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygon >
class SharedVertex
{
public:
  typedef typename std::list< Polygon * >::iterator Iterator;
  SharedVertex( ) : _polygons(), _visited( false ) { }
  void append( Polygon *t ) { _polygons.push_back( t ); }
  Iterator begin() { return _polygons.begin(); }
  Iterator end() { return _polygons.end(); }

  bool visited() { return _visited; }
  void visited( bool v ) { _visited = v; }
  
private:
  std::list< Polygon * > _polygons;
  bool _visited;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Polygon Class
//
///////////////////////////////////////////////////////////////////////////////
template < int VertsPerPoly >
class Polygon
{
public:
  typedef SharedVertex< Polygon > SharedVertex;
  typedef std::vector< SharedVertex* > Vertices;
  typedef typename Vertices::iterator Iterator;

  Polygon() : _index( 0 ), _vertices(), _visited( false ) { _vertices.reserve( VertsPerPoly ); }

  void append( SharedVertex *sv ) { _vertices.push_back( sv ); }
  Iterator begin() { return _vertices.begin(); }
  Iterator end() { return _vertices.end(); }

  bool visited() { return _visited; }
  void visited( bool v ) { _visited = v; }

  unsigned int index() { return _index; }
  void index( unsigned int i ) { _index = i; }
private:
  unsigned int _index;
  Vertices _vertices;
  bool _visited;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Functor for pushing tasks on to the todo stack.
//
///////////////////////////////////////////////////////////////////////////////
template < class IndexSequence, class Polygon, class SharedVertex >
struct UberFunctor
{
  typedef std::vector< UberFunctor > TodoStack;

  UberFunctor ( IndexSequence& answer, TodoStack& todoStack, Polygon *p ) :
  _answer( answer ),
  _todoStack( todoStack ),
  _polygon ( p ),
  _sharedVertex( 0x0 )
  { }
  UberFunctor ( IndexSequence& answer, TodoStack& todoStack, SharedVertex *sv ) :
  _answer( answer ),
  _todoStack( todoStack ),
  _polygon ( 0x0 ),
  _sharedVertex( sv )
  { }

  void operator() ()
  {
    if ( _polygon )
    {
      for( Polygon::Iterator sv = _polygon->begin(); sv != _polygon->end(); ++sv )
      {
        if( !(*sv)->visited() )
        _todoStack.push_back( UberFunctor < IndexSequence, Polygon, SharedVertex > ( _answer, _todoStack, *sv ) );
        (*sv)->visited ( true );
      }
      _answer.push_back( _polygon->index() );
    }
    if( _sharedVertex )
    {
      for(SharedVertex::Iterator i = _sharedVertex->begin(); i != _sharedVertex->end(); ++i)
      {
        if( !(*i)->visited() )
        {
          _todoStack.push_back( UberFunctor < IndexSequence, Polygon, SharedVertex > ( _answer, _todoStack, *i ) );
          (*i)->visited ( true );
        }
      }
    }
  }

  bool operator= ( const UberFunctor& that )
  {
    this->_answer = that._answer;
    this->_todoStack = that._todoStack;
    this->_polygon = that._polygon;
    this->_sharedVertex = that._sharedVertex;
    return true;
  }
private:
  IndexSequence &_answer;
  TodoStack &_todoStack;
  Polygon *_polygon;
  SharedVertex *_sharedVertex;
};

}; // namespace Detail

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Test to see if 2 triangles are adjacent
  //
  ///////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////
//
//  Function for getting a iter to the given key.
//
///////////////////////////////////////////////////////////////////////////////
template < class Iter, class Map, class Value >
inline Iter getMapIter( Map& map, const osg::Vec3& key, std::vector< Value >& values )
{
  Map::iterator iter = map.find( key );
  if( iter == map.end() )
  {
    //Add a new shared vertex
    values.push_back( Value() );
    map.insert( Map::value_type( key, &values.at( values.size() - 1 ) ) );
    iter = map.find( key );
  }
  return iter;
}

namespace Functors
{

struct NoCancelFunctor
{
  bool operator() () const { return false; }
};

template < class IndexSequence >
struct NoUpdateFunctor
{
   void operator () ( const std::string& )
    {
    }

    void operator() ( const IndexSequence& keepers, bool b = false )
    {
    }
};

};

///////////////////////////////////////////////////////////////////////////////
//
//  Function for building adjacency graph
//
//  polygons should be resized to the number of polygons
//  sharedVerts should be reserved to at least the number of polygons * 1.5
//
///////////////////////////////////////////////////////////////////////////////
template
<
  class PolygonSequence,
  class SharedVertexSequence,
  class VertexSequence,
  class Map,
  class UpdateFunctor,
  class CancelFunctor
> 
inline void buildAdjacencyGraph (
                    PolygonSequence &polygons,
                    SharedVertexSequence &sharedVerts,
                    const VertexSequence &vertices, 
                    Map &sharedVertsMap,
                    UpdateFunctor &updater,
                    const CancelFunctor &cancelFunctor,                
                    unsigned int numVertsPerPoly
                    )
{
  typedef typename PolygonSequence::value_type Polygon;
  typedef typename SharedVertexSequence::value_type SharedVertex;

  typedef typename VertexSequence::const_iterator VertexIterator;

  //build the polygons, shared vertices, and map
  updater ( "Finding shared vertices...");
  PolygonSequence::iterator currentPolygon ( polygons.begin() );
  unsigned int polyIndex ( 0 );
  //Loop through all the vertices
  for( VertexIterator i = vertices.begin(); i != vertices.end(); i+=numVertsPerPoly )
  {
    //Do we need to stop?
    if( cancelFunctor() )
      return;

    //Add the vertices to the polygon
    for( unsigned int j = 0; j < numVertsPerPoly; ++j )
    {
      Map::iterator iter = getMapIter< Map::iterator, Map, SharedVertex > ( sharedVertsMap, *(i + j), sharedVerts );
      currentPolygon->append ( iter->second );
      iter->second->append( &*currentPolygon );
    }
    //Set the polygon's index and go to the next polygon
    currentPolygon->index( polyIndex++ );
    ++currentPolygon;

    //update the progress
    if( sharedVerts.size() % 250 == 0 )
    {
      std::ostringstream message;
      message << "Found " << sharedVerts.size() << " shared vertices. ";
      updater ( message.str() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function for walking adjacency graph
//
///////////////////////////////////////////////////////////////////////////////
template
<
  class PolygonSequence,
  class SharedVertexSequence,
  class IndexSequence,
  class UpdateFunctor,
  class CancelFunctor
> 
inline void walkAdjacencyGraph (
                    PolygonSequence &polygons,
                    SharedVertexSequence &sharedVerts,
                    IndexSequence &answer,
                    UpdateFunctor &updater,
                    const CancelFunctor &cancelFunctor,
                    unsigned int selectedPolygon,
                    unsigned int numVertsPerPoly
                    )
{
  typedef typename SharedVertexSequence::value_type SharedVertex;
  typedef typename PolygonSequence::value_type Polygon;
  typedef Detail::UberFunctor< IndexSequence, Polygon, SharedVertex > Functor;
  typedef std::vector< Functor > TodoStack;

  TodoStack todoStack;
  todoStack.reserve( polygons.size() + sharedVerts.size() );

  //put the functor for the selected polygon on the stack
  polygons.at( selectedPolygon ).visited ( true );
  todoStack.push_back( Functor ( answer, todoStack, &polygons.at( selectedPolygon ) ) );

  TodoStack::iterator todoIterator = todoStack.begin();

  //loop through the todo stack
  while( todoIterator != todoStack.end() )
  {
    //Do we need to stop?
    if( cancelFunctor() )
      return;

    (*todoIterator)();
    //todoStack.pop_front();
    ++todoIterator;// = todoStack.begin();
    updater ( answer );
  }

  updater ( answer, true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Input:
//
//          updater:  Functor that updates something with a new percentage.
//         vertices:  Sequence of vertices that define polygons.
//  selectedPolygon:  Initial polygon to start checking against.
//  numVertsPerPoly:  Number of vertices in a polygon. Triangle = 3.
//
//  Output:
//
//          answer:   List of indices into "vertices". These vertices make 
//                    up polygons that are adjacent, either directly or 
//                    recursively, to the initial polygon "selectedPolygon".
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Polygon,
  class SharedVertex,
  class UpdateFunctor,
  class VertexSequence,
  class IndexSequence,
  class CancelFunctor
> 
inline void findAdjacent (
                    UpdateFunctor &updater,
                    const VertexSequence &vertices, 
                    unsigned int selectedPolygon,
                    unsigned int numVertsPerPoly,
                    IndexSequence &answer,
                    const CancelFunctor &cancelFunctor )
{
  //Useful typedefs
  typedef typename VertexSequence::value_type Vertex;
  typedef typename VertexSequence::size_type SizeType;
  typedef typename VertexSequence::const_iterator VertexIterator;
  typedef typename IndexSequence::iterator IndexIterator;

  typedef std::vector< Polygon > Polygons;
  typedef std::map< Vertex, SharedVertex * > Map;

  // Initialize.
  answer.erase ( answer.begin(), answer.end() );

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

  //Vector to hold all polygons
  Polygons polygons;
  polygons.resize( numPolygons );
  //Vector to hold all shared vertices
  typedef std::vector< SharedVertex > SharedVertices;
  SharedVertices sharedVerts;
  sharedVerts.reserve( (int ) (numPolygons * 1.6) );
  //Map of the shared vertices
  Map sharedVertsMap;

  buildAdjacencyGraph < Polygons, SharedVertices, VertexSequence, Map, UpdateFunctor, CancelFunctor > 
    ( polygons, sharedVerts, vertices, sharedVertsMap, updater, cancelFunctor, numVertsPerPoly );

  walkAdjacencyGraph < Polygons, SharedVertices, IndexSequence, UpdateFunctor, CancelFunctor > 
    ( polygons, sharedVerts, answer, updater, cancelFunctor, selectedPolygon, numVertsPerPoly );

}

}; // namespace Polygons
}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_POLYGONS_H_

