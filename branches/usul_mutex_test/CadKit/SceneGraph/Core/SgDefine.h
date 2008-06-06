
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
