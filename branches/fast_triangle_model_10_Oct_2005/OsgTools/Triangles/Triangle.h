
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Triangle class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_H_
#define _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_H_

#include "OsgTools/Export.h"

#include "Usul/Pointers/Pointers.h"

#include <list>
#include <vector>
#include <set>

namespace OsgTools {
namespace Triangles {

class SharedVertex;

class OSG_TOOLS_EXPORT Triangle
{
public:

  // Typedefs.
  typedef unsigned char ReferenceCount;
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef unsigned int IndexType;
  typedef std::set < Triangle* > PolygonSet;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Triangle );

  // Possible flags
  enum
  {
    VISITED     = 0x01,
    ON_EDGE     = 0x02,
    DELETED     = 0x04,
    SELECTED    = 0x08,
    MEMORY_POOL = 0x10
  };

  // Construction
  Triangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, IndexType index, unsigned int flags = 0 );

  // Use reference counting. Needs to be public to work with boost::object_pool.
  ~Triangle();

  // Sets all vertices to null.
  void                        clear();

  // Get/Set the deleted flag
  bool                        deleted() const;
  void                        deleted ( bool );

  // Get the neighbors of this Triangle
  void                        getNeighbors ( PolygonSet& ) const;

  // Get/set this Triangle's index
  IndexType                   index() const         { return _index; }
  void                        index ( IndexType i ) { _index = i; }

  // Get/Set the onEdge flag
  bool                        onEdge() const;
  void                        onEdge ( bool b );

  // Reference this instance.
  void                        ref();

  // Return the reference count.
  ReferenceCount              refCount() const { return _ref; }

  // Unreference this instance.
  void                        unref ( bool allowDeletion = true );

  // Get shared vertices.
  const SharedVertex *        vertex0() const { return _v0; }
  const SharedVertex *        vertex1() const { return _v1; }
  const SharedVertex *        vertex2() const { return _v2; }
  SharedVertex *              vertex0()       { return _v0; }
  SharedVertex *              vertex1()       { return _v1; }
  SharedVertex *              vertex2()       { return _v2; }

  // Set the shared vertex.
  void                        vertex0 ( SharedVertex *v );
  void                        vertex1 ( SharedVertex *v );
  void                        vertex2 ( SharedVertex *v );

  //Get/Set the visited flag
  bool                        visited() const;
  void                        visited ( bool v );

private:

  // Do not copy.
  Triangle ( const Triangle & );
  Triangle &operator = ( const Triangle & );

  SharedVertex *_v0;
  SharedVertex *_v1;
  SharedVertex *_v2;
  IndexType _index;
  unsigned char _flags;
  ReferenceCount _ref;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_H_
