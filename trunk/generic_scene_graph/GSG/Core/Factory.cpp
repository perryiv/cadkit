
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A node factory.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Factory.h"
#include "GSG/Core/Shape.h"
#include "GSG/Core/Container.h"

#include "boost/mpl/assert_is_same.hpp"

using namespace GSG;

GSG_IMPLEMENT_CLONE ( Factory );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : Referenced()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory ( const Factory &f ) : Referenced ( f )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a unit cube.
//
///////////////////////////////////////////////////////////////////////////////

Shape *Factory::cube()
{
  // Make a cube.
  return this->cube ( Vec3 ( 0, 0, 0 ), 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a cube.
//
///////////////////////////////////////////////////////////////////////////////

Shape *Factory::cube ( const Vec3 &center, Real size )
{
  // Make a box.
  return this->box ( center, Vec3 ( size, size, size ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a box.      6--------7
//                  /|       /|
//                 / |      / |
//                /  4-----/--5
//               /  /     /  /
//              2--/-----3  /
//              | /      | /
//              |/       |/
//              0--------1
//
///////////////////////////////////////////////////////////////////////////////

Shape *Factory::box ( const Vec3 &c, const Vec3 &s )
{
  BOOST_MPL_ASSERT_IS_SAME ( VertexPool::size_type, VertexPool::size_type );
  typedef VertexPool::size_type SizeType;
  Lock lock ( this );

  // Make a vertex and normal pool.
  VertexPool::ValidPtr vp ( new VertexPool );
  NormalPool::ValidPtr np ( new NormalPool );

  // Set the starting spots.
  SizeType sv ( vp->values().size() );
  SizeType sn ( np->values().size() );

  // Now make room.
  vp->values().resize ( sv + 8 );
  np->values().resize ( sn + 6 );

  // For convenience.
  const Real half ( static_cast < Real > ( 0.5 ) );
  const Vec3 hs ( s[0] * half, s[1] * half, s[2] * half );

  // Set the vertices.
  const SizeType V0 ( sv++ );
  const SizeType V1 ( sv++ );
  const SizeType V2 ( sv++ );
  const SizeType V3 ( sv++ );
  const SizeType V4 ( sv++ );
  const SizeType V5 ( sv++ );
  const SizeType V6 ( sv++ );
  const SizeType V7 ( sv++ );
  vp->value ( V0, Vec3 ( c[0] - hs[0], c[1] - hs[1], c[2] - hs[2] ) );
  vp->value ( V1, Vec3 ( c[0] + hs[0], c[1] - hs[1], c[2] - hs[2] ) );
  vp->value ( V2, Vec3 ( c[0] - hs[0], c[1] + hs[1], c[2] - hs[2] ) );
  vp->value ( V3, Vec3 ( c[0] + hs[0], c[1] + hs[1], c[2] - hs[2] ) );
  vp->value ( V4, Vec3 ( c[0] - hs[0], c[1] - hs[1], c[2] + hs[2] ) );
  vp->value ( V5, Vec3 ( c[0] + hs[0], c[1] - hs[1], c[2] + hs[2] ) );
  vp->value ( V6, Vec3 ( c[0] - hs[0], c[1] + hs[1], c[2] + hs[2] ) );
  vp->value ( V7, Vec3 ( c[0] + hs[0], c[1] + hs[1], c[2] + hs[2] ) );

  // Set the normals.
  const SizeType FRONT  ( sn++ );
  const SizeType RIGHT  ( sn++ );
  const SizeType BACK   ( sn++ );
  const SizeType LEFT   ( sn++ );
  const SizeType TOP    ( sn++ );
  const SizeType BOTTOM ( sn++ );
  np->value ( FRONT,  Vec3 (  0,  0,  1 ) );
  np->value ( RIGHT,  Vec3 (  1,  0,  0 ) );
  np->value ( BACK,   Vec3 (  0,  0, -1 ) );
  np->value ( LEFT,   Vec3 ( -1,  0,  0 ) );
  np->value ( TOP,    Vec3 (  0,  1,  0 ) );
  np->value ( BOTTOM, Vec3 (  0, -1,  0 ) );

  // Get the starting spots for the indices.
  sv = vp->indices().size();
  sn = np->indices().size();

  // Make room for the indices.
  vp->indices().resize ( sv + 16 );
  np->indices().resize ( sn + 6 );

  // Set the vertex indices for the Left-Top-Right tri-strip.
  vp->index ( sv++, V0 );
  vp->index ( sv++, V4 );
  vp->index ( sv++, V2 );
  vp->index ( sv++, V6 );
  vp->index ( sv++, V3 );
  vp->index ( sv++, V7 );
  vp->index ( sv++, V1 );
  vp->index ( sv++, V5 );

  // Now the normal indices.
  np->index ( sn++, LEFT );
  np->index ( sn++, TOP );
  np->index ( sn++, RIGHT );

  // Now set the vertices for the Front-Bottom-Back tri-strip.
  vp->index ( sv++, V2 );
  vp->index ( sv++, V3 );
  vp->index ( sv++, V0 );
  vp->index ( sv++, V1 );
  vp->index ( sv++, V4 );
  vp->index ( sv++, V5 ); 
  vp->index ( sv++, V6 );
  vp->index ( sv++, V7 );

  // And the normals.
  np->index ( sn++, FRONT );
  np->index ( sn++, BOTTOM );
  np->index ( sn++, BACK );

  // Make the primitive set.
  PrimitiveSet::ValidPtr set ( new PrimitiveSet );

  // It is a set of tri-strips.
  set->type ( PrimitiveSet::TYPE_TRIANGLE_STRIP );

  // The indices are random.
  vp->format ( VertexPool::INDICES_RANDOM );
  np->format ( NormalPool::INDICES_RANDOM );

  // Set the primitive's data. The indices are copied.
  set->vertices ( vp );
  set->normals  ( np );

  // Add the primitive set to a new shape.
  Shape::ValidPtr shape ( new Shape );
  shape->append ( set );

  // Return the shape.
  return shape.release();
}

/*

I eliminated the map in Vec3Pool and moved the indices from the Primitive to VecPool.
Primitive is now PrimitiveSet, which holds multiple primitives, all of the same type.
This was all done to take advantage of OpenGL vertex arrays.
The below code relies on that map which is no longer in Vec3Pool.
It should be easy enough to make a local map and do the same thing.

///////////////////////////////////////////////////////////////////////////////
//
//  Sub-divide a sphere.
//
///////////////////////////////////////////////////////////////////////////////

namespace GSG {
namespace Detail {
void subdivideSphere (
  Real x1, Real y1, Real z1, 
  Real x2, Real y2, Real z2, 
  Real x3, Real y3, Real z3, 
  Real r,  UnsignedInteger depth, 
  VertexPool &vp, NormalPool &np, 
  Indices &vi, Indices &ni )
{
  // The vertices are all a unit length from the center, 
  // so they are also the normal.
  if ( 0 == depth )
  {
    Vec3 p1 ( x1, y1, z1 );
    Vec3 p2 ( x2, y2, z2 );
    Vec3 p3 ( x3, y3, z3 );

    // This will look for the vertex in the pool and append it if not 
    // found. The bracket operator returns the index of the vertex. This 
    // index is appended to the sequence of indices.
    ni.push_back ( np[p1] );
    ni.push_back ( np[p2] );
    ni.push_back ( np[p3] );

    // Do the same for the vertices.
    vi.push_back ( vp[p1*r] );
    vi.push_back ( vp[p2*r] );
    vi.push_back ( vp[p3*r] );
    return;
  }

  Real x12 ( x1 + x2 );
  Real x23 ( x2 + x3 );
  Real x31 ( x3 + x1 );

  Real y12 ( y1 + y2 );
  Real y23 ( y2 + y3 );
  Real y31 ( y3 + y1 );

  Real z12 ( z1 + z2 );
  Real z23 ( z2 + z3 );
  Real z31 ( z3 + z1 );

  const Real one ( static_cast < Real > ( 1 ) );
  Real d ( GSG::Math::sqrt ( x12 * x12 + y12 * y12 + z12 * z12 ) );
  ErrorChecker ( d != 0 );
  Real invd = one / d;
  x12 *= invd; y12 *= invd; z12 *= invd;

  d = GSG::Math::sqrt ( x23 * x23 + y23 * y23 + z23 * z23 );
  ErrorChecker ( d != 0 );
  invd = one / d;
  x23 *= invd; y23 *= invd; z23 *= invd;

  d = GSG::Math::sqrt ( x31 * x31 + y31 * y31 + z31 * z31 );
  ErrorChecker ( d != 0 );
  invd = one / d;
  x31 *= invd; y31 *= invd; z31 *= invd;

  --depth;
  subdivideSphere (  x1,  y1,  z1, x12, y12, z12, x31, y31, z31, r, depth, vp, np, vi, ni );
  subdivideSphere (  x2,  y2,  z2, x23, y23, z23, x12, y12, z12, r, depth, vp, np, vi, ni );
  subdivideSphere (  x3,  y3,  z3, x31, y31, z31, x23, y23, z23, r, depth, vp, np, vi, ni );
  subdivideSphere ( x12, y12, z12, x23, y23, z23, x31, y31, z31, r, depth, vp, np, vi, ni );
}
}; // namespace Detail
}; // namespace GSG


///////////////////////////////////////////////////////////////////////////////
//
//  Make a unit sphere at the origin.
//
///////////////////////////////////////////////////////////////////////////////

Shape *Factory::sphere ( UnsignedInteger numSubDivisions )
{
  Lock lock ( this );
  return this->sphere ( Vec3 ( 0, 0, 0 ), 1, numSubDivisions );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere.
//
///////////////////////////////////////////////////////////////////////////////

Shape *Factory::sphere ( const Vec3 &c, Real r, UnsignedInteger n )
{
  BOOST_MPL_ASSERT_IS_SAME ( VertexPool::size_type, VertexPool::size_type );
  typedef VertexPool::size_type SizeType;
  Lock lock ( this );

  // Make a vertex and normal pool.
  VertexPool::ValidPtr vp ( this->_getVertexPool() );
  NormalPool::ValidPtr np ( this->_getNormalPool() );

  // Declare these constants used in the subdivision algorithm.
  const Real X ( static_cast < Real > ( 0.525731112119133606 ) );
  const Real Z ( static_cast < Real > ( 0.8506508083528655993 ) );

  // Declare the indices.
  Indices vi, ni;

  // Call the sub-divider.
  Detail::subdivideSphere ( -X,  0,  Z,  X,  0,  Z,  0,  Z,  X, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -X,  0,  Z,  0,  Z,  X, -Z,  X,  0, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -Z,  X,  0,  0,  Z,  X,  0,  Z, -X, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0,  Z,  X,  Z,  X,  0,  0,  Z, -X, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0,  Z,  X,  X,  0,  Z,  Z,  X,  0, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  Z,  X,  0,  X,  0,  Z,  Z, -X,  0, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  Z,  X,  0,  Z, -X,  0,  X,  0, -Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0,  Z, -X,  Z,  X,  0,  X,  0, -Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0,  Z, -X,  X,  0, -Z, -X,  0, -Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -X,  0, -Z,  X,  0, -Z,  0, -Z, -X, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0, -Z, -X,  X,  0, -Z,  Z, -X,  0, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0, -Z, -X,  Z, -X,  0,  0, -Z,  X, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0, -Z, -X,  0, -Z,  X, -Z, -X,  0, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -Z, -X,  0,  0, -Z,  X, -X,  0,  Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -X,  0,  Z,  0, -Z,  X,  X,  0,  Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0, -Z,  X,  Z, -X,  0,  X,  0,  Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -Z,  X,  0, -Z, -X,  0, -X,  0,  Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -Z,  X,  0, -X,  0, -Z, -Z, -X,  0, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere ( -Z,  X,  0,  0,  Z, -X, -X,  0, -Z, r, n, *vp, *np, vi, ni );
  Detail::subdivideSphere (  0, -Z, -X, -Z, -X,  0, -X,  0, -Z, r, n, *vp, *np, vi, ni );

  // Make the primitive. It is a collection of individual triangles.
  Primitive::ValidPtr prim ( new Primitive );
  prim->type ( Primitive::TRIANGLES );

  // Set the primitive's data. The indices are copied.
  prim->vertexIndices ( vi );
  prim->normalIndices ( ni );
  prim->vertexPool ( vp );
  prim->normalPool ( np );

  // Add the primitive to a new shape.
  Shape::ValidPtr shape ( new Shape );
  shape->append ( prim );

  // Return the shape.
  return shape.release();
}

*/
