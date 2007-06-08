
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


namespace OsgTools {
namespace State {


struct OSG_TOOLS_EXPORT StateSet
{
  // Set/get the lighting state
  static bool         getLighting  ( osg::Node *node );
  static bool         getLighting  ( const osg::StateSet *ss );
  static void         setLighting  ( osg::Node *node, bool state );
  static void         setLighting  ( osg::StateSet *ss, bool state );

  // Set/Get the two sided lighting state.
  static bool         getTwoSidedLighting ( osg::Node *node );
  static bool         getTwoSidedLighting ( const osg::StateSet *ss );
  static void         setTwoSidedLighting ( osg::Node *node, bool state );
  static void         setTwoSidedLighting ( osg::StateSet *ss, bool state );

  // Set/get the normalization state.
  static bool         getNormalize ( const osg::Node *node );
  static void         setNormalize ( osg::Node *node, bool state );
  static void         setNormalize ( osg::StateSet *ss, bool state );

  // Set/get the polygon state.
  static bool         getPolygonsFilled ( osg::Node *node, bool twoSided );
  static void         setPolygonsFilled ( osg::Node *node, bool twoSided );

  // Make polygons draw with lines.
  static bool         getPolygonsLines ( osg::Node *node, bool twoSided );
  static void         setPolygonsLines ( osg::Node *node, bool twoSided );

  // Make polygons draw with points.
  static bool         getPolygonsPoints ( osg::Node *node, bool twoSided );
  static void         setPolygonsPoints ( osg::Node *node, bool twoSided );

  // Make filled polygons draw with smooth shading.
  static bool         getPolygonsSmooth ( osg::Node *node );
  static void         setPolygonsSmooth ( osg::Node *node );

  // Make filled polygons draw with flat shading.
  static bool         getPolygonsFlat ( osg::Node *node );
  static void         setPolygonsFlat ( osg::Node *node );

  // Make filled polygons draw with flat shading.
  static bool         getPolygonsTextures ( osg::StateSet* ss );
  static void         setPolygonsTextures ( osg::StateSet* ss , bool todo );

  // Set/get the polygon mode.
  static void         setPolygonMode ( osg::Node *node, unsigned int face, unsigned int mode );
  static bool         getPolygonMode ( osg::Node *node, unsigned int face, unsigned int mode );

  // Set/Get line width
  static void         setLineWidth ( osg::StateSet *ss, float width );
  static void         setLineWidth ( osg::Node *node, float width );
  static float        getLineWidth ( osg::Node *node );

  // Set the states for hidden lines.
  static void         hiddenLines ( const osg::Vec4f &color, osg::StateSet *normal, osg::StateSet *hidden );
};


}; // namespace State
}; // namespace OsgTools


#endif // _OSG_TOOLS_STATE_CLASS_H_
