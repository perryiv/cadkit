
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for animations.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_ANIMATION_CLASS_H_
#define _OPEN_SCENE_GRAPH_TOOLS_ANIMATION_CLASS_H_

#include "OsgTools/Export.h"

#include "osg/Vec3"
#include "osg/Quat"

#include <string>


namespace OsgTools {
namespace Render {


struct OSG_TOOLS_EXPORT Animation
{
  Animation();
  ~Animation();

  void init ( const osg::Vec3 &t1, const osg::Quat &r1, const osg::Vec3 &t2, const osg::Quat &r2, double duration );

  bool matrix ( osg::Matrixd & ) const;

private:

  // No copying or assignment.
  Animation ( const Animation & );
  Animation &operator = ( const Animation & );

  double _start;
  double _stop;
  osg::Vec3 _t1;
  osg::Quat _r1; 
  osg::Vec3 _t2;
  osg::Quat _r2;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_ANIMATION_CLASS_H_
