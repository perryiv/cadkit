
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all type proxies.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Types/BaseProxy.h"

using namespace Usul::Types;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseProxy::BaseProxy ( const std::string &name, Mutex &mutex, TypeId type ) : BaseClass(),
  _name  ( name ),
  _mutex ( mutex ),
  _type  ( type )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseProxy::~BaseProxy()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name. No need to guard; it's never modified.
//
///////////////////////////////////////////////////////////////////////////////

std::string BaseProxy::name() const
{
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name. No need to guard; it's never modified.
//
///////////////////////////////////////////////////////////////////////////////

BaseProxy::TypeId BaseProxy::type() const
{
  return _type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

BaseProxy::Mutex &BaseProxy::mutex()
{
  return _mutex;
}
