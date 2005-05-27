
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_CANCEL_BUTTON_H___
#define __USUL_RESOURCES_CANCEL_BUTTON_H___

#include "Usul/Export/Export.h"

namespace Usul { namespace Interfaces { struct ICancelButton; } }


namespace Usul {
namespace Resources {


// Get/set pointer to cancel button
USUL_EXPORT Usul::Interfaces::ICancelButton*    cancelButton();
USUL_EXPORT void                                cancelButton ( Usul::Interfaces::ICancelButton * );


} // namespace Resources
} // namespace Usul




#endif // __USUL_RESOURCES_CANCEL_BUTTON_H___
