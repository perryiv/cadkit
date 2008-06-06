
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_WIDGETS_CLIP_PLANE_H__
#define __OSG_TOOLS_WIDGETS_CLIP_PLANE_H__

#include "OsgTools/Export.h"
#include "OsgTools/Configure/OSG.h"

#include "osg/ref_ptr"
#include "osg/BoundingBox"
#include "osg/ClipPlane"
#include "osg/Group"

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT ClipPlane : public osg::Group
{
public:
  typedef osg::Group BaseClass;
  typedef osg::ref_ptr < ClipPlane > Ptr;

  /// Constructor
  ClipPlane ( const osg::BoundingBox& bb, osg::ClipPlane *clip );
  
  void update();

protected:
  void _setGeometry();

  /// Destructor.  Use ref counting.
  virtual ~ClipPlane();

  osg::BoundingBox           _bb;
  osg::ref_ptr < osg::ClipPlane > _clipPlane;
};

}
}

#endif // __OSG_TOOLS_WIDGETS_CLIP_PLANE_H__
