
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
