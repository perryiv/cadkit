
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to work with menu buttons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_BUTTON_FUNCTIONS_H_
#define _FOX_TOOLS_MENU_BUTTON_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"

#include <string>

namespace FX { class FXToolBar; class FXObject; };


namespace FoxTools {
namespace Menu {


// Add a button to the tool-bar.
FOX_TOOLS_EXPORT void addButton ( FX::FXToolBar *parent, 
                                  const std::string &text, 
                                  unsigned int iconId, 
                                  FX::FXObject *target,
                                  unsigned int commandId,
                                  void *userData = 0x0 );

// Add a button to the tool-bar.
FOX_TOOLS_EXPORT void addButton ( FX::FXToolBar *parent, 
                                  const std::string &text, 
                                  FX::FXObject *target,
                                  unsigned int commandId,
                                  void *userData = 0x0 );


// Add a toggle button to the tool-bar.
FOX_TOOLS_EXPORT void addToggle ( FX::FXToolBar *parent, 
                                  const std::string &text, 
                                  unsigned int checkedIconId, 
                                  unsigned int uncheckedIconId, 
                                  FX::FXObject *target,
                                  unsigned int commandId,
                                  void *userdata = 0x0 );


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_BUTTON_FUNCTIONS_H_
