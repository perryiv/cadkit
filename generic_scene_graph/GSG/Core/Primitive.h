
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A vertex primitive.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_
#define _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_

#include "GSG/Core/Vec2Pool.h"
#include "GSG/Core/Vec3Pool.h"
#include "GSG/Core/Vec4Pool.h"


namespace GSG {


class GSG_CORE_EXPORT Primitive : public Referenced
{
public:

  GSG_DECLARE_CLONE ( Primitive );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Primitive, Referenced );
  typedef Vec3Pool VertexPool;
  typedef Vec3Pool NormalPool;
  typedef Vec4Pool ColorPool;
  typedef VertexPool::value_type vertex_type;
  typedef NormalPool::value_type normal_type;
  typedef ColorPool::value_type color_type;
  typedef Indices::size_type size_type;
  typedef GSG_PERMUTATION_ITERATOR ( VertexPool, Indices ) VertexIterator;
  typedef GSG_PERMUTATION_ITERATOR ( NormalPool, Indices ) NormalIterator;
  typedef GSG_PERMUTATION_ITERATOR ( ColorPool,  Indices ) ColorIterator;
  typedef GSG_CONST_PERMUTATION_ITERATOR ( VertexPool, Indices ) ConstVertexIterator;
  typedef GSG_CONST_PERMUTATION_ITERATOR ( NormalPool, Indices ) ConstNormalIterator;
  typedef GSG_CONST_PERMUTATION_ITERATOR ( ColorPool,  Indices ) ConstColorIterator;

  enum Type
  {
    UNKNOWN,
    TRI_STRIP,
    LINE_STRIP,
    POINTS,
    TRIANGLES,
  };

  explicit Primitive();
  Primitive ( const Primitive &prim );

  // Set/get the type of primitive this is.
  void                    type ( Type t );
  Type                    type() const;

  // Iterators to the vertices.
  ConstVertexIterator     beginVertices() const;
  VertexIterator          beginVertices();
  ConstVertexIterator     endVertices() const;
  VertexIterator          endVertices();

  // Iterators to the normals.
  ConstNormalIterator     beginNormals() const;
  NormalIterator          beginNormals();
  ConstNormalIterator     endNormals() const;
  NormalIterator          endNormals();

  // Iterators to the colors.
  ConstColorIterator      beginColors() const;
  ColorIterator           beginColors();
  ConstColorIterator      endColors() const;
  ColorIterator           endColors();

  // Access to the vectors.
  const vertex_type &     vertex ( Index i ) const;
  const normal_type &     normal ( Index i ) const;
  const color_type &      color  ( Index i ) const;

  // Get the vector.
  vertex_type &           vertex ( Index i );
  normal_type &           normal ( Index i );
  color_type &            color  ( Index i );

  // Get the number of vectors.
  size_type               numVertices() const;
  size_type               numNormals() const;
  size_type               numColors() const;

  // Set the indices.
  void                    vertexIndices ( const Indices &vi );
  void                    normalIndices ( const Indices &ni );
  void                    colorIndices  ( const Indices &ci );

  // Set the pools.
  void                    vertexPool ( VertexPool *vp );
  void                    normalPool ( NormalPool *np );
  void                    colorPool  ( ColorPool *cp );

  // Bounding sphere.
  virtual void            calculateBoundingSphere();
  const BoundingSphere &  boundingSphere() const;
  void                    boundingSphere ( const BoundingSphere &b );

protected:

  virtual ~Primitive();

private:

  Indices _vi;
  Indices _ni;
  Indices _ci;
  VertexPool::Ptr _vp;
  NormalPool::Ptr _np;
  ColorPool::Ptr _cp;
  Type _type;
  BoundingSphere _bound;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_
