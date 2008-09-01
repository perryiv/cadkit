
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL canvas for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgFoxCanvas.h"

#include "GUI/FOX/FoxDefine.h"

#include "Standard/SlAssert.h"

using namespace CadKit;


//////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
//////////////////////////////////////////////////////////////////////////

FXDEFMAP ( SgFoxCanvas ) SgFoxCanvasMap[] =
{
  // Message type, id, and handler.
  FXMAPFUNC ( SEL_LEFTBUTTONPRESS, 0, SgFoxCanvas::onLeftBtnPress ),
};

// This class, base class, array of message handlers, size of the array.
FXIMPLEMENT ( SgFoxCanvas, FXGLCanvas, SgFoxCanvasMap, ARRAYNUMBER ( SgFoxCanvasMap ) );


//////////////////////////////////////////////////////////////////////////
//
//  Default constructor (needed for FXIMPLEMENT macro).
//
//////////////////////////////////////////////////////////////////////////

SgFoxCanvas::SgFoxCanvas() : FXGLCanvas()
{
  // Don't use this constructor.
  SL_ASSERT ( 0 );
}


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

SgFoxCanvas::SgFoxCanvas ( FXComposite *parent, FXGLVisual *visual ) :
  FXGLCanvas ( parent, visual )
{
  // Empty.
}


//////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
//////////////////////////////////////////////////////////////////////////

SgFoxCanvas::~SgFoxCanvas()
{
  // Nothing to delete.
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is pressed.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxCanvas::onLeftBtnPress ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Pop this window to the front.
  this->setFocus();

  // Call the base class's function.
  return FXGLCanvas::onLeftBtnPress ( object, selector, callData );
}