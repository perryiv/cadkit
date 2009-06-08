
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  VertexBindings: Possible vertex bindings.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_VERTEX_BINDINGS_H_
#define _CADKIT_INTERFACE_VERTEX_BINDINGS_H_

#include "IQueryArray.h"


namespace CadKit
{

// Possible bindings.
enum VertexBinding
{
  BINDING_PER_VERTEX,     // One attribute for every vertex.
  BINDING_PER_PRIMITIVE,  // One attribute for every primitive.
  BINDING_OVERALL,        // One attribute for all the vertices.
  BINDING_OFF,            // No attribute.
  BINDING_UNDETERMINED    // Either unset or failed to determine.
};


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_VERTEX_BINDINGS_H_
