
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_POLYGONS_H__
#define __USUL_POLYGONS_POLYGONS_H__

#include "Usul/Polygons/SharedVertex.h"


namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Polygon Class
//
///////////////////////////////////////////////////////////////////////////////

template < unsigned int numVertsPerPoly = 3>
class Polygon
{
public:
  typedef SharedVertex< Polygon > SharedVertex;
  typedef std::vector< SharedVertex* > Vertices;
  typedef typename Vertices::iterator Iterator;

  Polygon() : _index( 0 ), _vertices(), _visited( false ) { _vertices.reserve( numVertsPerPoly ); }

  //Added a shared vertex
  void append( SharedVertex *sv ) { _vertices.push_back( sv ); }

  Iterator begin () { return _vertices.begin(); }
  Iterator end () { return _vertices.end(); }

  bool visited () const   { return _visited; }
  void visited ( bool v ) { _visited = v; }

  unsigned int index () const           { return _index; }
  void         index ( unsigned int i ) { _index = i; }

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
struct PolygonFunctor
{
  typedef std::vector< PolygonFunctor > TodoStack;

  //Construct with a polygon
  PolygonFunctor ( IndexSequence& answer, TodoStack& todoStack, Polygon *p ) :
  _answer( answer ),
  _todoStack( todoStack ),
  _polygon ( p ),
  _sharedVertex( 0x0 )
  { }

  //Construct with a shared vertex
  PolygonFunctor ( IndexSequence& answer, TodoStack& todoStack, SharedVertex *sv ) :
  _answer( answer ),
  _todoStack( todoStack ),
  _polygon ( 0x0 ),
  _sharedVertex( sv )
  { }

  void operator() ()
  {
    //If we have a polygon...
    if ( _polygon )
    {
      //Loop through the shared vertices in the polygon
      for( Polygon::Iterator sv = _polygon->begin(); sv != _polygon->end(); ++sv )
      {
        //If we haven't visited
        if( !(*sv)->visited() )
        {
          //Add the functor to the todo stack
          _todoStack.push_back( PolygonFunctor < IndexSequence, Polygon, SharedVertex > ( _answer, _todoStack, *sv ) );
          (*sv)->visited ( true );
        }
      }
      //If we get here the polygon is connected, add polygon to answer
      _answer.push_back( _polygon->index() );
    }

    //If we have a shared vertex
    if( _sharedVertex )
    {
      //Loop through the polygons of this shared vertex
      for(SharedVertex::Iterator i = _sharedVertex->begin(); i != _sharedVertex->end(); ++i)
      {
        //If we haven't visited yet
        if( !(*i)->visited() )
        {
          //Add the functor to the todo statck
          _todoStack.push_back( PolygonFunctor < IndexSequence, Polygon, SharedVertex > ( _answer, _todoStack, *i ) );
          (*i)->visited ( true );
        }
      }
    }
  }

  bool operator= ( const PolygonFunctor& that )
  {
    this->_answer = that._answer;
    this->_todoStack = that._todoStack;
    this->_polygon = that._polygon;
    this->_sharedVertex = that._sharedVertex;
    return true;
  }
private:
  IndexSequence &      _answer;
  TodoStack &          _todoStack;
  Polygon *            _polygon;
  SharedVertex *       _sharedVertex;
};

template< class Polygon >
struct PolyLess : public std::binary_function< Polygon, Polygon, bool >
{
  bool operator () ( const Polygon* p1, const Polygon* p2 ) const
  {
    return p1->index() < p2->index();
  }
};

template< class Polygon >
struct PolyEqual : public std::binary_function< Polygon, Polygon, bool >
{
  bool operator () ( const Polygon* p1, const Polygon* p2 ) const
  {
    return p1->index() == p2->index();
  }
};



} //namespace Polygons

} //namespace Usul


#endif // __USUL_POLYGONS_POLYGONS_H__
