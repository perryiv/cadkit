
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_UTILITY_FUNCTIONS_H_
#define _GENERIC_SCENE_GRAPH_UTILITY_FUNCTIONS_H_

#include "GSG/OpenGL/GlApi.h"

#include "GSG/Core/PrimitiveSet.h"


namespace GSG {
namespace Detail {


/////////////////////////////////////////////////////////////////////////////
//
//  Map PrimitiveSet::Type to the corresponding GLenum.
//
/////////////////////////////////////////////////////////////////////////////

inline GLenum primitiveType ( PrimitiveSet::Type type )
{
  switch ( type )
  {
    case PrimitiveSet::TYPE_POINTS:
                  return GL_POINTS;
    case PrimitiveSet::TYPE_LINES:
                  return GL_LINES;
    case PrimitiveSet::TYPE_LINE_STRIP:
                  return GL_LINE_STRIP;
    case PrimitiveSet::TYPE_LINE_LOOP:
                  return GL_LINE_LOOP;
    case PrimitiveSet::TYPE_TRIANGLES:
                  return GL_TRIANGLES;
    case PrimitiveSet::TYPE_TRIANGLE_STRIP:
                  return GL_TRIANGLE_STRIP;
    case PrimitiveSet::TYPE_TRIANGLE_FAN:
                  return GL_TRIANGLE_FAN;
    case PrimitiveSet::TYPE_QUADS:
                  return GL_QUADS;
    case PrimitiveSet::TYPE_QUAD_STRIP:
                  return GL_QUAD_STRIP;
    case PrimitiveSet::TYPE_POLYGON:
                  return GL_POLYGON;
    case PrimitiveSet::TYPE_UNKNOWN:
      ErrorChecker ( false );
      return 0;
    default:
      ErrorChecker ( false );
      return 0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Figure out the format for the interleaved array.
//
/////////////////////////////////////////////////////////////////////////////

inline GLenum interleavedFormat ( const InterleavedPool *pool )
{
  ErrorChecker ( 0x0 != pool );

  switch ( pool->contains() )
  {
    case ( InterleavedPool::NORMALS | InterleavedPool::COLORS4 ):

      return GL_C4F_N3F_V3F;

    case InterleavedPool::NORMALS:

      return GL_N3F_V3F;

    case InterleavedPool::COLORS3:

      return GL_C3F_V3F;

    default:

      ErrorChecker ( false );
      return 0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the binding is per-vertex. This also checks to see if there is 
//  a binding I am forgetting about.
//
/////////////////////////////////////////////////////////////////////////////

inline bool isPerVertexBinding ( PrimitiveSet::Binding binding )
{
  switch ( binding )
  {
    case PrimitiveSet::BINDING_PER_PRIMITIVE:

      return true;

    case PrimitiveSet::BINDING_PER_NONE:
    case PrimitiveSet::BINDING_PER_VERTEX:

      return false;

    case PrimitiveSet::BINDING_UNKNOWN:

      ErrorChecker ( false ); // Oops... somebody forgot to set this...
      return false;

    default:

      ErrorChecker ( false ); // What binding is this?
      return false;
  }
}


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_OPENGL_DRAW_H_
