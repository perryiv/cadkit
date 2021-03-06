
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/FindObject.h"

using namespace Minerva::Core::Visitors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FindObject::FindObject ( const ObjectID& objectID ) : BaseClass(),
  _objectID ( objectID ),
  _unknown ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FindObject::~FindObject()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a container.
//
///////////////////////////////////////////////////////////////////////////////

void FindObject::visit ( Minerva::Core::Data::Container &container )
{
  _unknown = container.find ( _objectID );
  
  // If we didn't find it, keep traversing.
  if ( false == _unknown.valid() )
    container.traverse ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the unknown (may be null).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr FindObject::object() const
{
  return _unknown;
}
