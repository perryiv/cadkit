
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for a single attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Attribute.h"

using namespace GSG;

GSG_IMPLEMENT_REFERENCED ( Attribute );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Attribute::Attribute() : Referenced()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Attribute::Attribute ( const Attribute &a ) : Referenced ( a )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Attribute::~Attribute()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set from the given object.
//
/////////////////////////////////////////////////////////////////////////////

void Attribute::setFrom ( const Attribute &a )
{
  Lock lock ( this );

  // Call the base class's function.
  BaseClass::setFrom ( a );
}
