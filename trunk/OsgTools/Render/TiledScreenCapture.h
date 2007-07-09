
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_RENDER_TILED_SCREEN_CAPTURE_H__
#define __OSGTOOLS_RENDER_TILED_SCREEN_CAPTURE_H__

#include "OsgTools/Export.h"

#include "Usul/Math/Vector2.h"

#include "osg/Vec4"
#include "osg/Matrix"

namespace osg     { class Image; }
namespace osgUtil { class SceneView; }

namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT TiledScreenCapture
{
public:

  /// Construction/Destruction
  TiledScreenCapture ();
  ~TiledScreenCapture ();

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

  /// Get/Set the number of samples.
  void                         numSamples ( unsigned int samples );
  unsigned int                 numSamples () const;

  // Create the image.
  void                         operator () ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection );
  osg::Image*                  operator () ( osgUtil::SceneView& sceneView, const osg::Matrix& projection );

private:
  Usul::Math::Vec2ui _size;
  osg::Vec4          _color;
  osg::Matrix        _viewMatrix;
  unsigned int       _numSamples;
};


}
}

#endif // __OSGTOOLS_RENDER_TILED_SCREEN_CAPTURE_H__
