
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_STATUS_BAR_H___
#define __USUL_RESOURCES_STATUS_BAR_H___

#include "Usul/Export/Export.h"

namespace Usul { namespace Interfaces { struct IStatusBar; } }


namespace Usul {
namespace Resources {


// Get/set pointer to progress bar
USUL_EXPORT Usul::Interfaces::IStatusBar*    statusBar();
USUL_EXPORT void                             statusBar ( Usul::Interfaces::IStatusBar * );


} // namespace Resources
} // namespace Usul


#endif // __USUL_RESOURCES_STATUS_BAR_H___
