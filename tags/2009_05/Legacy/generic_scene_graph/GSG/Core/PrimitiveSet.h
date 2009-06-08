
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A sequence of vertex primitives.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_SET_H_
#define _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_SET_H_

#include "GSG/Core/InterleavedPool.h"
#include "GSG/Core/Vec3Pool.h"
#include "GSG/Core/Vec4Pool.h"
#include "GSG/Core/Primitive.h"


namespace GSG {


class GSG_CORE_EXPORT PrimitiveSet : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( PrimitiveSet );
  GSG_DECLARE_LOCAL_TYPEDEFS ( PrimitiveSet, Referenced );
  typedef GSG_BIDIRECTIONAL_CONTAINER < Primitive::ValidPtr > Primitives;
  typedef Primitives::size_type size_type;
  typedef Primitives::iterator iterator;
  typedef Primitives::const_iterator const_iterator;
  typedef Primitives::reference reference;
  typedef Primitives::const_reference const_reference;
  typedef InterleavedPool ValuePool;
  typedef Vec3Pool NormalPool;
  typedef Vec4Pool ColorPool;

  // The type of all the primitives.
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
    BINDING_PER_NONE,
    BINDING_PER_VERTEX,
    BINDING_PER_PRIMITIVE
  };

  explicit PrimitiveSet();
  PrimitiveSet ( const PrimitiveSet &prim );

  // Iterators to the primitives.
  const_iterator          begin() const { return _prims.begin(); }
  iterator                begin()       { return _prims.begin(); }
  const_iterator          end() const   { return _prims.end(); }
  iterator                end()         { return _prims.end(); }

  // Add a primitive.
  void                    append  ( Primitive *p );
  void                    prepend ( Primitive *p );
  void                    insert  ( iterator beforeMe, Primitive *p );

  // The primitive type.
  Type                    type() const { return _type; }
  void                    type ( Type t );

  // Set/get the interleaved values.
  const ValuePool *       values() const { return _pool.get(); }
  ValuePool *             values()       { return _pool.get(); }
  void                    values ( ValuePool *pool );

  // Set/get the per-primitive normals. If the normals are in the 
  // interleaved array then this should be null.
  const NormalPool *      normals() const { return _ppn.get(); }
  NormalPool *            normals()       { return _ppn.get(); }
  void                    normals ( NormalPool * );

  // Set/get the per-primitive colors. If the colors are in the 
  // interleaved array then this should be null.
  const ColorPool *       colors() const { return _ppc.get(); }
  ColorPool *             colors()       { return _ppc.get(); }
  void                    colors ( ColorPool *pool );

  // Get the number of primitives.
  size_type               numPrimitives() const { return _prims.size(); }

  // Normal binding.
  Binding                 normalBinding() const { return _nb; }
  void                    normalBinding ( Binding nb );

  // Color binding.
  Binding                 colorBinding() const { return _cb; }
  void                    colorBinding ( Binding cb );

  // Bounding sphere.
  virtual void            calculateBoundingSphere();
  const BoundingSphere &  boundingSphere() const { return _bound; }
  void                    boundingSphere ( const BoundingSphere &b );

protected:

  virtual ~PrimitiveSet();

  void                    _grow ( const Primitive *prim, BoundingSphere &bound ) const;

private:

  ValuePool::Ptr _pool;
  NormalPool::Ptr _ppn;
  ColorPool::Ptr _ppc;
  Type _type;
  Binding _nb;
  Binding _cb;
  Primitives _prims;
  BoundingSphere _bound;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_SET_H_
