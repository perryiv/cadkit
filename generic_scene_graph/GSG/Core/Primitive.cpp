
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single vertex primitive.
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
  _start ( 0 ),
  _size  ( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::Primitive ( Index start, Index size ) : Referenced(), 
  _start ( start ),
  _size  ( size )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Primitive::Primitive ( const Primitive &s ) : Referenced ( s ), 
  _start ( s._start ),
  _size  ( s._size )
{
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
//  The start of the primitive.
//
/////////////////////////////////////////////////////////////////////////////

void Primitive::start ( Index s )
{
  Lock lock ( this );
  _start = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The number of vertices in the primitive.
//
///////////////////////////////////////////////////////////////////////////////

void Primitive::size ( Index s )
{
  Lock lock ( this );
  _size = s;
}
