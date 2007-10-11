
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_RENDER_FBO_SCREEN_CAPTURE_H__
#define __OSGTOOLS_RENDER_FBO_SCREEN_CAPTURE_H__

#include "OsgTools/Export.h"

#include "Usul/Math/Vector2.h"

#include "osg/Vec4"
#include "osg/Matrix"

namespace osg     { class Image; }
namespace osgUtil { class SceneView; }

namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT FBOScreenCapture
{
public:

  /// Construction/Destruction.
  FBOScreenCapture ();
  ~FBOScreenCapture ();

  /// Get/Set the size.
  void                         size ( unsigned int width, unsigned int height );
  void                         size ( const Usul::Math::Vec2ui& size );
  const Usul::Math::Vec2ui&    size () const;

  /// Get/Set the clear color.
  void                         clearColor ( const osg::Vec4& color );
  const osg::Vec4&             clearColor () const;

  /// Get/Set the view matrix.
  void                         viewMatrix ( const osg::Matrix& matrix );
  const osg::Matrix&           viewMatrix () const;

  // Create the image.
  void                         operator () ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection );
  osg::Image*                  operator () ( osgUtil::SceneView& sceneView, const osg::Matrix& projection );

private:
  Usul::Math::Vec2ui _size;
  osg::Vec4          _color;
  osg::Matrix        _viewMatrix;
};


}
}

#endif // __OSGTOOLS_RENDER_FBO_SCREEN_CAPTURE_H__
