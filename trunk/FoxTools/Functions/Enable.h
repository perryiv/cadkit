
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to enable or disable windows.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_ENABLE_DISABLE_H_
#define _FOX_TOOLS_ENABLE_DISABLE_H_

#include "FoxTools/Export/Export.h"

namespace FX { class FXWindow; };


namespace FoxTools {
namespace Functions {


// Enable or disable the window.
FOX_TOOLS_EXPORT void enable ( bool state, FX::FXWindow *window );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_ENABLE_DISABLE_H_
