
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

#include "FoxMainWindow.h"
#include "FoxDefine.h"

#include "Standard/SlAssert.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FoxMainWindow ) WindowMap[] = 
{
  // Message_Type          ID                       Message_Handler.
  FXMAPFUNC ( SEL_COMMAND, FoxMainWindow::ID_ABOUT, FoxMainWindow::onCommandAbout ),
};

FXIMPLEMENT ( FoxMainWindow, FXMainWindow, WindowMap, ARRAYNUMBER ( WindowMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIALIZER_LIST \
  _menuBar    ( 0x0 ), \
  _statusBar  ( 0x0 ), \
  _fileMenu   ( 0x0 ), \
  _windowMenu ( 0x0 ), \
  _helpMenu   ( 0x0 )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxMainWindow::FoxMainWindow() : FXMainWindow(), INITIALIZER_LIST
{
  // Don't use this constructor.
  SL_ASSERT ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxMainWindow::FoxMainWindow ( 
  FXApp *app,
  const FXString &name,
  FXIcon *icon,
  FXIcon *mi,
  FXuint options,
  FXint x,
  FXint y,
  FXint w,
  FXint h,
  FXint pl,
  FXint pr,
  FXint pt,
  FXint pb,
  FXint hs,
  FXint vs ) : 
  FXMainWindow ( app, name, icon, mi, options, x, y, w, h, pl, pr, pt, pb, hs, vs ),
  INITIALIZER_LIST
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxMainWindow::~FoxMainWindow()
{
  // Make sure we release these pointers because FOX is going to delete them.
  _statusBar.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize. Repeated calls should work (I think).
//
///////////////////////////////////////////////////////////////////////////////

bool FoxMainWindow::init()
{
  // Menubar.
  _menuBar = new FXMenuBar ( this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X );
  GUI_FOX_CHECK_EXPRESSION ( _menuBar.isValid() );

  // Status bar.
  _statusBar = new FXStatusBar ( this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER );
  GUI_FOX_CHECK_EXPRESSION ( _statusBar.isValid() );

  // File menu.
  _fileMenu = new FXMenuPane ( this );
  GUI_FOX_CHECK_EXPRESSION ( _fileMenu.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuTitle ( _menuBar, "&File", NULL, _fileMenu ) );

  // Window menu
  _windowMenu = new FXMenuPane ( this );
  GUI_FOX_CHECK_EXPRESSION ( _windowMenu.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuTitle ( _menuBar, "&Window", NULL, _windowMenu ) );

  // Help menu
  _helpMenu = new FXMenuPane ( this );
  GUI_FOX_CHECK_EXPRESSION ( _helpMenu.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _helpMenu, "&About...", NULL, this, FoxMainWindow::ID_ABOUT, 0 ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuTitle ( _menuBar, "&Help", NULL, _helpMenu, LAYOUT_LEFT ) );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "About" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMainWindow::onCommandAbout ( FXObject *, FXSelector, void * )
{
  SL_ASSERT ( this );

  const char *noName = "The mysterious application with no name...";
  FXString name ( this->getApp()->getAppName() );
  FXMessageBox::information ( this, MBOX_OK, "About", name.empty() ? noName : name.text() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the window. Keep this function, if for no reason other than the 
//  comment in the header file.
//
///////////////////////////////////////////////////////////////////////////////

void FoxMainWindow::create()
{
  // Call the base class's function.
  FXMainWindow::create();
}
