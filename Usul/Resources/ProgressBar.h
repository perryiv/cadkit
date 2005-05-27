
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_PROGRESS_BAR_H___
#define __USUL_RESOURCES_PROGRESS_BAR_H___

#include "Usul/Export/Export.h"

namespace Usul { namespace Interfaces { struct IProgressBar; } }


namespace Usul {
namespace Resources {


// Get/set pointer to progress bar
USUL_EXPORT Usul::Interfaces::IProgressBar*    progressBar();
USUL_EXPORT void                               progressBar ( Usul::Interfaces::IProgressBar * );


} // namespace Resources
} // namespace Usul



#endif // __USUL_RESOURCES_PROGRESS_BAR_H___
