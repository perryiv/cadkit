
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
#include "GSG/Core/Primitive.h"
#include "GSG/Core/Vec3Pool.h"
#include "GSG/Core/Vec4Pool.h"
#include "GSG/Core/Container.h"

#include "boost/mpl/assert_is_same.hpp"

using namespace GSG;

GSG_IMPLEMENT_CLONE ( Factory );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : Referenced(),
  _vp ( 0x0 ),
  _np ( 0x0 ),
  _cp ( 0x0 )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory ( const Factory &node ) : Referenced ( node ),
  _vp ( 0x0 ),
  _np ( 0x0 ),
  _cp ( 0x0 )
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
  VertexPool::ValidPtr pvp ( this->_getVertexPool() );
  NormalPool::ValidPtr pnp ( this->_getNormalPool() );
  VertexPool &vp = *pvp;
  NormalPool &np = *pnp;

  // For convenience.
  const Real half ( static_cast < Real > ( 0.5 ) );
  Vec3 hs ( s[0] * half, s[1] * half, s[2] * half );

  // Set the vertices.
  SizeType v0 ( vp [ Vec3 ( c[0] - hs[0], c[1] - hs[1], c[2] - hs[2] ) ] );
  SizeType v1 ( vp [ Vec3 ( c[0] + hs[0], c[1] - hs[1], c[2] - hs[2] ) ] );
  SizeType v2 ( vp [ Vec3 ( c[0] - hs[0], c[1] + hs[1], c[2] - hs[2] ) ] );
  SizeType v3 ( vp [ Vec3 ( c[0] + hs[0], c[1] + hs[1], c[2] - hs[2] ) ] );
  SizeType v4 ( vp [ Vec3 ( c[0] - hs[0], c[1] - hs[1], c[2] + hs[2] ) ] );
  SizeType v5 ( vp [ Vec3 ( c[0] + hs[0], c[1] - hs[1], c[2] + hs[2] ) ] );
  SizeType v6 ( vp [ Vec3 ( c[0] - hs[0], c[1] + hs[1], c[2] + hs[2] ) ] );
  SizeType v7 ( vp [ Vec3 ( c[0] + hs[0], c[1] + hs[1], c[2] + hs[2] ) ] );

  // Set the normals.
  SizeType FRONT  ( np [ Vec3 (  0,  0,  1 ) ] );
  SizeType RIGHT  ( np [ Vec3 (  1,  0,  0 ) ] );
  SizeType BACK   ( np [ Vec3 (  0,  0, -1 ) ] );
  SizeType LEFT   ( np [ Vec3 ( -1,  0,  0 ) ] );
  SizeType TOP    ( np [ Vec3 (  0,  1,  0 ) ] );
  SizeType BOTTOM ( np [ Vec3 (  0, -1,  0 ) ] );

  // Make the two primitives. They will be tri-strips.
  Primitive::ValidPtr ltr ( new Primitive ); // Left-Top-Right
  Primitive::ValidPtr fbb ( new Primitive ); // Front-Bottom-Back

  // They are tri-strips.
  ltr->type ( Primitive::TRI_STRIP );
  fbb->type ( Primitive::TRI_STRIP );

  // Allocate the indices.
  Indices vi, ni;
  vi.resize ( 8 );
  ni.resize ( 3 );

  // Set the vertex and normal indices for the Left-Top-Right tri-strip.
  GSG::reference ( vi, 0 ) = v0;
  GSG::reference ( vi, 1 ) = v4;
  GSG::reference ( vi, 2 ) = v2;
  GSG::reference ( vi, 3 ) = v6; GSG::reference ( ni, 0 ) = LEFT;
  GSG::reference ( vi, 4 ) = v3;
  GSG::reference ( vi, 5 ) = v7; GSG::reference ( ni, 1 ) = TOP;
  GSG::reference ( vi, 6 ) = v1;
  GSG::reference ( vi, 7 ) = v5; GSG::reference ( ni, 2 ) = RIGHT;

  // Set the primitive's data. The indices are copied.
  ltr->vertexIndices ( vi );
  ltr->normalIndices ( ni );
  ltr->vertexPool ( pvp );
  ltr->normalPool ( pnp );

  // Now set the Front-Bottom-Back tri-strip.
  GSG::reference ( vi, 0 ) = v2;
  GSG::reference ( vi, 1 ) = v3;
  GSG::reference ( vi, 2 ) = v0;
  GSG::reference ( vi, 3 ) = v1; GSG::reference ( ni, 0 ) = FRONT;
  GSG::reference ( vi, 4 ) = v4;
  GSG::reference ( vi, 5 ) = v5; GSG::reference ( ni, 1 ) = BOTTOM;
  GSG::reference ( vi, 6 ) = v6;
  GSG::reference ( vi, 7 ) = v7; GSG::reference ( ni, 2 ) = BACK;

  // Now set the other primitive's data. The indices are copied.
  fbb->vertexIndices ( vi );
  fbb->normalIndices ( ni );
  fbb->vertexPool ( pvp );
  fbb->normalPool ( pnp );

  // Add these two primitives to a new shape.
  Shape::ValidPtr shape ( new Shape );
  shape->append ( ltr );
  shape->append ( fbb );

  // Return the shape.
  return shape.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a vertex pool. Make a new one if you have to.
//
///////////////////////////////////////////////////////////////////////////////

VertexPool *Factory::_getVertexPool()
{
  Lock lock ( this );

  if ( _vp.valid() )
    return _vp.get();

  this->setVertexPool ( new VertexPool );
  return _vp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a normal pool. Make a new one if you have to.
//
///////////////////////////////////////////////////////////////////////////////

NormalPool *Factory::_getNormalPool()
{
  Lock lock ( this );

  if ( _np.valid() )
    return _np.get();

  this->setNormalPool ( new NormalPool );
  return _np.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a color pool. Make a new one if you have to.
//
///////////////////////////////////////////////////////////////////////////////

ColorPool *Factory::_getColorPool()
{
  Lock lock ( this );

  if ( _cp.valid() )
    return _cp.get();

  this->setColorPool ( new ColorPool );
  return _cp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex pool.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::setVertexPool ( VertexPool *vp )
{
  Lock lock ( this );
  _vp = vp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the normal pool.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::setNormalPool ( NormalPool *np )
{
  Lock lock ( this );
  _np = np;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color pool.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::setColorPool ( ColorPool *cp )
{
  Lock lock ( this );
  _cp = cp;
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
