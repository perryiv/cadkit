
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for working with the state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_STATE_CLASS_H_
#define _OSG_TOOLS_STATE_CLASS_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

namespace osg { class Vec4f; }


namespace OsgTools {
namespace State {


struct OSG_TOOLS_EXPORT StateSet
{
  // Set/get the lighting state
  static bool               getLighting  ( osg::Node *node );
  static bool               getLighting  ( const osg::StateSet *ss );
  static void               setLighting  ( osg::Node *node, bool state );
  static void               setLighting  ( osg::StateSet *ss, bool state );

  // Set/Get the two sided lighting state.
  static bool               getTwoSidedLighting ( osg::Node *node );
  static bool               getTwoSidedLighting ( const osg::StateSet *ss );
  static void               setTwoSidedLighting ( osg::Node *node, bool state );
  static void               setTwoSidedLighting ( osg::StateSet *ss, bool state );

  // Set/get the normalization state.
  static bool               getNormalize ( const osg::Node *node );
  static void               setNormalize ( osg::Node *node, bool state );
  static void               setNormalize ( osg::StateSet *ss, bool state );

  // Set/get the polygon state.
  static bool               getPolygonsFilled ( const osg::Node *node, bool twoSided );
  static void               setPolygonsFilled ( osg::Node *node, bool twoSided );

  // Make polygons draw with lines.
  static bool               getPolygonsLines ( const osg::Node *node, bool twoSided );
  static void               setPolygonsLines ( osg::Node *node, bool twoSided );

  // Make polygons draw with points.
  static bool               getPolygonsPoints ( const osg::Node *node, bool twoSided );
  static void               setPolygonsPoints ( osg::Node *node, bool twoSided );

  // Make filled polygons draw with smooth shading.
  static bool               getPolygonsSmooth ( const osg::Node *node );
  static void               setPolygonsSmooth ( osg::Node *node );

  // Make filled polygons draw with flat shading.
  static bool               getPolygonsFlat ( const osg::Node *node );
  static void               setPolygonsFlat ( osg::Node *node );

  // Make filled polygons draw with flat shading.
  static bool               getPolygonsTextures ( osg::StateSet* ss );
  static void               setPolygonsTextures ( osg::StateSet* ss, bool todo );

  // Set/get the polygon mode.
  static void               setPolygonMode ( osg::Node *node, unsigned int face, unsigned int mode );
  static bool               getPolygonMode ( osg::Node *node, unsigned int face, unsigned int mode );

  // Set/Get line width
  static void               setLineWidth ( osg::StateSet *ss, float width );
  static void               setLineWidth ( osg::Node *node, float width );
  static float              getLineWidth ( osg::Node *node );

  // Set/Get point size.
  static void               setPointSize ( osg::StateSet *ss, float width );
  static void               setPointSize ( osg::Node *node, float width );
  static float              getPointSize ( osg::Node *node );

  // Set the states for hidden lines.
  static void               hiddenLines ( const osg::Vec4f &color, osg::StateSet *normal, osg::StateSet *hidden );

  // Materials.
  static void               setMaterial ( osg::Node *node, osg::Material *mat );
  static void               setMaterial ( osg::StateSet *ss, osg::Material *mat );
  static void               setMaterial ( osg::Node *node, const osg::Vec4f &ambient, const osg::Vec4f &diffuse, float alpha );
  static void               setMaterialRandom ( osg::Node *node );
  static void               setMaterialDefault ( osg::Node *node );
  static osg::Material *    getMaterialDefault();
  static void               removeMaterial ( osg::Node *node );
  static void               removeMaterial ( osg::StateSet *ss );

  // Set the alpha value. Adds default material if needed.
  static void               setAlpha ( osg::Node *node, float );
  static void               setAlpha ( osg::StateSet *ss, float );

  // Back face culling.
  static void               setBackFaceCulling ( osg::StateSet* ss, bool b );
  static bool               getBackFaceCulling ( const osg::StateSet* ss );
};


}; // namespace State
}; // namespace OsgTools


#endif // _OSG_TOOLS_STATE_CLASS_H_
