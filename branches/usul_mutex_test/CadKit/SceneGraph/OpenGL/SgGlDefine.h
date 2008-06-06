
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
