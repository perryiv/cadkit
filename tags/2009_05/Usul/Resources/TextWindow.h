
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Shared text-window resource.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_UPDATE_TEXT_WINDOW_H___
#define __USUL_RESOURCES_UPDATE_TEXT_WINDOW_H___

#include "Usul/Export/Export.h"

#include <iosfwd>

namespace Usul { namespace Interfaces { struct IUpdateTextWindow; struct IUnknown; } }


namespace Usul {
namespace Resources {


// Get/set pointer to text-window interfaces.
USUL_EXPORT void                                  textWindow ( Usul::Interfaces::IUnknown * );
USUL_EXPORT Usul::Interfaces::IUpdateTextWindow * textWindowUpdate();


// Convenience functions for use with std::cout.
struct USUL_EXPORT TextWindow
{
  static std::ostream &endl  ( std::ostream &out );
  static std::ostream &flush ( std::ostream &out );
};


} // namespace Resources
} // namespace Usul


#endif // __USUL_RESOURCES_UPDATE_TEXT_WINDOW_H___
