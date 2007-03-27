
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Shared-vertex class.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"

#include "Usul/MPL/StaticAssert.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Bits/Bits.h"

#include <algorithm>
#include <limits>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex::SharedVertex ( unsigned int index, unsigned int numTrianglesToReserve, unsigned char flags ) : 
  _index     ( index ),
  _triangles (),
  _flags     ( flags ),
  _ref       ( 0 )
{
  /*USUL_STATIC_ASSERT (  4 == sizeof ( _index        ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _flags        ) );
  //USUL_STATIC_ASSERT (  1 == sizeof ( _ref          ) );

#if defined ( _MSC_VER ) && _MSC_VER < 1400
  USUL_STATIC_ASSERT ( 16 == sizeof ( _triangles    ) );
  USUL_STATIC_ASSERT ( 24 == sizeof ( SharedVertex  ) ); // Why?
#elif defined ( _MSC_VER ) && _MSC_VER == 1400
#ifdef _DEBUG
  USUL_STATIC_ASSERT ( 20 == sizeof ( _triangles    ) );
  //USUL_STATIC_ASSERT ( 28 == sizeof ( SharedVertex  ) ); // Why?
#else
  USUL_STATIC_ASSERT ( 16 == sizeof ( _triangles    ) );
  USUL_STATIC_ASSERT ( 24 == sizeof ( SharedVertex  ) ); // Why?
#endif
#else
  USUL_STATIC_ASSERT ( 12 == sizeof ( _triangles    ) );
  USUL_STATIC_ASSERT ( 20 == sizeof ( SharedVertex  ) ); // Why?
  #endif*/

  // Reserve triangles.
  if ( numTrianglesToReserve > 0 )
    _triangles.reserve ( numTrianglesToReserve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex::~SharedVertex()
{
  USUL_ASSERT ( 0 == _ref );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the given triangle from the list.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::removeTriangle ( Triangle *t )
{
  _triangles.erase ( std::find ( _triangles.begin(), _triangles.end(), Triangle::RefPtr ( t ) ) );
  this->dirtyNormal ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the given triangle to the list.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::addTriangle ( Triangle *t )
{
#ifdef _DEBUG
  const unsigned int num ( std::count_if ( _triangles.begin(), _triangles.end(), TriangleSequence::value_type::IsEqual ( t ) ) );
  USUL_ASSERT ( 0 == num );
#endif
  _triangles.push_back ( t );
  this->dirtyNormal ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::ref()
{
  if ( std::numeric_limits<ReferenceCount>::max() == _ref )
    throw std::range_error ( "Error 1283382107: Maximum reference count reached" );
  ++_ref;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::unref ( bool allowDeletion )
{
  USUL_ASSERT ( this );
  USUL_ASSERT ( _ref > 0 );
  if ( 0 == --_ref )
  {
    if ( allowDeletion )
    {
      const unsigned int bit ( SharedVertex::MEMORY_POOL );
      if ( Usul::Bits::has ( _flags, bit ) )
      {
        _triangles.clear();
      }
      else
      {
        delete this;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Has this shared vertex been visited?
//
///////////////////////////////////////////////////////////////////////////////

bool SharedVertex::visited() const
{
  const unsigned int bit ( SharedVertex::VISITED );
  return Usul::Bits::has ( _flags, bit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visited flag
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::visited ( bool state )
{
  const unsigned int bit ( SharedVertex::VISITED );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this shared vertex on the edge?
//
///////////////////////////////////////////////////////////////////////////////

bool SharedVertex::onEdge() const
{
  const unsigned int bit ( SharedVertex::ON_EDGE );
  return Usul::Bits::has ( _flags, bit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the on edge flag
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::onEdge ( bool state )
{
  const unsigned int bit ( SharedVertex::ON_EDGE );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::removeAllTriangles()
{
  _triangles.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::dirtyNormal ( bool state )
{
  const unsigned int bit ( SharedVertex::DIRTY_NORMAL );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SharedVertex::dirtyNormal() const
{
  const unsigned int bit ( SharedVertex::DIRTY_NORMAL );
  return Usul::Bits::has ( _flags, bit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::dirtyColor ( bool state )
{
  const unsigned int bit ( SharedVertex::DIRTY_COLOR );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SharedVertex::dirtyColor() const
{
  const unsigned int bit ( SharedVertex::DIRTY_COLOR );
  return Usul::Bits::has ( _flags, bit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the problem flag.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::problem ( bool state )
{
  const unsigned int bit ( SharedVertex::PROBLEM );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the problem flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SharedVertex::problem() const
{
  const unsigned int bit ( SharedVertex::PROBLEM );
  return Usul::Bits::has ( _flags, bit );
}
