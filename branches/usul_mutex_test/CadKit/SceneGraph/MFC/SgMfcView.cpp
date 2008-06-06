
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 
//  SgMfcView.cpp: An interactive view class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgMfcPrecompiled.h"
#include "SgMfcResource.h" // Resource ID's
#include "SgMfcView.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "SceneGraph/Core/SgMessageIds.h"
# include "Standard/SlBitmask.h"
# include "Standard/SlSystem.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CadKit;

IMPLEMENT_DYNCREATE(SgMfcView, CView)

BEGIN_MESSAGE_MAP(SgMfcView, CView)
  //{{AFX_MSG_MAP(SgMfcView)
  ON_WM_SIZE()
  ON_WM_ERASEBKGND()
  ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMfcView::SgMfcView() : CView(), 
  _viewer ( NULL ),
  _cursor ( NULL )
{
  SL_PRINT2 ( "SgMfcView::SgMfcView(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMfcView::~SgMfcView()
{
  SL_PRINT2 ( "SgMfcView::~SgMfcView(), this = %X\n", this );

  // The _viewer is a reference-pointer, no need to do anything.
}


/////////////////////////////////////////////////////////////////////////////
//
//  If we do not have a renderer then we erase the background. 
//
//  Note: If we don't have this function then we get a white flicker before 
//  the view is painted with the scene.
//
/////////////////////////////////////////////////////////////////////////////

BOOL SgMfcView::OnEraseBkgnd ( CDC *cdc ) 
{
  if ( _viewer && _viewer->getRenderer() ) 
    return FALSE;
  else 
    return CView::OnEraseBkgnd ( cdc );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::setViewer ( SgViewer *viewer ) 
{
  SL_ASSERT ( this );

  // Automatically does a ref/unref.
  _viewer = viewer;

  // If we have a good viewer...
  if ( _viewer )
  {
    // Hook up callback.
    _viewer->setCallback ( &SgMfcView::_viewerCallback, this );

    // Set the default mouse button interaction.
    _viewer->setMouseButtonsForRotating    ( SgViewer::MOUSE_LEFT );
    _viewer->setMouseButtonsForTranslating ( 2 == SlSystem::getNumMouseButtons() ? SgViewer::MOUSE_RIGHT : SgViewer::MOUSE_MIDDLE );
    _viewer->setMouseButtonsForScaling     ( 2 == SlSystem::getNumMouseButtons() ? SgViewer::MOUSE_LEFT | SgViewer::MOUSE_RIGHT : SgViewer::MOUSE_LEFT | SgViewer::MOUSE_MIDDLE );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called by the viewer.
//
/////////////////////////////////////////////////////////////////////////////

bool SgMfcView::_viewerCallback ( SgViewer::Message &message ) 
{
  // Get the client data.
  SgMfcView *me = (SgMfcView *) message.getClientData();
  SL_ASSERT ( me );

  // Call the other one.
  return me->_viewerMessage ( message );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Process the message from the viewer.
//
/////////////////////////////////////////////////////////////////////////////

bool SgMfcView::_viewerMessage ( SgViewer::Message &message ) 
{
  SL_ASSERT ( this );

  // Process the message.
  switch ( message.getId() )
  {
  case SG_VIEWER_ROTATING:

    // Set the appropriate cursor.
    _cursor = ::AfxGetApp()->LoadCursor ( IDC_SG_MFC_ROTATE_CURSOR );
    break;

  case SG_VIEWER_TRANSLATING:

    // Set the appropriate cursor.
    _cursor = ::AfxGetApp()->LoadCursor ( IDC_SG_MFC_TRANSLATE_CURSOR );
    break;

  case SG_VIEWER_SCALING:

    // Set the appropriate cursor.
    _cursor = ::AfxGetApp()->LoadCursor ( IDC_SG_MFC_SCALE_CURSOR );
    break;

  case SG_VIEWER_VIEWING_MODE:

    // Set the appropriate cursor.
    _cursor = ::AfxGetApp()->LoadCursor ( IDC_SG_MFC_ROTATE_CURSOR );
    break;

  case SG_VIEWER_PICKING_MODE:

    // Set the appropriate cursor.
    _cursor = ::AfxGetApp()->LoadStandardCursor ( IDC_ARROW );
    break;

  case SG_VIEWER_SEEKING_MODE:

    // Set the appropriate cursor.
    _cursor = ::AfxGetApp()->LoadCursor ( IDC_SG_MFC_SEEK_CURSOR );
    break;

  case SG_VIEWER_SPIN_START:

    // Set the timer.
    this->SetTimer ( (unsigned int) this, message.getCallData()->getSpinTimerInterval(), &SgMfcView::_timerCallback );
    break;

  case SG_VIEWER_SPIN_MOVE:

    // Do nothing.
    break;

  case SG_VIEWER_SPIN_FINISH:

    // Stop the motion.
    this->stopMotion();
    break;

  case SG_VIEWER_SEEK_MISS:
  case SG_VIEWER_SEEK_HIT:

    // Do nothing.
    break;

  default:

    SL_ASSERT ( 0 ); // What did we miss?
    break;
  }

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called by the timer.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::_timerCallback ( HWND hWnd, UINT message, UINT timerId, DWORD systemTime )
{
  SgMfcView *me = (SgMfcView *) timerId;
  me->getViewer()->spinNotify();
  me->render();
}


/////////////////////////////////////////////////////////////////////////////
//
//  This gets called when the user resized the window. It also gets called
//  at startup.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnSize ( UINT nType, int width, int height ) 
{
  SL_ASSERT ( this );

  // Ignore the initial call to this function and when it's minimized.
  if ( nType == SIZE_MINIMIZED || width <= 0 || height <= 0 ) 
    return;

  // If we have a viewer...
  if ( _viewer.isValid() )
  {
    // Call it's resize function.
    SL_VERIFY ( _viewer->resize ( width, height ) );
  }

  // Now call the base class's function.
  CView::OnSize ( nType, width, height );
}


/////////////////////////////////////////////////////////////////////////////
// 
//  This gets called when the view should redraw itself (i,e, resizing,
//  uniconify, etc).
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnDraw ( CDC *pDC )
{
  SL_ASSERT ( this );
  ASSERT_VALID ( this->GetDocument() );

  // If we are printing or doing a print preview...
  if ( pDC->IsPrinting() )
  {
    SL_ASSERT ( 0 ); // TODO.
  }

  // Otherwise, we are drawing to the screen.
  else
  {
    // Render the scene.
    this->render();
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse moved.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnMouseMove ( UINT nFlags, CPoint point ) 
{
  SL_ASSERT ( this );

  // If there's a viewer...
  if ( _viewer.isValid() )
  {
    // Determine what buttons are down.
    unsigned long buttonsDown = SgViewer::mapMouseEventBitFlags ( nFlags );

    // Tell the viewer that the mouse moved.
    SL_VERIFY ( _viewer->mouseMoved ( point.x, point.y, buttonsDown ) );

    //static int count = 0; TRACE ( "In SgMfcView::OnMouseMove(), x = %3d, y = %3d, buttonsDown = %3d, count = %3d\n", point.x, point.y, buttonsDown, count++ );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Render the viewer if there is one.
//
/////////////////////////////////////////////////////////////////////////////

bool SgMfcView::render()
{
  SL_ASSERT ( this );

  // If there's a viewer then render it.
  if ( _viewer.isValid() )
    return _viewer->render();

  // If not...
  return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  View the whole scene.
//
/////////////////////////////////////////////////////////////////////////////

bool SgMfcView::viewAll()
{
  SL_ASSERT ( this );

  // If there's a viewer then tell it to view everything.
  if ( _viewer.isValid() )
    return _viewer->viewAll();

  // If not...
  return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when a key is pressed when the mouse is over the window.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnKeyDown ( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
  SL_ASSERT ( this );

  // If there's a viewer then tell it a key was pressed.
  if ( _viewer.isValid() )
    SL_VERIFY ( _viewer->keyDown ( nChar ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when a key is released when the mouse is over the window.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnKeyUp ( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
  SL_ASSERT ( this );

  // If there's a viewer then tell it a key was released.
  if ( _viewer.isValid() )
    SL_VERIFY ( _viewer->keyUp ( nChar ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is pressed.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnLButtonDown(UINT nFlags, CPoint point) 
{
  // If there's a viewer tell it that the mouse is down.
  if ( _viewer.isValid() )
  {
    unsigned long buttonsDown = SgViewer::mapMouseEventBitFlags ( nFlags );
    SL_VERIFY ( _viewer->mouseButtonDown ( point.x, point.y, SgViewer::MOUSE_LEFT, buttonsDown ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is released.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnLButtonUp(UINT nFlags, CPoint point) 
{
  // If there's a viewer tell it that the mouse is up.
  if ( _viewer.isValid() )
  {
    unsigned long buttonsDown = SgViewer::mapMouseEventBitFlags ( nFlags );
    SL_VERIFY ( _viewer->mouseButtonUp ( point.x, point.y, SgViewer::MOUSE_LEFT, buttonsDown ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the middle mouse button is pressed.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnMButtonDown(UINT nFlags, CPoint point) 
{
  // If there's a viewer tell it that the mouse is down.
  if ( _viewer.isValid() )
  {
    unsigned long buttonsDown = SgViewer::mapMouseEventBitFlags ( nFlags );
    SL_VERIFY ( _viewer->mouseButtonDown ( point.x, point.y, SgViewer::MOUSE_MIDDLE, buttonsDown ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the middle mouse button is released.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnMButtonUp(UINT nFlags, CPoint point) 
{
  // If there's a viewer tell it that the mouse is up.
  if ( _viewer.isValid() )
  {
    unsigned long buttonsDown = SgViewer::mapMouseEventBitFlags ( nFlags );
    SL_VERIFY ( _viewer->mouseButtonUp ( point.x, point.y, SgViewer::MOUSE_MIDDLE, buttonsDown ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the right mouse button is pressed.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnRButtonDown(UINT nFlags, CPoint point) 
{
  // If there's a viewer tell it that the mouse is down.
  if ( _viewer.isValid() )
  {
    unsigned long buttonsDown = SgViewer::mapMouseEventBitFlags ( nFlags );
    SL_VERIFY ( _viewer->mouseButtonDown ( point.x, point.y, SgViewer::MOUSE_RIGHT, buttonsDown ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the right mouse button is released.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnRButtonUp(UINT nFlags, CPoint point) 
{
  // If there's a viewer tell it that the mouse is up.
  if ( _viewer.isValid() )
  {
    unsigned long buttonsDown = SgViewer::mapMouseEventBitFlags ( nFlags );
    SL_VERIFY ( _viewer->mouseButtonUp ( point.x, point.y, SgViewer::MOUSE_RIGHT, buttonsDown ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called when the cursor needs to be set.
//
/////////////////////////////////////////////////////////////////////////////

BOOL SgMfcView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  // If we don't have a valid cursor then pass the message on.
  if ( NULL == _cursor )
    return CView::OnSetCursor ( pWnd, nHitTest, message );

  // Set the current cursor.
  ::SetCursor ( _cursor );

  // We handled this message.
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Stop the motion. This does nothing if it's not spinning.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::stopMotion()
{
  // Kill the timer (may not be one).
  this->KillTimer ( (unsigned int) this );
}


/////////////////////////////////////////////////////////////////////////////
//
//	Clean up here. If you wait for the destructor then the virtual tables
//  are no longer valid.
//
/////////////////////////////////////////////////////////////////////////////

void SgMfcView::OnDestroy() 
{
  // This will release the viewer's pointer and unreference it.
  _viewer.setValue ( NULL );

  // Call the base class's function.
	CView::OnDestroy();
}
