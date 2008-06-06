
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlConstants: Typesafe constant definitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_CONSTANTS_H_
#define _CADKIT_SCENEGRAPH_OPENGL_CONSTANTS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlTypedefs.h"
# include "Standard/SlDeclareConst.h"
#endif


namespace CadKit
{
// Errors.
const SlResult    SG_GL_FAILED_TO_SET_OPENGL_RENDERING_CONTEXT          = SL_CONST_UINT64 ( 0x59e9f660341711d4 );
const SlResult    SG_GL_FAILED_DELETE_OPENGL_RENDERING_CONTEXT          = SL_CONST_UINT64 ( 0x59e9f662341711d4 );
const SlResult    SG_GL_FAILED_RELEASE_DEVICE_CONTEXT                   = SL_CONST_UINT64 ( 0x59e9f663341711d4 );
const SlResult    SG_GL_FAILED_GET_DEVICE_CONTEXT                       = SL_CONST_UINT64 ( 0x59e9f664341711d4 );
const SlResult    SG_GL_FAILED_CREATE_OPENGL_RENDERING_CONTEXT          = SL_CONST_UINT64 ( 0x59e9f665341711d4 );
const SlResult    SG_GL_FAILED_TO_SET_PIXEL_FORMAT                      = SL_CONST_UINT64 ( 0x4b8bc4a034ab11d4 );
const SlResult    SG_GL_FAILED_TO_SELECT_SYSTEM_FONT_FOR_DEVICE_CONTEXT = SL_CONST_UINT64 ( 0xa32f6ad034ba11d4 );
const SlResult    SG_GL_FAILED_TO_CREATE_QUADRIC                        = SL_CONST_UINT64 ( 0xc85e434034c811d4 );
const SlResult    SG_GL_FAILED_TO_SET_VIEWPORT                          = SL_CONST_UINT64 ( 0xe4090bd034d711d4 );
const SlResult    SG_GL_FAILED_TO_GET_VIEWPORT                          = SL_CONST_UINT64 ( 0x60e450d03efd11d4 );
const SlResult    SG_GL_FAILED_TO_GET_WINDOW_CLIENT_RECTANGLE           = SL_CONST_UINT64 ( 0x7ad8270034da11d4 );
const SlResult    SG_GL_FAILED_TO_GET_GROUP_BOUNDING_BOX                = SL_CONST_UINT64 ( 0x12d10db03d0f11d4 );
const SlResult    SG_GL_FAILED_TO_ALLOCATE_FEEDBACK_BUFFER              = SL_CONST_UINT64 ( 0x283e8e005eb811d4 );
const SlResult    SG_GL_FAILED_TO_ALLOCATE_PRIMITIVES_ARRAY             = SL_CONST_UINT64 ( 0x8068e8405fdb11d4 );
const SlResult    SG_GL_FAILED_TO_RENDER_FEEDBACK_BUFFER                = SL_CONST_UINT64 ( 0x41ba08d05f1c11d4 );
const SlResult    SG_GL_FAILED_TO_ESTIMATE_FEEDBACK_BUFFER_SIZE         = SL_CONST_UINT64 ( 0xc8e980e060c911d4 );
const SlResult    SG_GL_ESTIMATED_FEEDBACK_BUFFER_OVERFLOW              = SL_CONST_UINT64 ( 0xc8e980e160c911d4 );
const SlResult    SG_GL_UNEXPECTED_TOKEN_IN_FEEDBACK_BUFFER             = SL_CONST_UINT64 ( 0x6c29f3805f5111d4 );

const SlResult    SG_GL_FAILED_TO_RENDER_GROUP                          = SL_CONST_UINT64 ( 0x20f4c0503c2811d4 );
const SlResult    SG_GL_FAILED_TO_RENDER_SCENE                          = SL_CONST_UINT64 ( 0x59e9f661341711d4 );
const SlResult    SG_GL_FAILED_TO_RENDER_SELECTION                      = SL_CONST_UINT64 ( 0x20f4c0513c2811d4 );
const SlResult    SG_GL_FAILED_TO_RENDER_SEPARATOR                      = SL_CONST_UINT64 ( 0x20f4c0523c2811d4 );
};

#endif
