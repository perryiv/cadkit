
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

#include "GSG/Core/Vec2Pool.h"
#include "GSG/Core/Vec3Pool.h"
#include "GSG/Core/Vec4Pool.h"
#include "GSG/Core/Primitive.h"


namespace GSG {


class GSG_CORE_EXPORT PrimitiveSet : public Referenced
{
public:

  GSG_DECLARE_CLONE ( PrimitiveSet );
  GSG_DECLARE_LOCAL_TYPEDEFS ( PrimitiveSet, Referenced );
  typedef GSG_BIDIRECTIONAL_CONTAINER < Primitive::ValidPtr > Primitives;
  typedef Vec3Pool VertexPool;
  typedef Vec3Pool NormalPool;
  typedef Vec4Pool ColorPool;
  typedef VertexPool::value_type vertex_type;
  typedef NormalPool::value_type normal_type;
  typedef ColorPool::value_type color_type;
  typedef Primitives::size_type size_type;

  explicit PrimitiveSet();
  PrimitiveSet ( const PrimitiveSet &prim );

  // Set/get the vertices.
  const VertexPool *      vertices() const { return _vp.get(); }
  VertexPool *            vertices()       { return _vp.get(); }
  void                    vertices ( VertexPool *vp );

  // Set/get the normals.
  const NormalPool *      normals() const { return _np.get(); }
  NormalPool *            normals()       { return _np.get(); }
  void                    normals ( NormalPool *np );

  // Set/get the colors.
  const ColorPool *       colors() const { return _cp.get(); }
  ColorPool *             colors()       { return _cp.get(); }
  void                    colors ( ColorPool *cp );

  // Get the number of primitives.
  size_type               numPrimitives() const { return _prims.size(); }

  // Bounding sphere.
  virtual void            calculateBoundingSphere();
  const BoundingSphere &  boundingSphere() const;
  void                    boundingSphere ( const BoundingSphere &b );

protected:

  virtual ~PrimitiveSet();

private:

  VertexPool::Ptr _vp;
  NormalPool::Ptr _np;
  ColorPool::Ptr _cp;
  Primitives _prims;
  BoundingSphere _bound;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_SET_H_
