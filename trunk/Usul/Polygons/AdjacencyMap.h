
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_ADJACENCY_MAP_H__
#define __USUL_POLYGONS_ADJACENCY_MAP_H__

#include "Usul/Polygons/SharedVertex.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <functional>
#include <map>

namespace Usul {
namespace Polygons {


namespace Functors
{

struct NoCancelFunctor
{
  bool operator() () const { return false; }
};


struct NoUpdateFunctor
{
  void operator () ( int ) { }
  void operator () ( const std::string& ) { }
  template < class IndexSequence > void operator () ( const IndexSequence& keepers, bool b = false ) { }
};


} //namespace Functors


namespace Compare
{
  template< class Vertex, class TolType = Vertex >
  struct VertexTolerance : public std::binary_function< Vertex, Vertex, bool >
  {
    VertexTolerance( const TolType& t ) : _tol ( t ) { }
    bool operator () ( const Vertex& v1, const Vertex& v2 ) const
    {
#if 1
      //if ( v1 < v2 )
        return ( v1 + _tol ) < v2;
      //if( v2 < v1 )
      //  return ( v2 + _tol ) < v1;
      //return false;
#else
      return ( v1 < v2 );
#endif
    }
  private:
    VertexTolerance();
    TolType _tol;
  };
} //namespace Compare


///////////////////////////////////////////////////////////////////////////////
//
//  Adjacency Graph Class
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Polygon,
  class VertexSequence,
  class Compare = std::less< VertexSequence::value_type >
>
class AdjacencyMap : public Usul::Base::Referenced
{
public:

  // Useful typedefs
  typedef Usul::Base::Referenced BaseClass;
  typedef typename VertexSequence::value_type VertexType;
  typedef typename VertexSequence::size_type SizeType;
  typedef typename VertexSequence::const_iterator VertexIterator;
  typedef SharedVertex < Polygon, VertexType > SharedVertex;
  typedef VertexType NormalType;
  typedef typename SharedVertex::RefPtr SharedVertexPtr;
  typedef typename Polygon::ValidRefPtr PolygonPtr;
  typedef std::vector < PolygonPtr > Polygons;
  typedef std::map < VertexType, SharedVertexPtr, Compare > Map;

  // Smart pointers.
  USUL_DECLARE_REF_POINTERS ( AdjacencyMap );


  //////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor
  //
  ///////////////////////////////////////////////////////////////////////////////

  AdjacencyMap() : BaseClass(),
    _sharedVertsMap(),
    _polygons()
  {
    USUL_ASSERT_SAME_TYPE ( VertexType, typename Polygon::VertexType );
    USUL_ASSERT_SAME_TYPE ( VertexType, typename SharedVertex::ValueType );
  }

  //////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor with comparision predicate
  //
  ///////////////////////////////////////////////////////////////////////////////

  AdjacencyMap ( const Compare& c ) : BaseClass()
    _sharedVertsMap( c ),
    _polygons()
  { 
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear the data.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    _sharedVertsMap.clear();
    _polygons.clear();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access the map of shared vertices.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Map &sharedVertsMap() const
  {
    return _sharedVertsMap;
  }
  Map &sharedVertsMap()
  {
    return _sharedVertsMap;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access the polygons.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Polygons &polygons() const
  {
    return _polygons;
  }
  Polygons &polygons()
  {
    return _polygons;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return shared vertex for given value. Make one if needed.
  //
  /////////////////////////////////////////////////////////////////////////////

  SharedVertex *sharedVertex ( const VertexType &v )
  {
    Map::iterator i ( _sharedVertsMap.find ( v ) );
    if ( _sharedVertsMap.end() == i )
    {
      typedef std::pair < Map::iterator, bool > InsertResult;
      InsertResult result (  _sharedVertsMap.insert ( Map::value_type ( v, new SharedVertex ( v ) ) ) );
      return result.first->second.get();
    }
    else
    {
      return i->second.get();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Add single triangle. Note: if Polygon does not have triangle-like API, 
  //  then this won't compile.
  //
  /////////////////////////////////////////////////////////////////////////////

  void add ( const VertexType &v0, const VertexType &v1, const VertexType &v2, const NormalType &n )
  {
    // Add a new polygon.
    _polygons.push_back ( new Polygon );
    PolygonPtr p ( _polygons.back() );
    p->normal ( n );

    // Get shared vertices and hook things up.
    SharedVertex *v = this->sharedVertex ( v0 );
    v->append ( p );
    p->append ( v );

    // Same for second vertex.
    v = this->sharedVertex ( v1 );
    v->append ( p );
    p->append ( v );

    // And third vertex.
    v = this->sharedVertex ( v2 );
    v->append ( p );
    p->append ( v );

    // Set polygon's index.
    p->index ( _polygons.size() - 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Add vertices.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class UpdateFunctor, class CancelFunctor >
  void add ( const VertexSequence& vertices, UpdateFunctor& updater, const CancelFunctor& cancel, unsigned int numVertsPerPoly )
  {
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

    //Number of polygons
    unsigned int numPolygons ( vertices.size() / numVertsPerPoly );

    //Reserve enough room
    _polygons.reserve( _polygons.size() + numPolygons );

    //Loop through all the vertices
    for( VertexIterator i = vertices.begin(); i != vertices.end(); i += numVertsPerPoly )
    {
      //Do we need to stop?
      if( cancel() )
        return;

      //Add a new polygon
      PolygonPtr p ( new Polygon() );
      _polygons.push_back( p.get() );

      //Add the vertices to the polygon
      for( unsigned int j = 0; j < numVertsPerPoly; ++j )
      {
        //Key to find shared vertex in map
        VertexType key ( *(i + j ) );

        //Get the iterator if the key is in the map
        Map::iterator iter ( _sharedVertsMap.find( key ) );
        
        //Do we have a shared vertex already?
        if( iter == _sharedVertsMap.end() )
        {
          //Add a shared vertex to te map
          _sharedVertsMap.insert( Map::value_type( key, new SharedVertex( key ) ) );

          //Reset the iterator
          iter = _sharedVertsMap.find( key );
        }

        //add the shared vertex to the polygon
        p->append ( iter->second.get() );

        //add the polygon to the shared vertex
        iter->second->append( p.get() );
      }

      //Set the polygon's index
      p->index( _polygons.size() - 1 );

      //update the progress
      if( _sharedVertsMap.size() % 1000 == 0 )
        updater ( _sharedVertsMap.size() );
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //
  //  Walk the graph.  TODO move to own function
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Functor, class IndexSequence, class UpdateFunctor, class CancelFunctor >
  void walkPolygons ( IndexSequence& answer, UpdateFunctor& updater, const CancelFunctor& cancel, unsigned int selectedPolygon )
  {
    typedef typename IndexSequence::iterator IndexIterator;
    typedef std::vector< Functor > TodoStack;
    typedef TodoStack::iterator TodoStackItr;

    //Is the selected polygon outside of _polygons' range?
    if ( selectedPolygon >= _polygons.size() )
    {
      std::ostringstream message;
      message << "Error 3575595664, selected polygon is " << selectedPolygon << " but there are only " << _polygons.size() << " polygons";
      throw std::runtime_error ( message.str() );
    }

    //Todo stack to simulate recursive function calls
    TodoStack todoStack;

    //Reserve enough room
    todoStack.reserve( _polygons.size() + _sharedVertsMap.size() );

    //put the functor for the selected polygon on the stack
    _polygons.at( selectedPolygon )->visited ( true );
    todoStack.push_back( Functor ( answer, todoStack, _polygons.at( selectedPolygon ).get() ) );

    TodoStackItr todoIterator ( todoStack.begin() );

    //loop through the todo stack
    while( todoIterator != todoStack.end() )
    {
      //Do we need to stop?
      if( cancel() )
        return;

      //Do the functor's thing
      (*todoIterator)();

      //Go to the next one
      ++todoIterator;

      //Send a progress upate
      updater ( answer );
    }

    //Send a progress upate, make sure it updates
    updater ( answer, true );
  }


  //////////////////////////////////////////////////////////////////////////////
  //
  //  Set the polygons and shared verts to not visited.
  //
  ///////////////////////////////////////////////////////////////////////////////
  void setAllUnvisited()
  {
    for( Polygons::iterator iter = _polygons.begin(); iter != _polygons.end(); ++iter )
      (*iter)->visited( false );

    for( Map::iterator i = _sharedVertsMap.begin(); i != _sharedVertsMap.end(); ++i )
      i->second->visited( false );
  }

  //combined size
  unsigned int size() const
  {
    return _polygons.size() + _sharedVertsMap.size();
  }


  //////////////////////////////////////////////////////////////////////////////
  //
  //  Flip the normals.
  //
  ///////////////////////////////////////////////////////////////////////////////

  void flipNormals()
  {
    for ( Polygons::iterator i = _polygons.begin(); i != _polygons.end(); ++i )
      (*i)->flipNormal();
  }

private:

  // No copying.
  AdjacencyMap ( const AdjacencyMap & );
  AdjacencyMap &operator = ( const AdjacencyMap & );

  // Use reference counting
  virtual ~AdjacencyMap()
  {
  }

private:

  Map            _sharedVertsMap;
  Polygons       _polygons;
};


} //namespace Polygons
} //namespace Usul


#endif // __USUL_POLYGONS_ADJACENCY_MAP_H__
