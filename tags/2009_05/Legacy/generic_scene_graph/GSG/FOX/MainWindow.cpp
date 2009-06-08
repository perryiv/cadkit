
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/FOX/Precompiled.h"
#include "GSG/FOX/MainWindow.h"

#include "GSG/Core/Config.h"

#include "fx.h"

using namespace GSG;
using namespace GSG::FOX;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( MainWindow ) WindowMap[] = 
{
  // Message_Type          ID                       Message_Handler.
  FXMAPFUNC ( SEL_COMMAND, MainWindow::ID_ABOUT, MainWindow::onCommandAbout ),
};

FXIMPLEMENT ( MainWindow, FXMainWindow, WindowMap, ARRAYNUMBER ( WindowMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIALIZER_LIST \
  _menuBar    ( 0x0 ),\
  _statusBar  ( 0x0 ),\
  _fileMenu   ( 0x0 ),\
  _windowMenu ( 0x0 ),\
  _helpMenu   ( 0x0 )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : FXMainWindow(), 
  INITIALIZER_LIST
{
  // Do not use this constructor.
  ErrorChecker ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow ( 
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
  // Menu bar.
  unsigned int layout ( LAYOUT_SIDE_TOP | LAYOUT_FILL_X );
  _menuBar = FXMenuBarPtr ( new FXMenuBar ( this, layout ) );
  ErrorChecker ( 0x0 != _menuBar.get() );

  // File menu.
  _fileMenu = FXMenuPanePtr ( new FXMenuPane ( this ) );
  ErrorChecker ( 0x0 != _fileMenu.get() );
  ErrorChecker ( 0x0 != new FXMenuTitle ( _menuBar.get(), "&File", 0x0, _fileMenu.get() ) );

  // Status bar.
  layout = LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER;
  _statusBar = FXStatusBarPtr ( new FXStatusBar ( this, layout ) );
  ErrorChecker ( 0x0 != _statusBar.get() );

  // Window menu
  _windowMenu = FXMenuPanePtr ( new FXMenuPane ( this ) );
  ErrorChecker ( 0x0 != _windowMenu.get() );
  ErrorChecker ( 0x0 != new FXMenuTitle ( _menuBar.get(), "&Window", 0x0, _windowMenu.get() ) );

  // Help menu
  _helpMenu = FXMenuPanePtr ( new FXMenuPane ( this ) );
  ErrorChecker ( 0x0 != _helpMenu.get() );
  ErrorChecker ( 0x0 != new FXMenuTitle ( _menuBar.get(), "&Help", 0x0, _helpMenu.get(), LAYOUT_LEFT ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( _helpMenu.get(), "&About...", 0x0, this, MainWindow::ID_ABOUT, 0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "About" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandAbout ( FXObject *, FXSelector, void * )
{
  FXString defaultName ( "FOX-Binding for Generic Scene Graph" );
  FXString appName ( this->getApp()->getAppName() );
  FXString text ( appName.empty() ? defaultName.text() : appName.text() );
  FXMessageBox::information ( this, MBOX_OK, "About", text.text() );
  return 1; // Handled
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is it created?
//
///////////////////////////////////////////////////////////////////////////////

bool MainWindow::isCreated() const
{
  return ( 0x0 != this->id() );
}
