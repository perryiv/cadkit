
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get and set pointer to the main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MAIN_WINDOW_FUNCTION_H_
#define _FOX_TOOLS_MAIN_WINDOW_FUNCTION_H_

#include "FoxTools/Export/Export.h"

namespace FX { class FXMainWindow; };




namespace FoxTools {
namespace Functions {


// Get/set pointer to main window. Accessing null pointer will throw.
FOX_TOOLS_EXPORT FX::FXMainWindow *   mainWindow();
FOX_TOOLS_EXPORT void                 mainWindow ( FX::FXMainWindow * );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_MAIN_WINDOW_FUNCTION_H_
