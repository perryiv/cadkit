
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_ADJACENCY_MAP_H__
#define __USUL_POLYGONS_ADJACENCY_MAP_H__


#include "Usul/Export/Export.h"
#include "Usul/Polygons/SharedVertex.h"

namespace Usul {
namespace Polygons {


namespace Functors
{

struct NoCancelFunctor
{
  bool operator() () const { return false; }
};

template < class IndexSequence >
struct NoUpdateFunctor
{
  void operator () ( int ) { }
  void operator () ( const std::string& ) { }
  void operator () ( const IndexSequence& keepers, bool b = false ) { }
};

};

//////////////////////////////////////////////////////////////////////////////
//
//  Adjacency Graph Class
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Polygon,
  class VertexSequence,
  class UpdateFunctor = Functors::NoUpdateFunctor,
  class CancelFunctor = Functors::NoCancelFunctor
>
class AdjacencyMap
{
public:
  //Useful typedefs
  typedef typename VertexSequence::value_type Vertex;
  typedef typename VertexSequence::size_type SizeType;
  typedef typename VertexSequence::const_iterator VertexIterator;
  typedef SharedVertex< Polygon > SharedVertex;
  typedef std::vector< Polygon > Polygons;
  typedef std::map< Vertex, SharedVertex* > Map;
  typedef std::vector< SharedVertex > SharedVertices;

  AdjacencyMap() :
  _sharedVertsMap(),
  _sharedVerts(),
  _polygons()
  {
  }

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
    _sharedVerts.reserve ( _sharedVerts.size() + numPolygons );

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
        Vertex key ( *(i + j ) );

        //Get the iterator if the key is in the map
        Map::iterator iter ( _sharedVertsMap.find( key ) );
        
        //Do we have a shared vertex already?
        if( iter == _sharedVertsMap.end() )
        {
          //Add the shared vertex
          _sharedVerts.push_back( SharedVertex() );

          //Add the shared vertex to te map
          _sharedVertsMap.insert( Map::value_type( key, &_sharedVerts.back() ) );

          //Reset the iterator
          iter = _sharedVertsMap.find( key );
        }

        //add the shared vertex to the polygon
        p.append ( iter->second );

        //add the polygon to the shared vertex
        iter->second->append( &p );
      }

      //Set the polygon's index and go to the next polygon
      p.index( _polygons.size() - 1 );

      //update the progress
      if( _sharedVerts.size() % 1000 == 0 )
        updater ( _sharedVerts.size() );
    }
  }


  template < class Functor, class IndexSequence >
  void walkPolygons( IndexSequence& answer, UpdateFunctor& updater, const CancelFunctor& cancel, unsigned int selectedPolygon )
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
    todoStack.reserve( _polygons.size() + _sharedVerts.size() );

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


private:
  Map            _sharedVertsMap;
  SharedVertices _sharedVerts;
  Polygons       _polygons;

}; //class AdjacencyMap

} //namespace Polygons

} //namespace Usul


#endif // __USUL_POLYGONS_ADJACENCY_MAP_H__
