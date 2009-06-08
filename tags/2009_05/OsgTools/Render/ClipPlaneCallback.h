
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_CLIP_PLANE_CALLBACK_H__
#define __OSG_TOOLS_RENDER_CLIP_PLANE_CALLBACK_H__

#include "OsgTools/Draggers/Callback.h"

#include "osg/ref_ptr"
#include "osg/Vec3"

namespace osg { class ClipPlane; }

namespace OsgTools {
namespace Render {

class ClipPlaneCallback : public OsgTools::Draggers::Callback
{
public:
  typedef OsgTools::Draggers::Callback BaseClass;
  typedef OsgTools::Draggers::Dragger Dragger;

  ClipPlaneCallback ( const osg::Vec3& topLeft, const osg::Vec3& bottomLeft, const osg::Vec3& topRight, osg::ClipPlane* clipPlane );
  ClipPlaneCallback ( const ClipPlaneCallback & );

  // Execute the callback.
  virtual void operator() ( Dragger &d );
  
protected:

  // Use reference counting.
  virtual ~ClipPlaneCallback();

private:
  osg::Vec3 _topLeft;
  osg::Vec3 _bottomLeft;
  osg::Vec3 _topRight;

  osg::ref_ptr< osg::ClipPlane > _clipPlane;
};

}
}


#endif // __OSG_TOOLS_RENDER_CLIP_PLANE_CALLBACK_H__
