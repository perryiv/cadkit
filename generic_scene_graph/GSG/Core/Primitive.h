
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single vertex primitive.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_
#define _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT Primitive : public Referenced
{
public:

  GSG_DECLARE_CLONE ( Primitive );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Primitive, Referenced );
  typedef Indices::size_type size_type;

  // The type of primitive.
  enum Type
  {
    TYPE_UNKNOWN,
    TYPE_POINTS,
    TYPE_LINES,
    TYPE_LINE_STRIP,
    TYPE_LINE_LOOP,
    TYPE_TRIANGLES,
    TYPE_TRIANGLE_STRIP,
    TYPE_TRIANGLE_FAN,
    TYPE_QUADS,
    TYPE_QUAD_STRIP,
    TYPE_POLYGON
  };

  // The kind of normal and color binding.
  enum Binding
  {
    BINDING_UNKNOWN,
    BINDING_PER_VERTEX,
    BINDING_PER_PRIMITIVE
  };

  // How to interpret the indices.
  enum Format
  {
    INDICES_UNKNOWN,
    INDICES_RANDOM, // Each index is for a single vertex.
    INDICES_SEQUENTIAL_OVERLAPPED, // start1, count1, start2, count2, ...
    INDICES_SEQUENTIAL_ADJACENT,   // count1, count2, ...
  };

  explicit Primitive();
  Primitive ( const Primitive &prim );

  // Set/get the type of primitives.
  void                    type ( Type t );
  Type                    type() const { return _type; }

  // Set/get the format of the indices.
  Format                  format() const { return _format; }
  void                    format ( Format f );

  // Access the vertices.
  const Indices &         vertices() const { return _vi; }
  Indices &               vertices()       { return _vi; }

  // Access the normals.
  const Indices &         normals() const { return _ni; }
  Indices &               normals()       { return _ni; }

  // Set/get the colors.
  const Indices &         colors() const { return _ci; }
  Indices &               colors()       { return _ci; }

  // Normal binding.
  Binding                 normalBinding() const { return _nb; }
  void                    normalBinding ( Binding nb );

  // Color binding.
  Binding                 colorBinding()  const { return _cb; }
  void                    colorBinding  ( Binding cb );

protected:

  virtual ~Primitive();

private:

  Binding _nb;
  Binding _cb;
  Indices _vi;
  Indices _ni;
  Indices _ci;
  Type _type;
  Format _vf;
  Format _nf;
  Format _cf; HERE... do you need a different index-format for each array?
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_
