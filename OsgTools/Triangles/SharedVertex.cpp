
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

SharedVertex::SharedVertex ( unsigned int index, unsigned int numTrianglesToReserve, unsigned int flags ) : 
  _index     ( index ),
  _triangles (),
  _flags     ( flags ),
  _ref       ( 0 )
{
  USUL_STATIC_ASSERT (  4 == sizeof ( _index        ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _flags        ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _ref          ) );
#ifdef _WIN32
  USUL_STATIC_ASSERT ( 16 == sizeof ( _triangles    ) );
  USUL_STATIC_ASSERT ( 24 == sizeof ( SharedVertex  ) ); // Why?
#else
  USUL_STATIC_ASSERT ( 12 == sizeof ( _triangles    ) );
  USUL_STATIC_ASSERT ( 20 == sizeof ( SharedVertex  ) ); // Why?
#endif

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

void SharedVertex::remove ( Triangle *t )
{
  _triangles.erase ( std::find ( _triangles.begin(), _triangles.end(), Triangle::RefPtr ( t ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the given triangle to the list.
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::add ( Triangle *t )
{
  _triangles.push_back ( t );
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
      if ( Usul::Bits::has ( _flags, MEMORY_POOL ) )
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
  return Usul::Bits::has<unsigned char, unsigned char> ( _flags, VISITED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visited flag
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::visited ( bool v )
{
  if ( v )
    _flags = Usul::Bits::add<unsigned char, unsigned char> ( _flags, VISITED );
  else
    _flags = Usul::Bits::remove<unsigned char, unsigned char> ( _flags, VISITED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this shared vertex on the edge?
//
///////////////////////////////////////////////////////////////////////////////

bool SharedVertex::onEdge() const
{
  return Usul::Bits::has<unsigned char, unsigned char> ( _flags, ON_EDGE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the on edge flag
//
///////////////////////////////////////////////////////////////////////////////

void SharedVertex::onEdge ( bool e )
{
  if ( e )
    _flags = Usul::Bits::add<unsigned char, unsigned char> ( _flags, ON_EDGE );
  else
    _flags = Usul::Bits::remove<unsigned char, unsigned char> ( _flags, ON_EDGE );
}
