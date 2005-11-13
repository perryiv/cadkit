
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
//  SgGlDefine: Pound defines.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_POUND_DEFINES_H_
#define _CADKIT_SCENEGRAPH_OPENGL_POUND_DEFINES_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "SceneGraph/Core/SgDefine.h"
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Default materials.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_GL_DEFAULT_MATERIAL_FRONT_AMBIENT          SG_DEFAULT_MATERIAL_AMBIENT
#define SG_GL_DEFAULT_MATERIAL_FRONT_DIFFUSE          SG_DEFAULT_MATERIAL_DIFFUSE
#define SG_GL_DEFAULT_MATERIAL_FRONT_SPECULAR         SG_DEFAULT_MATERIAL_SPECULAR
#define SG_GL_DEFAULT_MATERIAL_FRONT_EMISSIVE         SG_DEFAULT_MATERIAL_EMISSIVE
#define SG_GL_DEFAULT_MATERIAL_FRONT_SHININESS        SG_DEFAULT_MATERIAL_SHININESS

#define SG_GL_DEFAULT_MATERIAL_BACK_AMBIENT           0.1f, 0.1f, 0.1f, 1.0f
#define SG_GL_DEFAULT_MATERIAL_BACK_DIFFUSE           0.2f, 0.2f, 0.2f, 1.0f
#define SG_GL_DEFAULT_MATERIAL_BACK_SPECULAR          SG_DEFAULT_MATERIAL_SPECULAR
#define SG_GL_DEFAULT_MATERIAL_BACK_EMISSIVE          SG_DEFAULT_MATERIAL_EMISSIVE
#define SG_GL_DEFAULT_MATERIAL_BACK_SHININESS         SG_DEFAULT_MATERIAL_SHININESS


///////////////////////////////////////////////////////////////////////////////
//
//  Default light.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_GL_DEFAULT_LIGHT_AMBIENT                   SG_DEFAULT_LIGHT_AMBIENT
#define SG_GL_DEFAULT_LIGHT_DIFFUSE                   SG_DEFAULT_LIGHT_DIFFUSE
#define SG_GL_DEFAULT_LIGHT_SPECULAR                  SG_DEFAULT_LIGHT_SPECULAR
#define SG_GL_DEFAULT_LIGHT_POSITION                  0.0f, 0.0f, 1.0f, 0.0f


///////////////////////////////////////////////////////////////////////////////
//
//  Default background color.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_GL_DEFAULT_BACKGROUND                      0.792156f, 0.913725f, 1.0f, 1.0f


#endif // _CADKIT_SCENEGRAPH_OPENGL_POUND_DEFINES_H_
