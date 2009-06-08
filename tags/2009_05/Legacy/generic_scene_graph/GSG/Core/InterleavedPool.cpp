
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 4004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A pool of 4D vectors.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/InterleavedPool.h"

using namespace GSG;

GSG_IMPLEMENT_REFERENCED ( InterleavedPool );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

InterleavedPool::InterleavedPool() : Referenced(), 
  _values(),
  _contains     ( 0 ),
  _vertexOffset ( 0 ),
  _normalOffset ( 0 ),
  _colorOffset  ( 0 )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

InterleavedPool::InterleavedPool ( const InterleavedPool &p ) : Referenced ( p ), 
  _values       ( p._values       ),
  _contains     ( p._contains     ),
  _vertexOffset ( p._vertexOffset ),
  _normalOffset ( p._normalOffset ),
  _colorOffset  ( p._colorOffset  )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

InterleavedPool::~InterleavedPool()
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::numRows ( InterleavedPool::size_type rows )
{
  Lock lock ( this );
  _values.resize ( rows * this->numColumns() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reserve the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::reserveRows ( InterleavedPool::size_type rows )
{
  Lock lock ( this );
  _values.reserve ( rows * this->numColumns() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the values.
//
///////////////////////////////////////////////////////////////////////////////

InterleavedPool::const_reference InterleavedPool::value ( InterleavedPool::size_type i ) const
{
  BoundsChecker ( _values.size(), i );
  return _values[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the values.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::value ( InterleavedPool::size_type i, InterleavedPool::value_type v )
{
  Lock lock ( this );
  BoundsChecker ( _values.size(), i );
  _values[i] = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  What the values are.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::contains ( UnsignedInteger bits )
{
  Lock lock ( this );

  // Set the bits.
  _contains = bits;

  // This is always true. Keep it in case you ever add textures.
  _colorOffset = 0;

  // Initialize this too. It makes the logic below easier to read.
  _normalOffset = 0;

  // Calculate the offsets. 
  // Note: interleaved values are always ordered: colors, normals, vertices.
  // However, either normals, colors, or both might be missing.
  switch ( _contains )
  {
    case 0:

      _vertexOffset = 0;
      break;

    case NORMALS:

      _vertexOffset = 3;
      break;

    case COLORS3:

      _vertexOffset = 3;
      break;

    case COLORS4:

      _vertexOffset = 4;
      break;

    case ( COLORS3 | NORMALS ):

      _normalOffset = 3;
      _vertexOffset = 6;
      break;

    case ( COLORS4 | NORMALS ):

      _normalOffset = 4;
      _vertexOffset = 7;
      break;

    default:

      ErrorChecker ( false ); // Illegal combination.
      _normalOffset = 0;
      _vertexOffset = 0;
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::vertex ( InterleavedPool::size_type row, Real v0, Real v1, Real v2 )
{
  Lock lock ( this );
  size_type index ( row * this->numColumns() + _vertexOffset );
  BoundsChecker ( _values.size() - 2, index );
  _values[index++] = static_cast < value_type > ( v0 );
  _values[index++] = static_cast < value_type > ( v1 );
  _values[index  ] = static_cast < value_type > ( v2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::vertex ( InterleavedPool::size_type row, Real &v0, Real &v1, Real &v2 ) const
{
  size_type index ( row * this->numColumns() + _vertexOffset );
  BoundsChecker ( _values.size() - 2, index );
  v0 = static_cast < Real > ( _values[index++] );
  v1 = static_cast < Real > ( _values[index++] );
  v2 = static_cast < Real > ( _values[index  ] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the normals.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::normal ( InterleavedPool::size_type row, Real n0, Real n1, Real n2 )
{
  Lock lock ( this );
  size_type index ( row * this->numColumns() + _normalOffset );
  BoundsChecker ( _values.size() - 2, index );
  _values[index++] = static_cast < value_type > ( n0 );
  _values[index++] = static_cast < value_type > ( n1 );
  _values[index  ] = static_cast < value_type > ( n2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the normals.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::normal ( InterleavedPool::size_type row, Real &n0, Real &n1, Real &n2 ) const
{
  size_type index ( row * this->numColumns() + _normalOffset );
  BoundsChecker ( _values.size() - 2, index );
  n0 = static_cast < Real > ( _values[index++] );
  n1 = static_cast < Real > ( _values[index++] );
  n2 = static_cast < Real > ( _values[index  ] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::color ( InterleavedPool::size_type row, Real c0, Real c1, Real c2 )
{
  Lock lock ( this );
  size_type index ( row * this->numColumns() + _colorOffset );
  BoundsChecker ( _values.size() - 2, index );
  _values[index++] = static_cast < value_type > ( c0 );
  _values[index++] = static_cast < value_type > ( c1 );
  _values[index  ] = static_cast < value_type > ( c2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::color ( InterleavedPool::size_type row, Real &c0, Real &c1, Real &c2 ) const
{
  size_type index ( row * this->numColumns() + _colorOffset );
  BoundsChecker ( _values.size() - 2, index );
  c0 = static_cast < Real > ( _values[index++] );
  c1 = static_cast < Real > ( _values[index++] );
  c2 = static_cast < Real > ( _values[index  ] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::color ( InterleavedPool::size_type row, Real c0, Real c1, Real c2, Real c3 )
{
  Lock lock ( this );
  size_type index ( row * this->numColumns() + _colorOffset );
  BoundsChecker ( _values.size() - 3, index );
  _values[index++] = static_cast < value_type > ( c0 );
  _values[index++] = static_cast < value_type > ( c1 );
  _values[index++] = static_cast < value_type > ( c2 );
  _values[index  ] = static_cast < value_type > ( c3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void InterleavedPool::color ( InterleavedPool::size_type row, Real &c0, Real &c1, Real &c2, Real &c3 ) const
{
  size_type index ( row * this->numColumns() + _colorOffset );
  BoundsChecker ( _values.size() - 3, index );
  c0 = static_cast < Real > ( _values[index++] );
  c1 = static_cast < Real > ( _values[index++] );
  c2 = static_cast < Real > ( _values[index++] );
  c3 = static_cast < Real > ( _values[index  ] );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set from the given object.
//
/////////////////////////////////////////////////////////////////////////////

void InterleavedPool::setFrom ( const InterleavedPool &p )
{
  Lock lock ( this );

  // Set the members.
  _values       = p._values;
  _contains     = p._contains;
  _vertexOffset = p._vertexOffset;
  _normalOffset = p._normalOffset;
  _colorOffset  = p._colorOffset;

  // Call the base class's function.
  BaseClass::setFrom ( p );
}
