
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//

///////////////////////////////////////////////////////////////////////////////
//
//  StarSystemFactory class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystemComponent.h"

#include "Usul/Components/Factory.h"
#include "Usul/Trace/Trace.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this library was compiled in debug mode.
//
///////////////////////////////////////////////////////////////////////////////

extern "C" bool usul_is_debug_build()
{
  USUL_TRACE_SCOPE_STATIC;
  #ifdef _DEBUG
    return true;
  #else
    return false;
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a class factory.
//
///////////////////////////////////////////////////////////////////////////////

extern "C" Usul::Interfaces::IClassFactory *usul_get_class_factory()
{
  USUL_TRACE_SCOPE_STATIC;
  Usul::Interfaces::IClassFactory::ValidRefPtr factory ( new Usul::Components::Factory<StarSystemComponent>() );
  return factory.release();
}
