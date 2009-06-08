
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OSG renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_RENDERER_H__
#define __OSG_TOOLS_RENDER_RENDERER_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/File/Temp.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IRenderInfoOSG.h"

#include "OsgTools/Builders/GradientBackground.h"

#include "osgUtil/SceneView"

#include "osg/ref_ptr"
#include "osg/Timer"
#include "osg/ClearNode"

#include <vector>


namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT Renderer : public Usul::Base::Referenced,
                                  public Usul::Interfaces::IRenderInfoOSG
{
public:

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef osgUtil::SceneView SceneView;
  typedef osg::ref_ptr<SceneView> SceneViewPtr;
  typedef osg::Viewport Viewport;
  typedef std::vector < std::string > ImageList;
  typedef OsgTools::Builders::GradientBackground GradientBackground;
  typedef GradientBackground::Corners            Corners;
  typedef osg::RenderInfo RenderInfo;

  // Constructor
  Renderer();

  // Set/get the background color.
  void                  backgroundColor ( const osg::Vec4 &color, unsigned int corners );
  void                  backgroundColor ( const osg::Vec4 &color );
  osg::Vec4             backgroundColor ( unsigned int corners ) const;
  osg::Vec4             backgroundColor() const;

  // Set/get the background corners.
  void                  backgroundCorners ( unsigned int corners );
  unsigned int          backgroundCorners() const;

  // Clear
  void                  clear();

  void cull();

  // Initialize.  Assumes the context is already current.
  void                  init();

  // Set the projection matrix as a frustum
  void                  setFrustum ( double left, double right, double bottom, double top, double near, double far );
  bool                  getFrustum ( double &left, double &right, double &bottom, double &top, double &near, double& far ) const;

  // Get the frame stamp.
  osg::FrameStamp*       framestamp();
  const osg::FrameStamp* framestamp() const;

  // Get the global state set.
  osg::StateSet*        getGlobalStateSet();
  const osg::StateSet*  getGlobalStateSet() const;

  // Is there an accumulation buffer?
  bool                  hasAccumBuffer() const;

  // Get the light.
  osg::Light *          light();
  const osg::Light *    light() const;

  // Get the clipping plane distances.
  void                  nearFar ( double &n, double &f ) const;
  
  // Set/get the number of rendering passes. Unavailable numbers have no effect.
  void                  setNumRenderPasses ( unsigned int );
  unsigned int          getNumRenderPasses() const { return _numPasses; }

  // Render.  Assumes the context is already current.
  void                  render();

  // Get the render info for this Renderer.
  virtual RenderInfo    getRenderInfo() const;

  // Resize the viewer.
  void                  resize ( unsigned int width, unsigned int height );

  // Set/get the scatter scale.
  double                scatterScale() const;
  void                  scatterScale ( double );

  // Set/Get the scene
  void                  scene ( osg::Node *node );
  const osg::Node *     scene() const;
  osg::Node *           scene();

  // Get a screen capture with the given view matrix.
  osg::Image*           screenCapture ( const osg::Matrix& matrix, unsigned int width, unsigned int height );

  // Capture the screen.
  osg::Image*           screenCapture ( float frameSizeScale, unsigned int numSamples );

  // Get the time.
  double                timeAverage ( const std::string &name ) const;
  double                timeLast    ( const std::string &name ) const;
  void                  timeStart   ( const std::string &name );
  void                  timeStop    ( const std::string &name );

  // Set a unique ID for the viewer.
  void                  uniqueID ( unsigned int id );

  // Update the scene.
  void                  update();

  // Set/Get the view matrix.
  void                  viewMatrix ( const osg::Matrixd& matrix );
  const osg::Matrixd    viewMatrix ( ) const;

  // Set/get the viewport.
  const Viewport *      viewport() const;
  Viewport *            viewport();
  void                  viewport ( Viewport *vp );
  void                  viewport ( int x, int y, unsigned int w, unsigned int h );

  // Get the viewer.
  const SceneView *     viewer() const;
  SceneView *           viewer();
  void                  viewer( SceneView * );

protected:

  virtual ~Renderer();

  void                  _cullAndDraw();

  void                  _multiPassRender();

  void                  _singlePassRender();

private:

  typedef std::pair < double, double > TimePair;
  typedef std::list < TimePair > TimeHistory;
  typedef std::map < std::string, TimeHistory > TimeHistories;

  SceneViewPtr _sceneView;
  osg::ref_ptr<osg::FrameStamp> _framestamp;
  osg::Timer _timer;
  osg::Timer_t _start_tick;
  TimeHistories _times;
  unsigned int _numPasses;  
  unsigned int _contextId;
  bool _hasAccumBuffer;
  GradientBackground _gradient;
  unsigned int _corners;
  osg::ref_ptr<osg::ClearNode> _clearNode;
};

}
}

#endif // __OSG_TOOLS_RENDER_RENDERER_H__
