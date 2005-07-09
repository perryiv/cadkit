
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Child functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_CHILD_FUNCTIONS_H_
#define _FOX_TOOLS_CHILD_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"

#include <vector>
#include <string>

namespace FX { class FXObject; class FXWindow; class FXMetaClass; };


namespace FoxTools {
namespace Functions {


// Find the i'th child of the given type and/or contains te given text.
//FOX_TOOLS_EXPORT FX::FXWindow *     findChild ( FX::FXWindow *window, unsigned int which, const FX::FXMetaClass *type, const std::string &text );

// Hide the i'th child of type.
//FOX_TOOLS_EXPORT void               hideChild ( FX::FXWindow *window, unsigned int which, const FX::FXMetaClass * );

// Get a vector of the children.
typedef std::vector<FX::FXWindow *> Children;
FOX_TOOLS_EXPORT void               getChildren ( FX::FXWindow *window, Children &v );
FOX_TOOLS_EXPORT Children           getChildren ( FX::FXWindow *window );

// Replace text in the i'th child that contains the string and is of type.
//FOX_TOOLS_EXPORT void               replaceText ( FX::FXWindow *window, const std::string &oldString, const std::string &newString, const FX::FXMetaClass *type = 0x0 );

// Get the number this window is in its parent's list of children.
FOX_TOOLS_EXPORT unsigned int       whichChild ( const FX::FXObject *object );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_CHILD_FUNCTIONS_H_
