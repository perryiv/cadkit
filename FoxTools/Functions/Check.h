
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to set the window's check.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_TOGGLE_CHECK_H_
#define _FOX_TOOLS_TOGGLE_CHECK_H_

#include "FoxTools/Export/Export.h"

namespace FX { class FXObject; };


namespace FoxTools {
namespace Functions {


// Set the check if possible.
FOX_TOOLS_EXPORT void check ( bool state, FX::FXObject *object );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_TOGGLE_CHECK_H_
