
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

GSG_IMPLEMENT_REFERENCED ( Factory );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : Referenced(),
  _attributes()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory ( const Factory &f ) : Referenced ( f ),
  _attributes ( f._attributes )
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


/////////////////////////////////////////////////////////////////////////////
//
//  Set from the given object.
//
/////////////////////////////////////////////////////////////////////////////

void Factory::setFrom ( const Factory &f )
{
  Lock lock ( this );

  // Set the members.
  _attributes = f._attributes;

  // Call the base class's function.
  BaseClass::setFrom ( f );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the attributes.
//
/////////////////////////////////////////////////////////////////////////////

void Factory::attributes ( AttributeSet *a )
{
  Lock lock ( this );
  _attributes = a;
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
/* redo... again
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
*/
  // Add the primitive set to a new shape.
  Shape::ValidPtr shape ( new Shape );
  //shape->append ( set );

  // Return the shape.
  return shape.release();
}


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
  const Vec3 &c, 
  Real r, 
  UnsignedInteger depth, 
  PrimitiveSet &prims )
{
  typedef PrimitiveSet::ValuePool ValuePool;
  typedef ValuePool::value_type value_type;

  // Get the pool.
  ValuePool::ValidPtr pool ( prims.values() );

  // The vertices are all a unit length from the center, 
  // so they are also the normal.
  if ( 0 == depth )
  {
#if 0
    const char *format = "%15.12f %15.12f %15.12f\n";
    ::printf ( format, x1, y1, z1 );
    ::printf ( format, x2, y2, z2 );
    ::printf ( format, x3, y3, z3 );
#endif

    // Make room in the pool.
    ValuePool::size_type index ( pool->numRows() );
    pool->numRows ( index + 3 );

    // Three new vertices (that are also normals).
    Vec3 p1 ( x1, y1, z1 );
    Vec3 p2 ( x2, y2, z2 );
    Vec3 p3 ( x3, y3, z3 );

    // Add the normals.
    pool->normal ( index,     p1[0], p1[1], p1[2] );
    pool->normal ( index + 1, p2[0], p2[1], p2[2] );
    pool->normal ( index + 2, p3[0], p3[1], p3[2] );

    // Scale by the radius. Do this first.
    p1 *= r;
    p2 *= r;
    p3 *= r;

    // Offset by the center. Do this second.
    p1 += c;
    p2 += c;
    p3 += c;

    // Add the vertices.
    pool->vertex ( index,     p1[0], p1[1], p1[2] );
    pool->vertex ( index + 1, p2[0], p2[1], p2[2] );
    pool->vertex ( index + 2, p3[0], p3[1], p3[2] );

    // Add the primitive.
    prims.append ( new Primitive ( index, 3 ) );

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
  subdivideSphere (  x1,  y1,  z1, x12, y12, z12, x31, y31, z31, c, r, depth, prims );
  subdivideSphere (  x2,  y2,  z2, x23, y23, z23, x12, y12, z12, c, r, depth, prims );
  subdivideSphere (  x3,  y3,  z3, x31, y31, z31, x23, y23, z23, c, r, depth, prims );
  subdivideSphere ( x12, y12, z12, x23, y23, z23, x31, y31, z31, c, r, depth, prims );
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
  // Declare these constants used in the subdivision algorithm.
  const Real X ( static_cast < Real > ( 0.525731112119133606 ) );
  const Real Z ( static_cast < Real > ( 0.8506508083528655993 ) );

  // Make an interleaved pool.
  typedef PrimitiveSet::ValuePool ValuePool;
  ValuePool::ValidPtr pool ( new ValuePool );
  pool->contains ( ValuePool::NORMALS );

  // Declare the primitive-set and attach the pool.
  PrimitiveSet::ValidPtr prims ( new PrimitiveSet );
  prims->type ( PrimitiveSet::TYPE_TRIANGLES );
  prims->normalBinding ( PrimitiveSet::BINDING_PER_VERTEX );
  prims->colorBinding  ( PrimitiveSet::BINDING_PER_NONE );
  prims->values ( pool );

  // Reserve space in the pool. It works out to 60 * 4 ^ n.
  const UnsignedInteger need ( 60 * GSG::Math::pow ( 4, n ) );
  pool->reserveRows ( need );

  // Call the sub-divider.
  Detail::subdivideSphere ( -X,  0,  Z,  X,  0,  Z,  0,  Z,  X, c, r, n, *prims );
  Detail::subdivideSphere ( -X,  0,  Z,  0,  Z,  X, -Z,  X,  0, c, r, n, *prims );
  Detail::subdivideSphere ( -Z,  X,  0,  0,  Z,  X,  0,  Z, -X, c, r, n, *prims );
  Detail::subdivideSphere (  0,  Z,  X,  Z,  X,  0,  0,  Z, -X, c, r, n, *prims );
  Detail::subdivideSphere (  0,  Z,  X,  X,  0,  Z,  Z,  X,  0, c, r, n, *prims );
  Detail::subdivideSphere (  Z,  X,  0,  X,  0,  Z,  Z, -X,  0, c, r, n, *prims );
  Detail::subdivideSphere (  Z,  X,  0,  Z, -X,  0,  X,  0, -Z, c, r, n, *prims );
  Detail::subdivideSphere (  0,  Z, -X,  Z,  X,  0,  X,  0, -Z, c, r, n, *prims );
  Detail::subdivideSphere (  0,  Z, -X,  X,  0, -Z, -X,  0, -Z, c, r, n, *prims );
  Detail::subdivideSphere ( -X,  0, -Z,  X,  0, -Z,  0, -Z, -X, c, r, n, *prims );
  Detail::subdivideSphere (  0, -Z, -X,  X,  0, -Z,  Z, -X,  0, c, r, n, *prims );
  Detail::subdivideSphere (  0, -Z, -X,  Z, -X,  0,  0, -Z,  X, c, r, n, *prims );
  Detail::subdivideSphere (  0, -Z, -X,  0, -Z,  X, -Z, -X,  0, c, r, n, *prims );
  Detail::subdivideSphere ( -Z, -X,  0,  0, -Z,  X, -X,  0,  Z, c, r, n, *prims );
  Detail::subdivideSphere ( -X,  0,  Z,  0, -Z,  X,  X,  0,  Z, c, r, n, *prims );
  Detail::subdivideSphere (  0, -Z,  X,  Z, -X,  0,  X,  0,  Z, c, r, n, *prims );
  Detail::subdivideSphere ( -Z,  X,  0, -Z, -X,  0, -X,  0,  Z, c, r, n, *prims );
  Detail::subdivideSphere ( -Z,  X,  0, -X,  0, -Z, -Z, -X,  0, c, r, n, *prims );
  Detail::subdivideSphere ( -Z,  X,  0,  0,  Z, -X, -X,  0, -Z, c, r, n, *prims );
  Detail::subdivideSphere (  0, -Z, -X, -Z, -X,  0, -X,  0, -Z, c, r, n, *prims );

  // Should be true.
  ErrorChecker ( pool->numRows() == pool->rowsReserved() );

  // Add the primitive-set to a new shape.
  Shape::ValidPtr shape ( new Shape );
  shape->append ( prims );

  // Set the shape's attributes.
  shape->attributes ( this->attibutes() );

  // Return the shape.
  return shape.release();
}
