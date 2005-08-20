

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get and set pointer to flush events.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Resources/EventQueue.h"
#include "Usul/Interfaces/IFlushEvents.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Pointer to flush events.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Resources
  {
    namespace Detail
    {
      Usul::Interfaces::IFlushEvents::QueryPtr _flush;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get pointer to flush events. It may be null.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IFlushEvents *Usul::Resources::flushEvents()
{
  return Detail::_flush.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set pointer to flush events.
//
///////////////////////////////////////////////////////////////////////////////

void Usul::Resources::flushEvents ( Usul::Interfaces::IFlushEvents *flush )
{
  Detail::_flush = flush;
} 