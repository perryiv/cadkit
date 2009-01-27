
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Open scene graph renderer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_RENDERER_CLASS_H_
#define _SCENE_GRAPH_OSG_RENDERER_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IOpenGLContext.h"
#include "Usul/Interfaces/ISceneGraph.h"

#include "osgUtil/SceneView"


namespace SceneGraph {
namespace OSG {


class Renderer :  :
  public Usul::Base::Object,
  public Usul::Interfaces::IOpenGLContext,
  public Usul::Interfaces::SceneGraph::IPreRender,
  public Usul::Interfaces::SceneGraph::IRender,
  public Usul::Interfaces::SceneGraph::IPostRender
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::SceneGraph::IPreRender IPreRender;
  typedef Usul::Interfaces::SceneGraph::IRender IRender;
  typedef Usul::Interfaces::SceneGraph::IPostRender IPostRender;
  typedef Usul::Interfaces::IOpenGLContext IContext;
  typedef osg::ref_ptr<osgUtil::SceneView> ViewerPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Renderer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Construction.
  Renderer ( IUnknown::RefPtr context, IUnknown::RefPtr caller );

  // Usul::Interfaces::SceneGraph::IPostRender
  virtual void              postRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene );

  // Usul::Interfaces::SceneGraph::IPreRender
  virtual void              preRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene );

  // Usul::Interfaces::SceneGraph::IRender
  virtual void              render ( IUnknown::RefPtr projection, IUnknown::RefPtr scene );

here

protected:

  // Use reference counting.
  virtual ~Renderer();

  // Render the scene.
  virtual void              _render();

  IContext::RefPtr          _getContext();
  const IContext::RefPtr    _getContext() const;
  ViewerPtr                 _getViewer();
  const ViewerPtr           _getViewer() const;

private:

  // No copying or assignment.
  Renderer ( const Renderer & );
  Renderer &operator = ( const Renderer & );

  void                      _destroy();

  ViewerPtr _viewer;
  IContext::QueryPtr _context;
  IUnknown::RefPtr _caller;
};


} // namespace OSG
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OSG_RENDERER_CLASS_H_
