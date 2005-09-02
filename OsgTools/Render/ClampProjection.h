
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

#include "osgUtil/CullVisitor"


namespace OsgTools {
namespace Render {


struct ClampProjection : public osg::CullSettings::ClampProjectionMatrixCallback
{
  typedef osg::CullSettings::ClampProjectionMatrixCallback BaseClass;

  ClampProjection ( osgUtil::CullVisitor &cv, double zNear, double zFar ) : BaseClass(), _cv ( cv ), _zNear ( zNear ), _zFar ( zFar )
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
      zNear = _zNear;
    }
    if ( zFar <= 0 )
    {
      zFar = _zFar;
    }
    if ( zNear > zFar )
    {
      zNear = _zNear;
      zFar  = _zFar;
    }

    // Ask cull-visitor to clamp the projection matrix.
    return _cv.clampProjectionMatrixImplementation ( projection, zNear, zFar );
  }

  osgUtil::CullVisitor &_cv;
  const double _zNear;
  const double _zFar;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_CLAMP_PROJECTION_H_
