
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Denis V. Dorozkain
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Library class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Library.h"

#include "Usul/Errors/Assert.h"

using namespace Collision;
using namespace Collision::PQP;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Library, Library::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Library::Library() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Library::~Library()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Library::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Collision::Interfaces::ICollider::IID:
    return static_cast<Collision::Interfaces::ICollider*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<Collision::Interfaces::ICollider*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the objects collide.
//
///////////////////////////////////////////////////////////////////////////////

bool Library::isColliding ( ICollisionObject *object1, ICollisionObject *object2 ) const
{
  USUL_ASSERT ( 0 ); // TODO.
  return false;
}
