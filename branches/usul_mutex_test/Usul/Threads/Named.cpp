
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Container of named threads.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Named.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

Named *Named::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Named::Named() : 
  _mutex ( Usul::Threads::Mutex::create() ),
  _map   ()
{
  USUL_TRACE_SCOPE;
  this->set ( Names::INSTANCE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Named::~Named()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Named::_destroy ), "1962009420" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton construction.
//
///////////////////////////////////////////////////////////////////////////////

Named &Named::instance()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 == _instance )
  {
    _instance = new Named;
  }
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton destruction.
//
///////////////////////////////////////////////////////////////////////////////

void Named::clear()
{
  USUL_TRACE_SCOPE_STATIC;
  delete _instance;
  _instance = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Named::_destroy()
{
  USUL_TRACE_SCOPE;
  _map.clear();
  delete _mutex; _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

Named::Mutex &Named::mutex() const
{
  USUL_TRACE_SCOPE;
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the pair.
//
///////////////////////////////////////////////////////////////////////////////

void Named::add ( const std::string &name, unsigned long id )
{
  Guard guard ( this->mutex() );
  _map.insert ( ThreadMap::value_type ( name, id ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the id. Throws if the name does not exist.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Named::find ( const std::string &name ) const
{
  Guard guard ( this->mutex() );
  ThreadMap::const_iterator i ( _map.find ( name ) );
  if ( _map.end() == i )
  {
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 3765208242: Failed to find thread '", name, "' in map" );
  }
  return i->second;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the name exists.
//
///////////////////////////////////////////////////////////////////////////////

bool Named::has ( const std::string &name ) const
{
  Guard guard ( this->mutex() );
  ThreadMap::const_iterator i ( _map.find ( name ) );
  return ( _map.end() != i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the current thread is registered by the given name.
//
///////////////////////////////////////////////////////////////////////////////

bool Named::is ( const std::string &name ) const
{
  Guard guard ( this->mutex() );
  ThreadMap::const_iterator i ( _map.find ( name ) );
  return ( ( _map.end() != i ) ? ( i->second == Usul::Threads::currentThreadId() ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name of the current thread.
//
///////////////////////////////////////////////////////////////////////////////

void Named::set ( const std::string &name )
{
  // Do not lock, mutex is not recursive!
  this->add ( name, Usul::Threads::currentThreadId() );
}