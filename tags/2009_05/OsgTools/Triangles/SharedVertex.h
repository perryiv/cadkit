
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

#include "Usul/Config/Config.h"

#include "OsgTools/Triangles/Triangle.h"

#include <vector>


namespace OsgTools {
namespace Triangles {


class OSG_TOOLS_EXPORT SharedVertex
{
public:

  // Typedefs.
  typedef std::vector < Triangle::ValidRefPtr >   TriangleSequence;
  typedef TriangleSequence::iterator              TriangleItr;
  typedef TriangleSequence::const_iterator        ConstTriangleItr;
  typedef TriangleSequence::size_type             SizeType;
  typedef unsigned int                            IndexType;
  typedef unsigned char                           ReferenceCount;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SharedVertex );

  // Possible flags
  enum
  {
    VISITED      = 0x01,
    ON_EDGE      = 0x02,
    SELECTED     = 0x04,
    MEMORY_POOL  = 0x08,
    DIRTY_NORMAL = 0x10,
    DIRTY_COLOR  = 0x20,
    PROBLEM      = 0x40,
  };

  // Construction
  SharedVertex ( unsigned int index, unsigned int numTrianglesToReserve = 0, unsigned char flags = 0 );

  // Add the given triangle to the list.
  void                  addTriangle ( Triangle *t );

  // Iterators to the triangles.
  ConstTriangleItr      begin() const { return _triangles.begin(); }
  TriangleItr           begin()       { return _triangles.begin(); }

  // Return the current triangle capacity.
  unsigned int          capacity() const { return static_cast<unsigned int> ( _triangles.capacity() ); }

  // Set/get dirty flags.
  void                  dirtyColor ( bool );
  bool                  dirtyColor() const;

  // Set/get dirty flags.
  void                  dirtyNormal ( bool );
  bool                  dirtyNormal() const;

  // Iterators to the triangles.
  ConstTriangleItr      end() const { return _triangles.end(); }
  TriangleItr           end()       { return _triangles.end(); }

  // Set/get the index.
  void                  index ( IndexType i ) { _index = i; }
  IndexType             index() const { return _index; }

  // Get the number of triangles.
  SizeType              numTriangles() const { return _triangles.size(); }

  // Get/set on edge flag
  bool                  onEdge() const;
  void                  onEdge ( bool e );

  // Set/get the flags that says there was some kind of problem.
  bool                  problem() const;
  void                  problem ( bool e );

  // Reference this instance.
  void                  ref();

  // Return the reference count.
  ReferenceCount        refCount() const { return _ref; }

  // Remove the given triangle from the list.
  void                  removeTriangle ( Triangle *t );
  void                  removeAllTriangles();

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

  // Use reference counting.
  ~SharedVertex();

private:

  // Do not copy.
  SharedVertex ( const SharedVertex & );
  SharedVertex &operator = ( const SharedVertex & );

  IndexType _index;
  TriangleSequence _triangles;
  unsigned char _flags;
  ReferenceCount _ref;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_SHARED_VERTEX_H_
