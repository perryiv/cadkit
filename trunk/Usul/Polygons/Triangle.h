
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_TRIANGLES_H__
#define __USUL_POLYGONS_TRIANGLES_H__



namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Triangle Class
//
///////////////////////////////////////////////////////////////////////////////

class Triangle
{
public:
  typedef SharedVertex< Triangle > SharedVertex;
  

  Triangle() : 
  _index( 0 ), 
  _v1( 0x0 ), 
  _v2( 0x0 ),
  _v3( 0x0 ),
  _visited( false ) 
  {  }

  void setVertices( SharedVertex* v1, SharedVertex* v2, SharedVertex* v3 )
  {
    _v1 = v1;
    _v2 = v2;
    _v3 = v3;
  }

  void append( SharedVertex *v )
  {
    if( _v1 == 0x0 )
      _v1 = v;
    else if( _v2 == 0x0 )
      _v2 = v;
    else if( _v3 == 0x0 )
      _v3 = v;
  }

  SharedVertex * vertexOne()    { return _v1; }
  SharedVertex * vertexTwo()    { return _v2; }
  SharedVertex * vertexThree()  { return _v3; }

  const SharedVertex * vertexOne()   const  { return _v1; }
  const SharedVertex * vertexTwo()   const  { return _v2; }
  const SharedVertex * vertexThree() const  { return _v3; }
  
  //Has this triangle been visited?
  bool visited () const   { return _visited; }
  void visited ( bool v ) { _visited = v; }

  //Get the index
  unsigned int index() const           { return _index; }
  void         index( unsigned int i ) { _index = i; }

  //Get all the neighbors of this triangle
  std::list< Triangle* > getNeighbors() const
  {
    std::list< Triangle* > triangles;

    triangles.splice( triangles.begin(), _v1->getPolygons() );
    triangles.splice( triangles.end(),   _v2->getPolygons() );
    triangles.splice( triangles.end(),   _v3->getPolygons() );

    return triangles;
  }

private:
  unsigned int _index;
  SharedVertex * _v1;
  SharedVertex * _v2;
  SharedVertex * _v3;
  bool _visited;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Policy class to create triangles
//
///////////////////////////////////////////////////////////////////////////////

struct TriangleCreator
{
  template < class VertexSequence >
  void operator() ( VertexSequence& vertices, unsigned int number )
  {
  }

};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to walk triangles
//
///////////////////////////////////////////////////////////////////////////////

template < class IndexSequence >
struct TriangleFunctor
{
  typedef std::vector< TriangleFunctor > TodoStack;
  typedef Triangle::SharedVertex SharedVertex;

  //Construct with a triangle
  TriangleFunctor ( IndexSequence& answer, TodoStack& todoStack, Triangle *t ) :
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
      SharedVertex* v1 ( _triangle->vertexOne() );
      SharedVertex* v2 ( _triangle->vertexTwo() );
      SharedVertex* v3 ( _triangle->vertexThree() );

      //Have we visited v1 yet?
      if( !v1->visited() )
      {
        _todoStack.push_back( TriangleFunctor < IndexSequence > ( _answer, _todoStack, v1 ) );
        v1->visited( true );
      }

      //Have we visited v2 yet?
      if( !v2->visited() )
      {
        _todoStack.push_back( TriangleFunctor < IndexSequence > ( _answer, _todoStack, v2 ) );
        v2->visited( true );
      }

      //Have we visited v3 yet?
      if( !v3->visited() )
      {
        _todoStack.push_back( TriangleFunctor < IndexSequence > ( _answer, _todoStack, v3 ) );
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
          _todoStack.push_back( TriangleFunctor < IndexSequence > ( _answer, _todoStack, *triangle ) );
          (*triangle)->visited ( true );
        }
      }
    }
  }

  bool operator= ( const TriangleFunctor& that )
  {
    this->_answer = that._answer;
    this->_todoStack = that._todoStack;
    this->_triangle = that._triangle;
    this->_sharedVertex = that._sharedVertex;
    return true;
  }
private:
  IndexSequence &      _answer;
  TodoStack &          _todoStack;
  Triangle *           _triangle;
  SharedVertex *       _sharedVertex;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Test to see if 2 triangles are adjacent
//
///////////////////////////////////////////////////////////////////////////////

struct TriangleTest
{
#if 0
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
#endif

  bool operator () ( const Triangle* t1, const Triangle *t2 ) const
  {
    typedef Triangle::SharedVertex SharedVertex;
    unsigned int count ( 0 );

    const SharedVertex* t1v1 ( t1->vertexOne() );
    const SharedVertex* t1v2 ( t1->vertexTwo() );
    const SharedVertex* t1v3 ( t1->vertexThree() );

    const SharedVertex* t2v1 ( t2->vertexOne() );
    const SharedVertex* t2v2 ( t2->vertexTwo() );
    const SharedVertex* t2v3 ( t2->vertexThree() );

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

} //namespace Polygons

} //namespace Usul


#endif // __USUL_POLYGONS_TRIANGLES_H__
