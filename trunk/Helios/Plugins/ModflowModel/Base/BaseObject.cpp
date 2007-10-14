
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all classes in this library.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModflowModel/Base/BaseObject.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

using namespace Modflow::Base;

USUL_IMPLEMENT_TYPE_ID ( BaseObject );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( BaseObject, BaseObject::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

BaseObject::BaseObject ( const std::string &name ) : BaseClass()
{
  USUL_TRACE_SCOPE;
  BaseClass::name ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

BaseObject::~BaseObject()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BaseObject::_destroy ), "3119714390" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void BaseObject::_destroy()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces. Just plumbing.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *BaseObject::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
    return static_cast < Usul::Interfaces::IUnknown* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a string row.
//
///////////////////////////////////////////////////////////////////////////////

BaseObject::StringRow BaseObject::makeStringRow ( const std::string &s )
{
  StringRow row;
  row.push_back ( s );
  return row;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a string row.
//
///////////////////////////////////////////////////////////////////////////////

BaseObject::StringRow BaseObject::makeStringRow ( const std::string &a, const std::string &b )
{
  StringRow row;
  row.push_back ( a );
  row.push_back ( b );
  return row;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the class name.
//
///////////////////////////////////////////////////////////////////////////////

std::string BaseObject::className() const
{
  return this->typeId().name();
}
