
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get and set pointer to the OpenGL visual.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_OPEN_GL_VISUAL_FUNCTION_H_
#define _FOX_TOOLS_OPEN_GL_VISUAL_FUNCTION_H_

#include "FoxTools/Export/Export.h"

namespace FX { class FXGLVisual; };




namespace FoxTools {
namespace Functions {


// Get/set pointer to visual.
FOX_TOOLS_EXPORT FX::FXGLVisual *     visual();
FOX_TOOLS_EXPORT void                 visual ( FX::FXGLVisual * );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_OPEN_GL_VISUAL_FUNCTION_H_
