
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

#include "Export.h"
#include "Declarations.h"


namespace OsgTools {


struct OSG_TOOLS_EXPORT State
{
  // Set/get the lighting state
  static bool         getLighting  ( osg::Node *node );
  static void         setLighting  ( osg::Node *node, bool state );

  // Set/get the normalization state.
  static bool         getNormalize ( osg::Node *node );
  static void         setNormalize ( osg::Node *node, bool state );

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

};


}; // namespace OsgTools


#endif // _OSG_TOOLS_STATE_CLASS_H_
