
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Scoped viewport.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_SCOPED_VIEWPORT_H_
#define _OSG_TOOLS_SCOPED_VIEWPORT_H_

#include "osgUtil/SceneView"
#include "osg/Viewport"


namespace OsgTools {


struct ScopedViewport
{
  ScopedViewport ( osgUtil::SceneView *sv ) : _viewport(), _sceneView ( sv )
  {
    if ( _sceneView.valid() )
      _viewport = new osg::Viewport ( *(_sceneView->getViewport()), osg::CopyOp::DEEP_COPY_ALL );
  }

  ~ScopedViewport()
  {
    if ( _sceneView.valid() )
      _sceneView->setViewport ( _viewport.get() );
  }

private:

  ScopedViewport ( const ScopedViewport & );
  ScopedViewport &operator = ( const ScopedViewport & );

  osg::ref_ptr<osg::Viewport> _viewport;
  osg::ref_ptr<osgUtil::SceneView> _sceneView;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SCOPED_VIEWPORT_H_
