
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

#include "OsgTools/Builders/GradientBackground.h"

#include "osg/AutoTransform"
#include "osg/Vec4"
#include "osg/Matrix"
#include "osg/NodeVisitor"
#include "osg/Projection"

#include "osgText/Text"

#include <map>

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

  // Set the background properties.
  void                         background ( const OsgTools::Builders::GradientBackground& background );

  /// Get/Set the clear color.
  void                         clearColor ( const osg::Vec4& color );
  const osg::Vec4&             clearColor () const;

  /// Get/Set the view matrix.
  void                         viewMatrix ( const osg::Matrix& matrix );
  const osg::Matrix&           viewMatrix () const;

  /// Get/Set the number of samples.
  void                         numSamples ( unsigned int samples );
  unsigned int                 numSamples () const;

  /// Get/Set the scale.
  void                         scale ( float scale );
  float                        scale () const;

  // Create the image.
  void                         operator () ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection );
  osg::Image*                  operator () ( osgUtil::SceneView& sceneView, const osg::Matrix& projection );

private:

  /// Pre or post process the scene for tiled rendering.
  class ProcessScene : public osg::NodeVisitor
  {
  public:
    typedef osg::NodeVisitor BaseClass;

    ProcessScene ( float scale );

    void         restoreScene ();

    virtual void apply( osg::Projection& node );
    virtual void apply( osg::Transform& node );
    virtual void apply( osg::Geode& node );

  protected:
    virtual ~ProcessScene ();

  private:
    typedef osg::ref_ptr < osg::AutoTransform > AutoTransformPtr;
    typedef osg::ref_ptr < osg::Projection > ProjectionPtr;
    typedef osg::ref_ptr < osgText::Text > TextPtr;

    typedef std::map < AutoTransformPtr, bool > AutoTransforms;
    typedef std::map < ProjectionPtr, unsigned int > Projections;
    typedef std::map < TextPtr, float > Texts;

    float _scale;
    AutoTransforms _autoTransforms;
    Projections _projections;
    Texts _texts;
  };

  void                         _capturePixels ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection );
  void                         _accumulate    ( osg::Image& image, const osg::Image& tile, unsigned int x, unsigned int y );

  Usul::Math::Vec2ui _size;
  osg::Vec4          _color;
  osg::Matrix        _viewMatrix;
  unsigned int       _numSamples;
  float              _scale;
  OsgTools::Builders::GradientBackground _background;
};


}
}

#endif // __OSGTOOLS_RENDER_TILED_SCREEN_CAPTURE_H__
