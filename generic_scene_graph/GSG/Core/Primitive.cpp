
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A shape node.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Primitive.h"

using namespace GSG;

GSG_IMPLEMENT_CLONE ( Primitive );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::Primitive() : Referenced(), 
  _vi (),
  _ni (),
  _ci (),
  _vp ( 0x0 ),
  _np ( 0x0 ),
  _cp ( 0x0 ),
  _type ( UNKNOWN ),
  _bound()
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::Primitive ( const Primitive &s ) : Referenced ( s ), 
  _vi ( s._vi ),
  _ni ( s._ni ),
  _ci ( s._ci ),
  _vp ( s._vp ),
  _np ( s._np ),
  _cp ( s._cp ),
  _type ( s._type ),
  _bound ( s._bound )
{
  BOOST_MPL_ASSERT_IS_SAME ( size_type, VertexPool::size_type );
  BOOST_MPL_ASSERT_IS_SAME ( size_type, NormalPool::size_type );
  BOOST_MPL_ASSERT_IS_SAME ( size_type, ColorPool::size_type );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::~Primitive()
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::type ( Type type )
{
  Lock lock ( this );
  _type = type;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::Type Primitive::type() const
{
  return _type;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::vertexIndices ( const Indices &vi )
{
  Lock lock ( this );
  _vi = vi;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::normalIndices ( const Indices &ni )
{
  Lock lock ( this );
  _ni = ni;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::colorIndices ( const Indices &ci )
{
  Lock lock ( this );
  _ci = ci;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::vertexPool ( VertexPool *vp )
{
  Lock lock ( this );
  _vp = vp;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::normalPool ( NormalPool *np )
{
  Lock lock ( this );
  _np = np;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the vertex indices.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::colorPool  ( ColorPool *cp )
{
  Lock lock ( this );
  _cp = cp;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

const BoundingSphere &Primitive::boundingSphere() const
{
  return _bound;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::boundingSphere ( const BoundingSphere &b )
{
  Lock lock ( this );
  _bound = b;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Calculate the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::calculateBoundingSphere()
{
  Lock lock ( this );

  // Accumulate the bounding sphere for all of the vertices.
  Detail::GetBoundOfPoints < BoundingSphere, VertexPool > gbs ( _vp );
  gbs = std::for_each ( _vi.begin(), _vi.end(), gbs );
  this->boundingSphere ( gbs.get() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Iterators to the vertices.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::ConstVertexIterator Primitive::beginVertices() const
{
  ErrorChecker ( _vp.valid() && _vi.size() && _vp->size() );
  return ConstVertexIterator ( _vp->begin(), _vi.begin() );
}
Primitive::VertexIterator Primitive::beginVertices()
{
  ErrorChecker ( _vp.valid() && _vi.size() && _vp->size() );
  return VertexIterator ( _vp->begin(), _vi.begin() );
}
Primitive::ConstVertexIterator Primitive::endVertices() const
{
  ErrorChecker ( _vp.valid() && _vi.size() && _vp->size() );
  return ConstVertexIterator ( _vp->begin(), _vi.end() );
}
Primitive::VertexIterator Primitive::endVertices()
{
  ErrorChecker ( _vp.valid() && _vi.size() && _vp->size() );
  return VertexIterator ( _vp->begin(), _vi.end() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Iterators to the normals.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::ConstNormalIterator Primitive::beginNormals() const
{
  ErrorChecker ( _np.valid() && _ni.size() && _np->size() );
  return ConstNormalIterator ( _np->begin(), _ni.begin() );
}
Primitive::NormalIterator Primitive::beginNormals()
{
  ErrorChecker ( _np.valid() && _ni.size() && _np->size() );
  return NormalIterator ( _np->begin(), _ni.begin() );
}
Primitive::ConstNormalIterator Primitive::endNormals() const
{
  ErrorChecker ( _np.valid() && _ni.size() && _np->size() );
  return ConstNormalIterator ( _np->begin(), _ni.end() );
}
Primitive::NormalIterator Primitive::endNormals()
{
  ErrorChecker ( _np.valid() && _ni.size() && _np->size() );
  return NormalIterator ( _np->begin(), _ni.end() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Iterators to the colors.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::ConstColorIterator Primitive::beginColors() const
{
  ErrorChecker ( _cp.valid() && _ci.size() && _cp->size() );
  return ConstColorIterator ( _cp->begin(), _ci.begin() );
}
Primitive::ColorIterator Primitive::beginColors()
{
  ErrorChecker ( _cp.valid() && _ci.size() && _cp->size() );
  return ColorIterator ( _cp->begin(), _ci.begin() );
}
Primitive::ConstColorIterator Primitive::endColors() const
{
  ErrorChecker ( _cp.valid() && _ci.size() && _cp->size() );
  return ConstColorIterator ( _cp->begin(), _ci.end() );
}
Primitive::ColorIterator Primitive::endColors()
{
  ErrorChecker ( _cp.valid() && _ci.size() && _cp->size() );
  return ColorIterator ( _cp->begin(), _ci.end() );
}
