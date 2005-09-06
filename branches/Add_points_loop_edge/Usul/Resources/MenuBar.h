
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_MENU_BUTTON_H___
#define __USUL_RESOURCES_MENU_BUTTON_H___

#include "Usul/Export/Export.h"

namespace Usul { namespace Interfaces { struct IMenuBar; } }


namespace Usul {
namespace Resources {


// Get/set pointer to menu bar interface
USUL_EXPORT Usul::Interfaces::IMenuBar*    menuBar ();
USUL_EXPORT void                           menuBar ( Usul::Interfaces::IMenuBar * );


} // namespace Resources
} // namespace Usul




#endif // __USUL_RESOURCES_MENU_BUTTON_H___
