
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

/////////////////////////////////////////////////////////////////////////////
//
//  SgDefine.h
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_POUND_DEFINES_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_POUND_DEFINES_H_


/////////////////////////////////////////////////////////////////////////////
//
//  Some common colors.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_COLOR_BLACK                  0.0f,  0.0f,  0.0f
#define SG_COLOR_BLUE                   0.0f,  0.0f,  1.0f
#define SG_COLOR_YELLOW                 1.0f,  1.0f,  0.0f
#define SG_COLOR_RED                    1.0f,  0.0f,  0.0f
#define SG_COLOR_GREEN                  0.0f,  1.0f,  0.0f
#define SG_COLOR_PURPLE                 1.0f,  0.0f,  1.0f
#define SG_COLOR_WHITE                  1.0f,  1.0f,  1.0f
#define SG_COLOR_ORANGE                 0.5f,  0.5f,  0.0f


/////////////////////////////////////////////////////////////////////////////
//
//  Default material.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_DEFAULT_MATERIAL_AMBIENT     0.2f,  0.2f,  0.2f,  1.0f
#define SG_DEFAULT_MATERIAL_DIFFUSE     0.8f,  0.8f,  0.8f,  1.0f
#define SG_DEFAULT_MATERIAL_SPECULAR    0.0f,  0.0f,  0.0f,  1.0f
#define SG_DEFAULT_MATERIAL_EMISSIVE    0.0f,  0.0f,  0.0f,  1.0f
#define SG_DEFAULT_MATERIAL_SHININESS   0.2f


/////////////////////////////////////////////////////////////////////////////
//
//  Default light.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_DEFAULT_LIGHT_AMBIENT        1.0f,  1.0f,  1.0f,  1.0f
#define SG_DEFAULT_LIGHT_DIFFUSE        1.0f,  1.0f,  1.0f,  1.0f
#define SG_DEFAULT_LIGHT_SPECULAR       1.0f,  1.0f,  1.0f,  1.0f
#define SG_DEFAULT_LIGHT_DIRECTION      0.0f,  0.0f, -1.0f


/////////////////////////////////////////////////////////////////////////////
//
//  Default camera.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_DEFAULT_CAMERA_Z_NEAR                0.1f
#define SG_DEFAULT_CAMERA_Z_FAR              1000.0f
#define SG_DEFAULT_CAMERA_ASPECT_RATIO          1.0f
#define SG_DEFAULT_CAMERA_SCALE                 1.0f
#define SG_DEFAULT_CAMERA_VIEW_ALL_FACTOR       1.0f
#define SG_DEFAULT_ORTHOGRAPHIC_CAMERA_HEIGHT   2.0f
#define SG_DEFAULT_PERSPECTIVE_CAMERA_HEIGHT   45.0f


#endif //_CADKIT_SCENEGRAPH_CORE_LIBRARY_POUND_DEFINES_H_
