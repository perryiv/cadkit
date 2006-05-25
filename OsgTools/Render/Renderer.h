
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_RENDERER_H__
#define __OSG_TOOLS_RENDER_RENDERER_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"

#include "osg/ref_ptr"

#include "osgUtil/SceneView"

namespace OsgTools {
namespace Render {


class Renderer : public Usul::Base::Referenced
{
public:
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Usul::Interfaces::IUnknown members.
  //USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef osgUtil::SceneView SceneView;
  typedef osg::ref_ptr<SceneView> SceneViewPtr;
  typedef osg::Viewport Viewport;

  Renderer();

  // Set/get the background color. Throws if getting color from a null viewer.
  void                  backgroundColor ( const osg::Vec4 &color );
  const osg::Vec4 &     backgroundColor() const;

  // Clear
  void                  clear();

  // Initialize.  Assumes the context is already current.
  void                  init();

  // Set the view frustum.
  void                  frustum ( float left, float right, float bottom, float top, float near, float far );

  // Is there an accumulation buffer?
  bool                  hasAccumBuffer() const;

  // Get the clipping plane distances.
  void                  nearFar ( double &n, double &f ) const;

  // Set/get the number of rendering passes. Unavailable numbers have no effect.
  void                  numRenderPasses ( unsigned int );
  unsigned int          numRenderPasses() const { return _numPasses; }

  // Render.  Assumes the context is already current.
  void                  render();

  // Set/Get the scene
  void                  scene ( osg::Node *node );
  const osg::Node *     scene() const;
  osg::Node *           scene();

  // Get the time.
  double                timeAverage ( const std::string &name ) const;
  double                timeLast    ( const std::string &name ) const;
  void                  timeStart   ( const std::string &name );
  void                  timeStop    ( const std::string &name );

  // Set a unique ID for the viewer
  void                  uniqueID ( unsigned int id );

  // Update the scene.
  void                  updateScene();

  // Set/Get the view matrix.
  void                  viewMatrix ( const osg::Matrixf& matrix );
  const osg::Matrixf&   viewMatrix ( ) const;

  // Set/get the viewport.
  const Viewport *      viewport() const;
  Viewport *            viewport();
  void                  viewport ( Viewport *vp );
  void                  viewport ( int x, int y, unsigned int w, unsigned int h );

  // Set/get the viewer.
  const SceneView *     viewer() const;
  SceneView *           viewer();
  void                  viewer ( SceneView * );
protected:

  virtual ~Renderer();

  void                  _multiPassRender();
  void                  _singlePassRender();

  void                  _render();

  void                  _cullAndDraw();

private:

  typedef std::pair < double, double > TimePair;
  typedef std::list < TimePair > TimeHistory;
  typedef std::map < std::string, TimeHistory > TimeHistories;

  SceneViewPtr _sceneView;
  osg::ref_ptr< osg::Node > _scene;

  TimeHistories _times;

  unsigned int _numPasses;  
  
  bool _hasAccumulationBuffer;

  unsigned int _contextId;
};

}
}

#endif // __OSG_TOOLS_RENDER_RENDERER_H__
