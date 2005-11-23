
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Call back to explicitly set the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_CALLBACKS_BOUNDING_SPHERE_H__
#define __OSG_TOOLS_CALLBACKS_BOUNDING_SPHERE_H__

#include "OsgTools/Export.h"

#include "osg/Node"
#include "osg/BoundingSphere"


namespace OsgTools {
namespace Callbacks {


struct OSG_TOOLS_EXPORT BoundingSphere : public osg::Node::ComputeBoundingSphereCallback
{
  BoundingSphere ( const osg::BoundingSphere &bs );

  virtual osg::BoundingSphere computeBound(const osg::Node&) const;

private:
  osg::BoundingSphere _bs;
};


} // namespace Callbacks
} // namespace OsgTools

#endif //__OSG_TOOLS_CALLBACKS_BOUNDING_SPHERE_H__

  