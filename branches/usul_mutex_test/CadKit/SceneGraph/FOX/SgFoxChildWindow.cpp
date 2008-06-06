
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

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "SgFoxChildWindow.h"
#include "SgFoxView.h"

#include "GUI/FOX/FoxDefine.h"

#include "SceneGraph/FOX/SgFoxContext.h"

#include "SceneGraph/OpenGL/SgGlRenderer.h"

#include "SceneGraph/Core/SgPerspectiveCamera.h"

#include "Standard/SlAssert.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

SlAutoPtr<FXGLVisual> SgFoxChildWindow::_visual ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

//FXDEFMAP ( SgFoxChildWindow ) WindowMap[] = 
//{
//  // Message_Type          ID                       Message_Handler.
//  FXMAPFUNC ( SEL_COMMAND, SgFoxChildWindow::ID_ABOUT, SgFoxChildWindow::onCommandAbout ),
//};
//
//FXIMPLEMENT ( SgFoxChildWindow, FXMDIChild, WindowMap, ARRAYNUMBER ( WindowMap ) );
FXIMPLEMENT ( SgFoxChildWindow, FXMDIChild, 0x0, 0 );


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
  _root ( new SgGroup )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgFoxChildWindow::SgFoxChildWindow() : FXMDIChild(), INITIALIZER_LIST
{
  // Don't use this constructor.
  SL_ASSERT ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgFoxChildWindow::SgFoxChildWindow ( 
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
  SL_ASSERT ( _root.isValid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgFoxChildWindow::~SgFoxChildWindow()
{
  // Nothing to delete.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize. Repeated calls should work (I think).
//
///////////////////////////////////////////////////////////////////////////////

bool SgFoxChildWindow::init()
{
  SL_ASSERT ( this );
  SL_ASSERT ( this->getApp() );
  SL_ASSERT ( this->isCreated() );
	SL_ASSERT ( _root.isValid() );

  // The first time we create the visual.
  if ( _visual.isNull() )
  {
    _visual = new FXGLVisual ( this->getApp(), VISUAL_DOUBLEBUFFER );
    GUI_FOX_CHECK_EXPRESSION ( _visual.isValid() );
    _visual->create();
  }

  // View.
  _view = new SgFoxView ( this, _visual );
  GUI_FOX_CHECK_EXPRESSION ( _view.isValid() );
  _view->create();
  GUI_FOX_CHECK_EXPRESSION ( _view->init() );

  // Make the viewer.
  SgViewer::Ptr viewer ( new SgViewer );
  GUI_FOX_CHECK_EXPRESSION ( viewer.isValid() );

  // Set the view's viewer.
  _view->setViewer ( viewer );

  // Make the rendering context.
  SgGlContext::Ptr context ( new SgFoxContext );
  GUI_FOX_CHECK_EXPRESSION ( context.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( context->create ( _view ) );

  // Make the renderer.
  SgGlRenderer::Ptr renderer ( new SgGlRenderer );
  GUI_FOX_CHECK_EXPRESSION ( renderer.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( renderer->init ( SlViewporti ( 0, 0, _view->getWidth(), _view->getHeight() ), context ) );

  // Set the viewer's renderer.
  viewer->setRenderer ( renderer );

  // Set the viewer's scene.
  viewer->setScene ( _root );

  // Make the camera.
  SgPerspectiveCamera::Ptr camera ( new SgPerspectiveCamera );
  GUI_FOX_CHECK_EXPRESSION ( camera.isValid() );

  // Set the camera's aspect ratio.
  camera->setAspectRatio ( _view->getWidth(), _view->getHeight() );

  // Set the viewer's camera.
  viewer->setCamera ( camera );

  // Look at the whole scene.
  viewer->viewAll();

	// It worked.
	return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the window. Keep this function, if for no reason other than the 
//  comment in the header file.
//
///////////////////////////////////////////////////////////////////////////////

void SgFoxChildWindow::create()
{
  // Call the base class's function.
  FXMDIChild::create();
}
