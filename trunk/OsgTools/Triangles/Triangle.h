
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


namespace OsgTools {
namespace Triangles {

class SharedVertex;

class OSG_TOOLS_EXPORT Triangle
{
public:

  // Typedefs.
  typedef unsigned char ReferenceCount;

  // Construction & destruction.
  Triangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2 );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Triangle );

  // Sets all vertices to null.
  void                        clear();

  // Reference this instance.
  void                        ref();

  // Return the reference count.
  ReferenceCount              refCount() const { return _ref; }

  // Unreference this instance.
  void                        unref();

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

protected:

  /// Do not copy.
  Triangle ( const Triangle & );
  Triangle &operator = ( const Triangle & );

  // Use reference counting.
  ~Triangle();

private:

  SharedVertex *_v0;
  SharedVertex *_v1;
  SharedVertex *_v2;
  unsigned char _flags;
  ReferenceCount _ref;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_H_
