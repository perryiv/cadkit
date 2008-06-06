
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
#include "Usul/Strings/Format.h"
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
  _imMutex ( 0x0 ),
  _objects(),
  _count ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to print output.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void print ( const std::string &s )
  {
    if ( false == s.empty() )
    {
#ifdef __APPLE__
      ::fprintf( stderr, "%s\n", s.c_str() );
      ::fflush( stderr );
#else
      std::cout << s << std::flush;
#endif
      
      #ifdef _MSC_VER
      ::OutputDebugString ( s.c_str() );
      #endif
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InstanceManager::~InstanceManager()
{
  // Delete the mutex.
  delete _imMutex; _imMutex = 0x0;

  // Safely...
  try
  {
    // If we have objects...
    if ( false == _objects.empty() )
    {
      #if _MSC_VER
      USUL_ASSERT ( 0 ); // FYI
      #endif

      Helper::print ( Usul::Strings::format ( "Error 3951994349: ", _objects.size(), " referenced items remain:\n" ) );
      while ( false == _objects.empty() )
      {
        const ObjectMap::value_type value ( *(_objects.begin()) );
        const Referenced *address ( value.first );

        std::ostringstream out;
        out << "\taddress = " << address;
        out << ", number = " << value.second.first;
        out << ", name = " << value.second.second;
        out << '\n';

        Helper::print ( out.str() );

        _objects.erase ( _objects.begin() );
      }
    }
  }

  // Catch all exceptions.
  catch ( ... )
  {
    Helper::print ( "Error 3974320743: exception caught when trying to examine remaining objects\n" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex. Create it if this is the first time.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Mutex& InstanceManager::_mutex()
{
  if ( 0x0 == _imMutex )
  {
    _imMutex = Mutex::create();
  }
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
    ObjectInfo info ( _count++, object->typeId().name() );
    ObjectMap::value_type value ( object, info );
    const Result result ( _objects.insert ( value ) );
    USUL_ASSERT ( true == result.second );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove this address from the set. Should be one occurance.
//
///////////////////////////////////////////////////////////////////////////////

void InstanceManager::remove ( const Referenced *object )
{
  USUL_ASSERT ( 0x0 != object );

  Guard guard ( this->_mutex() );
  ObjectMap::size_type num ( _objects.erase ( object ) );

  USUL_ASSERT ( 1 == num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the entry.
//
///////////////////////////////////////////////////////////////////////////////

void InstanceManager::update ( const Referenced *object )
{
  USUL_ASSERT ( 0x0 != object );
  if ( 0x0 != object )
  {
    Guard guard ( this->_mutex() );
    ObjectMap::iterator i ( _objects.find ( object ) );
    if ( _objects.end() == i )
    {
      this->add ( object );
    }
    else
    {
      i->second.second = object->typeId().name();
    }
  }
}
