
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
  ScopedViewport ( osg::Viewport *vp ) : _viewport( vp )
  {
    if ( _viewport.valid() )
    {
      _x      = _viewport->x();
      _y      = _viewport->y();
      _width  = _viewport->width();
      _height = _viewport->height();
    }
  }

  ~ScopedViewport()
  {
    if ( _viewport.valid() )
    {
      _viewport->setViewport ( _x, _y, _width, _height );
    }
  }

private:

  ScopedViewport ( const ScopedViewport & );
  ScopedViewport &operator = ( const ScopedViewport & );

  osg::ref_ptr<osg::Viewport> _viewport;
  osg::Viewport::value_type _x;
  osg::Viewport::value_type _y;
  osg::Viewport::value_type _width;
  osg::Viewport::value_type _height;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SCOPED_VIEWPORT_H_
