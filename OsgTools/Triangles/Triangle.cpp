
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Triangle class.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/Triangle.h"
#include "OsgTools/Triangles/SharedVertex.h"

#include "Usul/MPL/StaticAssert.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Bits/Bits.h"

#include "Usul/Polygons/Predicates.h"

#include <limits>
#include <algorithm>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  For safely referencing and unreferencing a shared vertex. The header file 
//  for this class has a forward-declaration of SharedVertex.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > void reference ( T *v )
  {
    if ( v )
      v->ref();
  }
  template < class T > void unreference ( T *v )
  {
    if ( v )
      v->unref();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Triangle::Triangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index ) : 
  _v0    ( v0 ),
  _v1    ( v1 ),
  _v2    ( v2 ),
  _index ( index ),
  _flags ( 0  ),
  _ref   ( 0  )
{
  USUL_STATIC_ASSERT (  4 == sizeof ( _v0       ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _v1       ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _v2       ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _index    ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _flags    ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _ref      ) );
  USUL_STATIC_ASSERT ( 20 == sizeof ( Triangle  ) ); // Why?

  Detail::reference ( _v0 );
  Detail::reference ( _v1 );
  Detail::reference ( _v2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Triangle::~Triangle()
{
  USUL_ASSERT ( 0 == _ref );
  Detail::unreference ( _v0 );
  Detail::unreference ( _v1 );
  Detail::unreference ( _v2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sets all vertices to null.
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::clear()
{
  this->vertex0 ( 0x0 );
  this->vertex1 ( 0x0 );
  this->vertex2 ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::vertex0 ( SharedVertex *v )
{
  Detail::unreference ( _v0 );
  _v0 = v;
  Detail::reference ( _v0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::vertex1 ( SharedVertex *v )
{
  Detail::unreference ( _v1 );
  _v1 = v;
  Detail::reference ( _v1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::vertex2 ( SharedVertex *v )
{
  Detail::unreference ( _v2 );
  _v2 = v;
  Detail::reference ( _v2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::ref()
{
  if ( std::numeric_limits<ReferenceCount>::max() == _ref )
    throw std::range_error ( "Error 2842253439: Maximum reference count reached" );
  ++_ref;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::unref()
{
  USUL_ASSERT ( this );
  USUL_ASSERT ( _ref > 0 );
  if ( 0 == --_ref )
    delete this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Has this triangle been visited?
//
///////////////////////////////////////////////////////////////////////////////

bool Triangle::visited() const
{
  return Usul::Bits::has<unsigned char, unsigned char> ( _flags, VISITED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visited flag
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::visited ( bool v )
{
  if( v )
    _flags = Usul::Bits::add<unsigned char, unsigned char> ( _flags, VISITED );
  else
    _flags = Usul::Bits::remove<unsigned char, unsigned char> ( _flags, VISITED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this triangle's neighbors
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::getNeighbors( PolygonList& triangles ) const
{
  // Insert each shared vertex's list of polygons
  triangles.insert ( triangles.end(), _v0->begin(), _v0->end() );
  triangles.insert ( triangles.end(), _v1->begin(), _v1->end() );
  triangles.insert ( triangles.end(), _v2->begin(), _v2->end() );

  // Sort them in ascending order
  triangles.sort ( Usul::Polygons::PolyLess< Triangle >() );

  // Remove all unique polygons
  triangles.unique( Usul::Polygons::PolyEqual < Triangle >() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Has this triangle been deleted?
//
///////////////////////////////////////////////////////////////////////////////

bool Triangle::deleted() const
{
  return Usul::Bits::has<unsigned char, unsigned char> ( _flags, DELETED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the deleted flag
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::deleted( bool d )
{
  if( d )
    _flags = Usul::Bits::add<unsigned char, unsigned char> ( _flags, DELETED );
  else
    _flags = Usul::Bits::remove<unsigned char, unsigned char> ( _flags, DELETED );
}
