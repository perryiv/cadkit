
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Shared-vertex class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_SHARED_VERTEX_H_
#define _OPEN_SCENE_GRAPH_TOOLS_SHARED_VERTEX_H_

#include "OsgTools/Triangles/Triangle.h"

#include <vector>


namespace OsgTools {
namespace Triangles {


class OSG_TOOLS_EXPORT SharedVertex
{
public:

  // Typedefs.
  typedef std::vector < Triangle::ValidRefPtr > TriangleSequence;
  typedef TriangleSequence::iterator        TriangleItr;
  typedef TriangleSequence::const_iterator  ConstTriangleItr;
  typedef TriangleSequence::size_type       SizeType;
  typedef unsigned int                      IndexType;
  typedef unsigned char                     ReferenceCount;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SharedVertex );

  // Possible flags
  enum
  {
    VISITED  = 0x01,
    ON_EDGE  = 0x02,
    DELETED  = 0x04,
    SELECTED = 0x08
  };

  // Construction.
  SharedVertex ( unsigned int index, unsigned int numTrianglesToReserve = 0 );

  // Add the given triangle to the list.
  void                  add ( Triangle *t );

  // Iterators to the triangles.
  ConstTriangleItr      begin() const { return _triangles.begin(); }
  TriangleItr           begin()       { return _triangles.begin(); }

  // Return the current triangle capacity.
  unsigned int          capacity() const { return _triangles.capacity(); }

  // Iterators to the triangles.
  ConstTriangleItr      end() const { return _triangles.end(); }
  TriangleItr           end()       { return _triangles.end(); }

  // Get the index.
  IndexType             index() const { return _index; }

  // Get the number of triangles.
  SizeType              numTriangles() const { return _triangles.size(); }

  //Get/Set on edge flag
  bool                  onEdge() const;
  void                  onEdge( bool e );

  // Reference this instance.
  void                  ref();

  // Return the reference count.
  ReferenceCount        refCount() const { return _ref; }

  // Remove the given triangle from the list.
  void                  remove ( Triangle *t );

  // Reserve space for triangles.
  void                  reserve ( unsigned int num ) { _triangles.reserve ( num ); }

  // Unreference this instance.
  void                  unref ( bool allowDeletion = true );

  // Get/set the visited flag
  bool                  visited() const;
  void                  visited ( bool v );

  // Predicate for finding a shared vertex in an stl container.
  struct FindVertex
  {
    FindVertex ( SharedVertex *v ) : _vertex ( v ){}
    bool operator () ( const SharedVertex::ValidRefPtr &i ) const
    {
      return i == _vertex;
    }
  private:
    SharedVertex::ValidRefPtr _vertex;
  };


protected:

  // Do not copy.
  SharedVertex ( const SharedVertex & );
  SharedVertex &operator = ( const SharedVertex & );

  // Use reference counting.
  ~SharedVertex();

private:

  IndexType _index;
  TriangleSequence _triangles;
  unsigned char _flags;
  ReferenceCount _ref;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_SHARED_VERTEX_H_
