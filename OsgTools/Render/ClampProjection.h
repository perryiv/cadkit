
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback class for clamping the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_CLAMP_PROJECTION_H_
#define _OPEN_SCENE_GRAPH_TOOLS_CLAMP_PROJECTION_H_

#include "OsgTools/Render/Constants.h"

#include "osgUtil/CullVisitor"


namespace OsgTools {
namespace Render {


struct ClampProjection : public osg::CullSettings::ClampProjectionMatrixCallback
{
  typedef osg::CullSettings::ClampProjectionMatrixCallback BaseClass;

  ClampProjection ( osgUtil::CullVisitor &cv ) : BaseClass(), _cv ( cv )
  {
  }

  virtual bool clampProjectionMatrixImplementation ( osg::Matrixf &projection, double &zNear, double &zFar ) const
  {
    return this->_common ( projection, zNear, zFar );
  }

  virtual bool clampProjectionMatrixImplementation ( osg::Matrixd &projection, double &zNear, double &zFar ) const
  {
    return this->_common ( projection, zNear, zFar );
  }

protected:

  template < class Matrix > bool _common ( Matrix &projection, double &zNear, double &zFar ) const
  {
    // Make sure near and far values are reasonable.
    if ( zNear <= 0 )
    {
      zNear = OsgTools::Render::CAMERA_Z_NEAR;
    }
    if ( zFar <= 0 )
    {
      zFar = OsgTools::Render::CAMERA_Z_FAR;
    }
    if ( zNear > zFar )
    {
      zNear = OsgTools::Render::CAMERA_Z_NEAR;
      zFar  = OsgTools::Render::CAMERA_Z_FAR;
    }

    // Ask cull-visitor to clamp the projection matrix.
    return _cv.clampProjectionMatrixImplementation ( projection, zNear, zFar );
  }

  osgUtil::CullVisitor &_cv;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_CLAMP_PROJECTION_H_
