
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/FOX/Precompiled.h"
#include "GSG/FOX/MdiChildWindow.h"
#include "GSG/FOX/View.h"
#include "GSG/FOX/Context.h"

#include "GSG/OpenGL/GlDraw.h"

#include "GSG/Core/Camera.h"
#include "GSG/Core/Update.h"
#include "GSG/Core/FrustumCull.h"
#include "GSG/Core/Sort.h"
#include "GSG/Core/Renderer.h"
#include "GSG/Core/RenderBin.h"

using namespace GSG;
using namespace GSG::FOX;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::FXGLVisualPtr MdiChildWindow::_visual ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXIMPLEMENT ( MdiChildWindow, FXMDIChild, 0x0, 0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
//  We have to declare the scene root here in the constructor instead of 
//  in init() so that clients can add nodes to the scene in the 
//  constructors of their frame classes that inherit from this class.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIALIZER_LIST \
  _view ( 0x0 ), \
  _root ( new Group )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::MdiChildWindow() : FXMDIChild(), INITIALIZER_LIST
{
  // Do not use this constructor.
  ErrorChecker ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::MdiChildWindow ( 
  FXMDIClient *parent,
  const FXString &name,
  FXIcon *icon,
  FXMenuPane *menu,
  FXuint options,
  FXint x,
  FXint y,
  FXint w,
  FXint h ) : 
  FXMDIChild ( parent, name, icon, menu, options, x, y, w, h ),
  INITIALIZER_LIST
{
  // Should be true.
  ErrorChecker ( 0x0 != _root.get() );
  ErrorChecker ( 0x0 != this->getApp() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::~MdiChildWindow()
{
  // Nothing to delete.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::create()
{
  // Should be true.
  ErrorChecker ( 0x0 != _root.get() );
  ErrorChecker ( 0x0 != this->getApp() );
  ErrorChecker ( 0x0 != this->getParent()->id() );

  // Call the base class's function.
  BaseClass::create();

  // The first time we create the visual.
  if ( 0x0 == _visual.get() )
  {
    _visual = FXGLVisualPtr ( new FXGLVisual ( this->getApp(), FX::VISUAL_DOUBLEBUFFER ) );
    ErrorChecker ( 0x0 != _visual.get() );
    _visual->create();
  }

  // View.
  _view = ViewPtr ( new View ( this, _visual.get() ) );
  ErrorChecker ( 0x0 != _view.get() );
  _view->create();

  // Make the viewer and set it.
  Viewer::ValidPtr viewer ( new Viewer );
  _view->viewer ( viewer );

  // Make the rendering context.
  GlContext::ValidPtr ct ( new FOX::Context ( _view.get() ) );

  // Need a viewport.
  Viewport vp ( 0, 0, _view->getWidth(), _view->getHeight() );

  // Make the renderer.
  RenderBin::ValidPtr bin ( new RenderBin );
  Renderer::ValidPtr renderer ( new Renderer );
  renderer->append ( new Update );
  renderer->append ( new FrustumCull ( bin ) );
  renderer->append ( new Sort ( bin ) );
  renderer->append ( new GlDraw ( bin, vp, ct ) );

  // Set the viewer's renderer.
  viewer->renderer ( renderer );

  // Set the viewer's scene.
  viewer->scene ( _root );

  // Make the camera.
  Camera::ValidPtr camera ( new Camera );

  // Set the camera's aspect ratio.
  camera->aspectRatio ( vp.width(), vp.height() );

  // Set the viewer's camera.
  viewer->camera ( camera );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is it created?
//
///////////////////////////////////////////////////////////////////////////////

bool MdiChildWindow::isCreated() const
{
  return ( 0x0 != this->id() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the root.
//
///////////////////////////////////////////////////////////////////////////////

const Group *MdiChildWindow::root() const
{
  GSG::Lock lock ( this );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the root.
//
///////////////////////////////////////////////////////////////////////////////

Group *MdiChildWindow::root()
{
  GSG::Lock lock ( this );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the view.
//
///////////////////////////////////////////////////////////////////////////////

const View *MdiChildWindow::view() const
{
  GSG::Lock lock ( this );
  return _view.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the view.
//
///////////////////////////////////////////////////////////////////////////////

View *MdiChildWindow::view()
{
  GSG::Lock lock ( this );
  return _view.get();
}
