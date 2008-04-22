
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Mouse functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_FILE_FUNCTIONS_H_
#define _FOX_TOOLS_FILE_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"

namespace FX { struct FXEvent; };




namespace FoxTools {
namespace Functions {


// Get the mouse state from the event.
FOX_TOOLS_EXPORT unsigned int mouseState ( const FX::FXEvent *event );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_FILE_FUNCTIONS_H_
