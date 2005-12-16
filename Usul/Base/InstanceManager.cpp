
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

#ifdef _MSC_VER
#include "windows.h"
#endif

#include <iostream>
#include <sstream>

using namespace Usul;
using namespace Usul::Base;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InstanceManager::InstanceManager() : _objects()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InstanceManager::~InstanceManager()
{
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
      for ( ObjectMap::const_iterator i = _objects.begin(); i != _objects.end(); ++i )
      {
        const ObjectMap::value_type value ( *i );
        const Referenced *address ( value.first );
        const std::string &typeName ( value.second );
        out << "\taddress = " << address << ", name = " << typeName << '\n';
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
//  Add a new object. Should not be in there already.
//
///////////////////////////////////////////////////////////////////////////////

void InstanceManager::add ( const Referenced *object )
{
  USUL_ASSERT ( 0x0 != object );
  if ( object )
  {
    const Result result ( _objects.insert ( ObjectMap::value_type ( object, object->typeId().name() ) ) );
    USUL_ASSERT ( true == result.second );
    USUL_ASSERT ( object == result.first->first );
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
