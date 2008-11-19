
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
//  Encapsulates database environment.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/Oracle/Environment.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"

#include "occi.h"

#include <iostream>
#include <stdexcept>

using namespace CadKit::Database::Oracle;


///////////////////////////////////////////////////////////////////////////////
//
//  File-scope variables.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  Mutex _mutex;
  Environment *_instance ( 0x0 );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the environment singleton.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct DestroyEnvironment
  {
    DestroyEnvironment()
    {
    }
    ~DestroyEnvironment()
    {
      CadKit::Database::Oracle::Environment::destroy();
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Environment::Environment() :
  _env ( 0x0 )
{
  USUL_TRY_BLOCK
  {
    _env = oracle::occi::Environment::createEnvironment ( oracle::occi::Environment::THREADED_MUTEXED );
  }
  catch ( const oracle::occi::SQLException &e )
  {
    // Even though oracle::occi::SQLException inherits from std::exception,
    // in order to get proper propagation of any exception information we 
    // have to catch and re-throw.
    throw std::runtime_error ( Usul::Strings::format
      ( "Error 1560224438: failed to create database environment. ",
        ( ( 0x0 != e.what() ) ? e.what() : "" ) ) );
  }
  if ( 0x0 == _env )
  {
    throw std::runtime_error ( "Error 8279841650: failed to create database environment" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Environment::~Environment()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Environment::_destroy ), "2385719611" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Environment::_destroy()
{
  if ( 0x0 != _env )
  {
    USUL_TRY_BLOCK
    {
      oracle::occi::Environment::terminateEnvironment ( _env );
    }
    catch ( const oracle::occi::SQLException &e )
    {
      std::cout << Usul::Strings::format
        ( "Error 2902903324: failed to terminate database environment. ",
        ( ( 0x0 != e.what() ) ? e.what() : "" ) ) << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the instance. Using the double-check pattern, which is not perfect.
//  http://www.devarticles.com/c/a/Cplusplus/C-plus-in-Theory-Why-the-Double-Check-Lock-Pattern-Isnt-100-ThreadSafe/
//
///////////////////////////////////////////////////////////////////////////////

Environment &Environment::instance()
{
  if ( 0x0 == Detail::_instance )
  {
    Detail::Guard guard ( Detail::_mutex );
    {
      if ( 0x0 == Detail::_instance )
      {
        Detail::_instance = new Environment;
      }
    }
  }
  return *Detail::_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the instance.
//
///////////////////////////////////////////////////////////////////////////////

void Environment::destroy()
{
  Detail::Guard guard ( Detail::_mutex );

  if ( 0x0 != Detail::_instance )
  {
    delete Detail::_instance;
    Detail::_instance = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the environment.
//
///////////////////////////////////////////////////////////////////////////////

oracle::occi::Environment *Environment::env()
{
  return _env;
}
