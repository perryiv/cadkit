
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Call back to explicitly set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_CALLBACKS_BOUNDING_BOX_H__
#define __OSG_TOOLS_CALLBACKS_BOUNDING_BOX_H__

#include "OsgTools/Export.h"

#include "osg/Drawable"
#include "osg/BoundingBox"


namespace OsgTools {
namespace Callbacks {


struct OSG_TOOLS_EXPORT BoundingBox : public osg::Drawable::ComputeBoundingBoxCallback
{
  BoundingBox ( const osg::BoundingBox &bb ) :
  _bb ( bb )
  {
  }

  virtual osg::BoundingBox computeBound(const osg::Drawable&) const
  {
    return _bb;
  }

private:
  osg::BoundingBox _bb;
};


} // namespace Callbacks
} // namespace OsgTools

#endif //__OSG_TOOLS_CALLBACKS_BOUNDING_BOX_H__

