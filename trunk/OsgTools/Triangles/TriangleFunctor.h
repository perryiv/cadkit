
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_TRIANGLES_TRIANGLE_FUNCTOR_H__
#define __OSG_TOOLS_TRIANGLES_TRIANGLE_FUNCTOR_H__


namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to walk triangles
//
///////////////////////////////////////////////////////////////////////////////

template < class IndexSequence, class Triangle >
struct TriangleFunctor
{
  typedef TriangleFunctor < IndexSequence, Triangle > ThisType;
  typedef std::vector < TriangleFunctor > TodoStack;
  typedef typename Triangle::SharedVertex SharedVertex;
  typedef typename SharedVertex::RefPtr SharedVertexPtr;
  typedef typename SharedVertex::TriangleItr SharedVertexItr;

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

  //This handles a triangle or a shared vertex based on which one is valid
  void operator() ()
  {
    //If we have a triangle...
    if ( _triangle )
    {
      SharedVertexPtr v1 ( _triangle->vertex0() );
      SharedVertexPtr v2 ( _triangle->vertex1() );
      SharedVertexPtr v3 ( _triangle->vertex2() );

      //Have we visited v1 yet?
      if( !v1->visited() )
      {
        _todoStack.push_back( ThisType ( _answer, _todoStack, v1 ) );
        v1->visited( true );
      }

      //Have we visited v2 yet?
      if( !v2->visited() )
      {
        _todoStack.push_back( ThisType ( _answer, _todoStack, v2 ) );
        v2->visited( true );
      }

      //Have we visited v3 yet?
      if( !v3->visited() )
      {
        _todoStack.push_back( ThisType ( _answer, _todoStack, v3 ) );
        v3->visited( true );
      }
      
      //If we get here the polygon is connected, add polygon to answer
      _answer.push_back( _triangle->index() );
    }

    //If we have a shared vertex
    else if( _sharedVertex )
    {
      //Loop through the triangles of this shared vertex
      for(SharedVertexItr triangle = _sharedVertex->begin(); triangle != _sharedVertex->end(); ++triangle)
      {
        //If we haven't visited yet
        if( !(*triangle)->visited() )
        {
          //Add the functor to the todo statck
          _todoStack.push_back( ThisType ( _answer, _todoStack, *triangle ) );
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
  Triangle *            _triangle;
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
    typedef const SharedVertex* SharedVertexPtr;
    unsigned int count ( 0 );

    SharedVertexPtr t1v1 ( t1.vertex0() );
    SharedVertexPtr t1v2 ( t1.vertex1() );
    SharedVertexPtr t1v3 ( t1.vertex2() );

    SharedVertexPtr t2v1 ( t2.vertex0() );
    SharedVertexPtr t2v2 ( t2.vertex1() );
    SharedVertexPtr t2v3 ( t2.vertex2() );

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


#endif // __OSG_TOOLS_TRIANGLES_TRIANGLE_FUNCTOR_H__
