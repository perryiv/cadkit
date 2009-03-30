
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

#include "Usul/Config/Config.h"
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
  typedef OsgTools::Triangles::SharedVertex SharedVertex; // Needed in templates.
  typedef unsigned int IndexType;
  typedef std::set < Triangle* > PolygonSet;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Triangle );

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
    ORIGINAL     = 0x80,
  };

  // Construction
  Triangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, IndexType index, unsigned char flags = 0 );

  // Sets all vertices to null.
  void                        clear();

  // Set/get dirty flags.
  void                        dirtyColor ( bool );
  bool                        dirtyColor() const;

  // Set/get dirty flags.
  void                        dirtyNormal ( bool );
  bool                        dirtyNormal() const;

  // Get the neighbors of this Triangle
  void                        getNeighbors ( PolygonSet& ) const;

  // Get/set this Triangle's index
  IndexType                   index() const         { return _index; }
  void                        index ( IndexType i ) { _index = i; }

  // Get/Set the onEdge flag
  bool                        onEdge() const;
  void                        onEdge ( bool );

  // Set/get the flags that says there was some kind of problem.
  bool                        problem() const;
  void                        problem ( bool );

  // Set/get the flags that says this triangle is original (e.g., read from file).
  bool                        original() const;
  void                        original ( bool );

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

protected:

  // Use reference counting.
  ~Triangle();

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
