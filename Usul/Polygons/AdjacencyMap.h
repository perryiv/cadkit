
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
class AdjacencyMap
{
public:

  //Useful typedefs
  typedef typename VertexSequence::value_type VertexType;
  typedef typename VertexSequence::size_type SizeType;
  typedef typename VertexSequence::const_iterator VertexIterator;
  typedef SharedVertex < Polygon, VertexType > SharedVertex;
  typedef std::vector < Polygon > Polygons;
  typedef std::map < VertexType, SharedVertex, Compare > Map;

  AdjacencyMap() :
    _sharedVertsMap(),
    _polygons()
  {
    USUL_ASSERT_SAME_TYPE ( VertexType, typename Polygon::VertexType );
    USUL_ASSERT_SAME_TYPE ( VertexType, typename SharedVertex::ValueType );
  }

  AdjacencyMap( const Compare& c ) :
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
      _polygons.push_back( Polygon() );
      Polygon &p = _polygons.back();

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
          _sharedVertsMap.insert( Map::value_type( key, SharedVertex() ) );

          //Reset the iterator
          iter = _sharedVertsMap.find( key );
        }

        //add the shared vertex to the polygon
        p.append ( &iter->second );

        //add the polygon to the shared vertex
        iter->second.append( &p );
      }

      //Set the polygon's index and go to the next polygon
      p.index( _polygons.size() - 1 );

      //update the progress
      if( _sharedVertsMap.size() % 1000 == 0 )
        updater ( _sharedVertsMap.size() );
    }
  }


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
    _polygons.at( selectedPolygon ).visited ( true );
    todoStack.push_back( Functor ( answer, todoStack, &_polygons.at( selectedPolygon ) ) );

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

  void setAllUnvisited()
  {
    for( Polygons::iterator iter = _polygons.begin(); iter != _polygons.end(); ++iter )
      iter->visited( false );

    for( Map::iterator i = _sharedVertsMap.begin(); i != _sharedVertsMap.end(); ++i )
      i->second.visited( false );
  }

  unsigned int size() const
  {
    return _polygons.size() + _sharedVertsMap.size();
  }

private:

  Map            _sharedVertsMap;
  Polygons       _polygons;

}; //class AdjacencyMap


} //namespace Polygons
} //namespace Usul


#endif // __USUL_POLYGONS_ADJACENCY_MAP_H__
