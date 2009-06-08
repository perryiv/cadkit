
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_EVENT_QUEUE_H___
#define __USUL_RESOURCES_EVENT_QUEUE_H___


#include "Usul/Export/Export.h"

namespace Usul { namespace Interfaces { struct IFlushEvents; } }


namespace Usul {
namespace Resources {


// Get/set pointer to flush events
USUL_EXPORT Usul::Interfaces::IFlushEvents*    flushEvents();
USUL_EXPORT void                               flushEvents ( Usul::Interfaces::IFlushEvents * );


} // namespace Resources
} // namespace Usul



#endif // __USUL_RESOURCES_EVENT_QUEUE_H___
