
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Open scene graph renderer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_OSG_RENDERER_CLASS_H_
#define _DISPLAY_LIBRARY_OSG_RENDERER_CLASS_H_

#include "Display/Render/Renderer.h"

#include "Usul/Interfaces/IOpenGLContext.h"

#include "osgUtil/SceneView"


namespace Display {
namespace Render {
namespace OSG {


class DISPLAY_LIBRARY_EXPORT Renderer : public Display::Render::Renderer
{
public:

  // Typedefs.
  typedef Display::Render::Renderer BaseClass;
  typedef osg::ref_ptr<osgUtil::SceneView> ViewerPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IOpenGLContext IContext;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Renderer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Construction.
  Renderer ( IUnknown::RefPtr context, IUnknown::RefPtr caller );

  // Call this when you want the viewport to resize.
  virtual void              resize ( unsigned int width, unsigned int height );

  // Set the scene.
  virtual void              scene ( NodePtr );

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
} // namespace Render
} // namespace Display


#endif // _DISPLAY_LIBRARY_OSG_RENDERER_CLASS_H_
