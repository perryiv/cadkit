
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Enumerations.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_ENUMERATIONS_H_
#define _CADKIT_INTERFACE_ENUMERATIONS_H_


namespace CadKit {


// Possible format attributes.
enum FormatAttribute
{
  FORMAT_ATTRIBUTE_ASCII,
  FORMAT_ATTRIBUTE_BINARY
};


// Message types.
enum MessageType
{
  MESSAGE_ERROR,
  MESSAGE_WARNING,
  MESSAGE_QUESTION,
  MESSAGE_INFO,
  MESSAGE_PROGRESS
};


// Possible types of vertex sets.
enum VertexSetType
{
  LINE_STRIP_SET,
  POINT_SET,
  POLYGON_SET,
  TRI_STRIP_SET,
  TRI_FAN_SET,
  UNKNOWN
};


// Possible vertex bindings.
enum VertexBinding
{
  BINDING_PER_VERTEX,     // One attribute for every vertex.
  BINDING_PER_PRIMITIVE,  // One attribute for every primitive.
  BINDING_OVERALL,        // One attribute for all the vertices.
  BINDING_OFF,            // No attribute.
  BINDING_UNDETERMINED    // Either unset or failed to determine.
};


// Possible options for processing LODs.
enum LodProcessOption
{
  PROCESS_ALL_LODS,
  PROCESS_HIGH_LOD,
  PROCESS_LOW_LOD,
};

// Possible primitives.
enum PrimitiveType
{
  BOX,
  CYLINDER,
  PYRAMID,
  SPHERE,
  TRIPRISM,
  UNKNOWN_PRIM
};


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_ENUMERATIONS_H_
