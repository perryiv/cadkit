
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
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_POLYGONS_H__
#define __USUL_POLYGONS_POLYGONS_H__

#include "Usul/Export/Export.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Polygons/SharedVertex.h"


namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Triangle Class
//
///////////////////////////////////////////////////////////////////////////////

class Triangle : public Usul::Base::Referenced
{
public:
  typedef SharedVertex< Triangle > SharedVertex;
  typedef typename SharedVertex::RefPtr SharedVertexPtr;
  typedef std::vector< SharedVertexPtr > Vertices;
  typedef typename Vertices::iterator Iterator;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Triangle );

  Triangle() : _index( 0 ), _vertices(), _visited( false ) {  }

  void setVertices( SharedVertex* v1, SharedVertex* v2, SharedVertex* v3 )
  {
    _v1 = v1;
    _v2 = v2;
    _v3 = v3;
  }
  
  //Has this triangle been visited?
  bool visited() { return _visited; }
  void visited( bool v ) { _visited = v; }

  //Get the index
  unsigned int index() { return _index; }
  void index( unsigned int i ) { _index = i; }

  //Not sure if this is needed
  unsigned int numVerts () const { return 3; }

protected:
  //Use ref counting
  virtual ~Triangle () {}

private:
  unsigned int _index;
  SharedVertexPtr _v1;
  SharedVertexPtr _v2;
  SharedVertexPtr _v3;
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

struct TriangleFunctor
{
};


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

} //namespace Polygons

} //namespace Usul


#endif // __USUL_POLYGONS_TRIANGLES_H__