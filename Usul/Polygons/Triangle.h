
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_TRIANGLES_H__
#define __USUL_POLYGONS_TRIANGLES_H__

#include "Usul/Polygons/Polygons.h"
#include "Usul/Errors/Assert.h"

#if 0
#include <sstream>
#include <windows.h>
#endif

namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Triangle Class
//
///////////////////////////////////////////////////////////////////////////////

template < class VertexType_ > class Triangle
{
public:
  typedef VertexType_ VertexType;
  typedef SharedVertex < Triangle, VertexType > SharedVertex;
  typedef typename SharedVertex::RefPtr SharedVertexPtr;

  //Constructor
  Triangle() : 
  _index( 0 ), 
  _v1( 0x0 ), 
  _v2( 0x0 ),
  _v3( 0x0 ),
  _visited( false ) 
  {  }

  //Set all three vertices
  void setVertices ( SharedVertex* v1, SharedVertex* v2, SharedVertex* v3 )
  {
    _v1 = v1;
    _v2 = v2;
    _v3 = v3;
  }

  //Append a shared vertex
  void append ( SharedVertex *v )
  {
    if( _v1 == 0x0 )
      _v1 = v;
    else if( _v2 == 0x0 )
      _v2 = v;
    else if( _v3 == 0x0 )
      _v3 = v;
  }

  SharedVertex * vertex1() { return _v1.get(); }
  SharedVertex * vertex2() { return _v2.get(); }
  SharedVertex * vertex3() { return _v3.get(); }

  const SharedVertex * vertex1() const { return _v1.get(); }
  const SharedVertex * vertex2() const { return _v2.get(); }
  const SharedVertex * vertex3() const { return _v3.get(); }
  
  //Has this triangle been visited?
  bool visited () const   { return _visited; }
  void visited ( bool v ) { _visited = v; }

  //Get/Set the index
  unsigned int index() const           { return _index; }
  void         index( unsigned int i ) { _index = i; }

  //Get all the neighbors of this triangle
  std::list< Triangle* > getNeighbors()
  {
    typedef std::list< Triangle* > Triangles;
    Triangles triangles;

    triangles.splice( triangles.begin(), _v1->polygons() );
    triangles.splice( triangles.end(),   _v2->polygons() );
    triangles.splice( triangles.end(),   _v3->polygons() );

    triangles.sort ( PolyLess< Triangle >() );

    triangles.unique ( PolyEqual < Triangle >() );

#if 0
    std::ostringstream os;
    os << std::endl;
    for( Triangles::const_iterator i = triangles.begin(); i != triangles.end(); ++i )
      os << (*i)->index() << " ";
    os << std::endl;
    ::OutputDebugString( os.str().c_str() );
#endif

    return triangles;
  }

  VertexType normal() const
  {
    USUL_ASSERT ( _v1 && _v2 && _v3 );

    const VertexType &v1 ( _v1->value() );
    const VertexType &v2 ( _v2->value() );
    const VertexType &v3 ( _v3->value() );

    const VertexType v12 ( v2 - v1 );
    const VertexType v13 ( v3 - v1 );

    return ( v12 ^ v13 );
  }

private:
  unsigned int _index;
  SharedVertexPtr _v1;
  SharedVertexPtr _v2;
  SharedVertexPtr _v3;
  bool _visited;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to walk triangles
//
///////////////////////////////////////////////////////////////////////////////

template < class IndexSequence, class Vertex >
struct TriangleFunctor
{
  typedef std::vector< TriangleFunctor > TodoStack;
  typedef typename Triangle<Vertex>::SharedVertex SharedVertex;
  typedef typename SharedVertex::Ptr SharedVertexPtr;

  //Construct with a triangle
  TriangleFunctor ( IndexSequence& answer, TodoStack& todoStack, Triangle<Vertex> *t ) :
  _answer( answer ),
  _todoStack( todoStack ),
  _triangle ( t ),
  _sharedVertex( 0x0 )
  { } 

  //Construct with a shared vertex
  TriangleFunctor ( IndexSequence& answer, TodoStack& todoStack, SharedVertex *sv ) :
  _answer( answer ),
  _todoStack( todoStack ),
  _triangle ( 0x0 ),
  _sharedVertex( sv )
  { }

  void operator() ()
  {
    //If we have a triangle...
    if ( _triangle )
    {
      SharedVertexPtr v1 ( _triangle->vertex1() );
      SharedVertexPtr v2 ( _triangle->vertex2() );
      SharedVertexPtr v3 ( _triangle->vertex3() );

      //Have we visited v1 yet?
      if( !v1->visited() )
      {
        _todoStack.push_back( TriangleFunctor < IndexSequence, Vertex > ( _answer, _todoStack, v1 ) );
        v1->visited( true );
      }

      //Have we visited v2 yet?
      if( !v2->visited() )
      {
        _todoStack.push_back( TriangleFunctor < IndexSequence, Vertex > ( _answer, _todoStack, v2 ) );
        v2->visited( true );
      }

      //Have we visited v3 yet?
      if( !v3->visited() )
      {
        _todoStack.push_back( TriangleFunctor < IndexSequence, Vertex > ( _answer, _todoStack, v3 ) );
        v3->visited( true );
      }
      
      //If we get here the polygon is connected, add polygon to answer
      _answer.push_back( _triangle->index() );
    }

    //If we have a shared vertex
    else if( _sharedVertex )
    {
      //Loop through the triangles of this shared vertex
      for(SharedVertex::Iterator triangle = _sharedVertex->begin(); triangle != _sharedVertex->end(); ++triangle)
      {
        //If we haven't visited yet
        if( !(*triangle)->visited() )
        {
          //Add the functor to the todo statck
          _todoStack.push_back( TriangleFunctor < IndexSequence, Vertex > ( _answer, _todoStack, *triangle ) );
          (*triangle)->visited ( true );
        }
      }
    }
  }

  bool operator= ( const TriangleFunctor& that )
  {
    this->_answer       = that._answer;
    this->_todoStack    = that._todoStack;
    this->_triangle     = that._triangle;
    this->_sharedVertex = that._sharedVertex;
    return true;
  }
private:
  IndexSequence &       _answer;
  TodoStack &           _todoStack;
  Triangle< Vertex > *  _triangle;
  SharedVertexPtr       _sharedVertex;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Test to see if 2 triangles are adjacent
//
///////////////////////////////////////////////////////////////////////////////

struct TriangleTest
{
  template < class TriangleType > bool operator () ( const TriangleType &t1, const TriangleType &t2 ) const
  {
    typedef typename TriangleType::SharedVertex SharedVertex;
    typedef typename const SharedVertex* SharedVertexPtr;
    unsigned int count ( 0 );

    SharedVertexPtr t1v1 ( t1.vertex1() );
    SharedVertexPtr t1v2 ( t1.vertex2() );
    SharedVertexPtr t1v3 ( t1.vertex3() );

    SharedVertexPtr t2v1 ( t2.vertex1() );
    SharedVertexPtr t2v2 ( t2.vertex2() );
    SharedVertexPtr t2v3 ( t2.vertex3() );

    if( t1v1 != t2v1 && t1v1 != t2v2 && t1v1 != t2v3 )
      ++count;
    if( t1v2 != t2v1 && t1v2 != t2v2 && t1v2 != t2v3 )
      ++count;
    if( count == 2 )
      return false;
    if( count == 0 )
      return true;
    if( t1v3 != t2v1 && t1v3 != t2v2 && t1v3 != t2v3 )
      ++count;
    if( count >= 2 )
      return false;
    return true;
  }
};


} // namespace Polygons
} // namespace Usul


#endif // __USUL_POLYGONS_TRIANGLES_H__
