
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Default values.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_RENDER_BINDING_DEFAULT_VALUES_H_
#define _OSG_TOOLS_RENDER_BINDING_DEFAULT_VALUES_H_

#include "osg/Vec4"

namespace OsgTools {
namespace Render {
namespace Defaults {


///////////////////////////////////////////////////////////////////////////////
//
//  Some common colors.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 COLOR_BLACK  ( 0.0f,  0.0f,  0.0f,  1.0f );
const osg::Vec4 COLOR_BLUE   ( 0.0f,  0.0f,  1.0f,  1.0f );
const osg::Vec4 COLOR_YELLOW ( 1.0f,  1.0f,  0.0f,  1.0f );
const osg::Vec4 COLOR_RED    ( 1.0f,  0.0f,  0.0f,  1.0f );
const osg::Vec4 COLOR_GREEN  ( 0.0f,  1.0f,  0.0f,  1.0f );
const osg::Vec4 COLOR_PURPLE ( 1.0f,  0.0f,  1.0f,  1.0f );
const osg::Vec4 COLOR_WHITE  ( 1.0f,  1.0f,  1.0f,  1.0f );
const osg::Vec4 COLOR_ORANGE ( 0.5f,  0.5f,  0.0f,  1.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Default material.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 MATERIAL_FRONT_AMBIENT   ( 0.2f,  0.2f,  0.2f,  1.0f );
const osg::Vec4 MATERIAL_FRONT_DIFFUSE   ( 0.8f,  0.8f,  0.8f,  1.0f );
const osg::Vec4 MATERIAL_FRONT_SPECULAR  ( 0.0f,  0.0f,  0.0f,  1.0f );
const osg::Vec4 MATERIAL_FRONT_EMISSIVE  ( 0.0f,  0.0f,  0.0f,  1.0f );
const float     MATERIAL_FRONT_SHININESS ( 0.2f );

const osg::Vec4 MATERIAL_BACK_AMBIENT    ( 0.1f, 0.1f, 0.1f, 1.0f );
const osg::Vec4 MATERIAL_BACK_DIFFUSE    ( 0.2f, 0.2f, 0.2f, 1.0f );
const osg::Vec4 MATERIAL_BACK_SPECULAR   ( MATERIAL_FRONT_SPECULAR );
const osg::Vec4 MATERIAL_BACK_EMISSIVE   ( MATERIAL_FRONT_EMISSIVE );
const float     MATERIAL_BACK_SHININESS  ( MATERIAL_FRONT_SHININESS );


///////////////////////////////////////////////////////////////////////////////
//
//  Default light.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 LIGHT_AMBIENT   ( 1.0f,  1.0f,  1.0f,  1.0f );
const osg::Vec4 LIGHT_DIFFUSE   ( 1.0f,  1.0f,  1.0f,  1.0f );
const osg::Vec4 LIGHT_SPECULAR  ( 1.0f,  1.0f,  1.0f,  1.0f );
const osg::Vec4 LIGHT_POSITION  ( 0.0f,  0.0f,  1.0f,  0.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Default camera.
//
///////////////////////////////////////////////////////////////////////////////

const double CAMERA_Z_NEAR   ( 0.1 );
const double CAMERA_Z_FAR ( 1000.0 );
const double CAMERA_FOV_Y   ( 45.0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Default clear color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 CLEAR_COLOR ( 0.792156f, 0.913725f, 1.0f, 1.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Default animation time (microseconds).
//
///////////////////////////////////////////////////////////////////////////////

const double TO_MICROSECONDS ( 1000 );

///////////////////////////////////////////////////////////////////////////////
//
//  Default animation time (milliseconds).
//
///////////////////////////////////////////////////////////////////////////////

const double TO_MILLISECONDS ( 1000 );

} // namespace Defaults
} // namespace Render
} // namespace OsgTools


#endif //_OSG_TOOLS_RENDER_BINDING_DEFAULT_VALUES_H_
