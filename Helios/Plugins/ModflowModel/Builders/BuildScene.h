
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility functions for helping build the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BUILD_SCENE_FUNCTIONS_H_
#define _MODFLOW_MODEL_BUILD_SCENE_FUNCTIONS_H_

#include "Helios/Plugins/ModflowModel/CompileGuard.h"

#include "osg/Array"
#include "osg/Vec2d"


namespace Modflow {
namespace Builders {


struct BuildScene
{
public:

  // Add quads to the list of vertices. Sizes of the "cube" are controlled with the flags.
  static void addQuads ( unsigned int flags, double x, double y, double top, double bottom, 
                         const osg::Vec2d &half, double verticalMargin, 
                         osg::Vec3Array *vertices, osg::Vec3Array *normals );
};


} // namespace Builders
} // namespace Modflow


#endif // _MODFLOW_MODEL_BUILD_SCENE_FUNCTIONS_H_
