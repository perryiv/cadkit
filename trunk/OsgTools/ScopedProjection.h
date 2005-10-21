
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Scoped projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_SCOPED_PROJECTION_MATRIX_H_
#define _OSG_TOOLS_SCOPED_PROJECTION_MATRIX_H_

#include "osgUtil/SceneView"
#include "osg/Matrixd"


namespace OsgTools {


struct ScopedProjection
{
  ScopedProjection ( osgUtil::SceneView *sv ) : _matrix(), _viewer ( sv )
  {
    if ( _viewer.valid() )
      _matrix = _viewer->getProjectionMatrix();
  }

  ~ScopedProjection()
  {
    if ( _viewer.valid() )
      _viewer->setProjectionMatrix ( _matrix );
  }

private:

  ScopedProjection ( const ScopedProjection & );
  ScopedProjection &operator = ( const ScopedProjection & );

  osg::Matrixd _matrix;
  osg::ref_ptr<osgUtil::SceneView> _viewer;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SCOPED_PROJECTION_MATRIX_H_
