
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get and set pointer to the application.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_APPLICATION_FUNCTION_H_
#define _FOX_TOOLS_APPLICATION_FUNCTION_H_

#include "FoxTools/Export/Export.h"

namespace FX { class FXApp; };




namespace FoxTools {
namespace Functions {


// Get/set pointer to main window.
FOX_TOOLS_EXPORT FX::FXApp *    application();
FOX_TOOLS_EXPORT void           application ( FX::FXApp * );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_APPLICATION_FUNCTION_H_
