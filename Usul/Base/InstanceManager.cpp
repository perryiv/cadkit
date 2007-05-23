
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal class used for debugging.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Base/InstanceManager.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Threads/Guard.h"

#ifdef _MSC_VER
# define NOMINMAX
# include <windows.h>
#endif

#include <iostream>
#include <sstream>

using namespace Usul;
using namespace Usul::Base;


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Base
  {
    typedef Usul::Threads::Mutex Mutex;
    typedef Usul::Threads::Guard<Mutex> Guard;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InstanceManager::InstanceManager() : 
  _imMutex( 0x0 ),
  _objects()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InstanceManager::~InstanceManager()
{
  // Delete the mutex.
  if ( 0x0 != _imMutex )
    delete _imMutex;

  // Need this declared up here.
  std::ostringstream out;

  // Safely...
  try
  {
    // If we have objects...
    if ( false == _objects.empty() )
    {
      USUL_ASSERT ( 0 ); // FYI
      out << "Error 3951994349: " << _objects.size() << " referenced items remain:\n";
      while ( false == _objects.empty() )
      {
        const ObjectSet::value_type value ( *(_objects.begin()) );
        const Referenced *address ( value );
        out << "\taddress = " << address;
        if ( 0x0 != address )
          out << ", name = " << address->typeId().name();
        out << '\n';
        _objects.erase ( _objects.begin() );
      }
    }
  }

  // Catch all exceptions.
  catch ( ... )
  {
    out << "Error 3974320743: exception caught when trying to examine remaining objects\n";
  }

  // If the string has anything, print it.
  if ( false == out.str().empty() )
  {
    std::cout << out.str() << std::endl;
    #ifdef _MSC_VER
    ::OutputDebugString ( out.str().c_str() );
    ::OutputDebugString ( "\n" );
    #endif
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex. Create it if this is the first time.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Mutex& InstanceManager::_mutex()
{
  if( 0x0 == _imMutex )
    _imMutex = Mutex::create();

  return *_imMutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a new object. Should not be in there already.
//
///////////////////////////////////////////////////////////////////////////////

void InstanceManager::add ( const Referenced *object )
{
  Guard guard ( this->_mutex() );

  USUL_ASSERT ( 0x0 != object );
  if ( 0x0 != object )
  {
    const Result result ( _objects.insert ( object ) );
    USUL_ASSERT ( true == result.second );
    USUL_ASSERT ( object == *(result.first) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove this address from the set. Should be one occurance.
//
///////////////////////////////////////////////////////////////////////////////

void InstanceManager::remove ( const Referenced *object )
{
  Guard guard ( this->_mutex() );

  USUL_ASSERT ( 0x0 != object );
  USUL_ASSERT ( 1 == _objects.erase ( object ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the entry.
//
///////////////////////////////////////////////////////////////////////////////

void InstanceManager::update ( const Referenced *object )
{
  USUL_ASSERT ( 0x0 != object );
  this->remove ( object );
  this->add    ( object );
}
