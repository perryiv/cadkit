
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Default values.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_DEFAULT_VALUES_H_
#define _GENERIC_SCENE_GRAPH_CORE_DEFAULT_VALUES_H_

#include "GSG/Core/Config.h"
#include "GSG/Core/Color.h"


namespace GSG {


/////////////////////////////////////////////////////////////////////////////
//
//  Some common colors.
//
/////////////////////////////////////////////////////////////////////////////

const Color GSG_COLOR_BLACK  ( 0.0f,  0.0f,  0.0f,  1.0f );
const Color GSG_COLOR_BLUE   ( 0.0f,  0.0f,  1.0f,  1.0f );
const Color GSG_COLOR_YELLOW ( 1.0f,  1.0f,  0.0f,  1.0f );
const Color GSG_COLOR_RED    ( 1.0f,  0.0f,  0.0f,  1.0f );
const Color GSG_COLOR_GREEN  ( 0.0f,  1.0f,  0.0f,  1.0f );
const Color GSG_COLOR_PURPLE ( 1.0f,  0.0f,  1.0f,  1.0f );
const Color GSG_COLOR_WHITE  ( 1.0f,  1.0f,  1.0f,  1.0f );
const Color GSG_COLOR_ORANGE ( 0.5f,  0.5f,  0.0f,  1.0f );


/////////////////////////////////////////////////////////////////////////////
//
//  Default material.
//
/////////////////////////////////////////////////////////////////////////////

const Color GSG_MATERIAL_FRONT_AMBIENT   ( 0.2f,  0.2f,  0.2f,  1.0f );
const Color GSG_MATERIAL_FRONT_DIFFUSE   ( 0.8f,  0.8f,  0.8f,  1.0f );
const Color GSG_MATERIAL_FRONT_SPECULAR  ( 0.0f,  0.0f,  0.0f,  1.0f );
const Color GSG_MATERIAL_FRONT_EMISSIVE  ( 0.0f,  0.0f,  0.0f,  1.0f );
const Real  GSG_MATERIAL_FRONT_SHININESS ( 0.2f );

const Color GSG_MATERIAL_BACK_AMBIENT    ( 0.1f, 0.1f, 0.1f, 1.0f );
const Color GSG_MATERIAL_BACK_DIFFUSE    ( 0.2f, 0.2f, 0.2f, 1.0f );
const Color GSG_MATERIAL_BACK_SPECULAR   ( GSG_MATERIAL_FRONT_SPECULAR );
const Color GSG_MATERIAL_BACK_EMISSIVE   ( GSG_MATERIAL_FRONT_EMISSIVE );
const Real  GSG_MATERIAL_BACK_SHININESS  ( GSG_MATERIAL_FRONT_SHININESS );


/////////////////////////////////////////////////////////////////////////////
//
//  Default light.
//
/////////////////////////////////////////////////////////////////////////////

const Color GSG_LIGHT_AMBIENT   ( 1.0f,  1.0f,  1.0f,  1.0f );
const Color GSG_LIGHT_DIFFUSE   ( 1.0f,  1.0f,  1.0f,  1.0f );
const Color GSG_LIGHT_SPECULAR  ( 1.0f,  1.0f,  1.0f,  1.0f );
const Vec4  GSG_LIGHT_POSITION  ( 0.0f,  0.0f,  1.0f,  0.0f );


/////////////////////////////////////////////////////////////////////////////
//
//  Default camera.
//
/////////////////////////////////////////////////////////////////////////////

const Real GSG_CAMERA_Z_NEAR       ( 0.1f );
const Real GSG_CAMERA_Z_FAR     ( 1000.0f );
const Real GSG_FIELD_OF_VIEW_Y    ( 45.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Default background color.
//
///////////////////////////////////////////////////////////////////////////////

const Color GSG_BACKGROUND_COLOR ( 0.792156f, 0.913725f, 1.0f, 1.0f );


}; // namespace GSG


#endif //_GENERIC_SCENE_GRAPH_CORE_DEFAULT_VALUES_H_
