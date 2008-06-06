
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to delete windows.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_DELETE_FUNCTIONS_H_
#define _FOX_TOOLS_DELETE_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"

namespace FX { class FXWindow; };


namespace FoxTools {
namespace Functions {


// Delete all the child windows.
FOX_TOOLS_EXPORT void deleteChildren ( FX::FXWindow *window );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_DELETE_FUNCTIONS_H_
