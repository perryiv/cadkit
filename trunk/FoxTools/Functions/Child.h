
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

namespace FX { class FXObject; };


namespace FoxTools {
namespace Functions {


// Get the number this window is in its parent's list of children.
FOX_TOOLS_EXPORT unsigned int whichChild ( const FX::FXObject *object );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_CHILD_FUNCTIONS_H_
