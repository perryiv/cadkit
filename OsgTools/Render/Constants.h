
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Constant values.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_RENDER_CONSTANT_VALUES_H_
#define _OPEN_SCENE_GRAPH_TOOLS_RENDER_CONSTANT_VALUES_H_

#include "osg/Vec4"

#include <string>


namespace OsgTools {
namespace Render {


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

const osg::Vec4 MATERIAL_BACK_AMBIENT    ( 0.1f,  0.1f,  0.1f,  1.0f );
const osg::Vec4 MATERIAL_BACK_DIFFUSE    ( 0.2f,  0.2f,  0.2f,  1.0f );
const osg::Vec4 MATERIAL_BACK_SPECULAR   ( MATERIAL_FRONT_SPECULAR   );
const osg::Vec4 MATERIAL_BACK_EMISSIVE   ( MATERIAL_FRONT_EMISSIVE   );
const float     MATERIAL_BACK_SHININESS  ( MATERIAL_FRONT_SHININESS  );


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

const double CAMERA_Z_NEAR (    0.1 );
const double CAMERA_Z_FAR  ( 1000.0 );
const double CAMERA_FOV_Y  (   45.0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Default clear color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 CLEAR_COLOR ( 0.792156f, 0.913725f, 1.0f, 1.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Default clear color.
//
///////////////////////////////////////////////////////////////////////////////

const std::string BOUNDING_BOX      ( "bounding_box"    );
const std::string BOUNDING_SPHERE   ( "bounding_sphere" );
const std::string TEXT_MATRIX       ( "text_matrix"     );
const std::string SELECTION_BOX     ( "selection_box"   );
const std::string CLIPPING_PLANES   ( "cliping_planes"  );


///////////////////////////////////////////////////////////////////////////////
//
//  Constant strings used for preferences, registry keys, etc.
//
///////////////////////////////////////////////////////////////////////////////

const std::string ALLOW_SPIN        ( "allow_spin"     );
const std::string ANIMATION_TIME    ( "animation_time" );
const std::string DISPLAY_LISTS     ( "display_lists"  );
const std::string LOW_LODS          ( "low_lods"       );
const std::string HIGH_LODS         ( "high_lods"      );


}; // namespace Render
}; // namespace OsgTools


#endif //_OPEN_SCENE_GRAPH_TOOLS_RENDER_CONSTANT_VALUES_H_
