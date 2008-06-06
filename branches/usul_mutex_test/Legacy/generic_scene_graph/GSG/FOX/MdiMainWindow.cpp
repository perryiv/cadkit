
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI Main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/FOX/Precompiled.h"
#include "GSG/FOX/MdiMainWindow.h"
#include "GSG/FOX/MdiChildWindow.h"

#include "GSG/Core/Config.h"

#include "fx.h"

using namespace GSG;
using namespace GSG::FOX;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( MdiMainWindow ) WindowMap[] = 
{
  // Message_Type          ID                     Message_Handler.
  FXMAPFUNC ( SEL_COMMAND, MdiMainWindow::ID_NEW, MdiMainWindow::onCommandNew ),
};

FXIMPLEMENT ( MdiMainWindow, MainWindow, WindowMap, ARRAYNUMBER ( WindowMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIALIZER_LIST \
  _clientArea ( 0x0 ), \
  _mdiMenu    ( 0x0 )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiMainWindow::MdiMainWindow() : MainWindow(), 
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

MdiMainWindow::MdiMainWindow ( 
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
  MainWindow ( app, name, icon, mi, options, x, y, w, h, pl, pr, pt, pb, hs, vs ), 
  INITIALIZER_LIST
{
  // MDI client area.
  _clientArea = FXMDIClientPtr ( new FXMDIClient ( this, LAYOUT_FILL_X | LAYOUT_FILL_Y ) );
  ErrorChecker ( 0x0 != _clientArea.get() );

  // Make MDI Menu
  _mdiMenu = FXMDIMenuPtr ( new FXMDIMenu ( this, _clientArea.get() ) );
  ErrorChecker ( 0x0 != _mdiMenu.get() );

  // MDI buttons in menu.
  ErrorChecker ( 0x0 != new FXMDIWindowButton   ( this->menuBar(), _mdiMenu.get(), _clientArea.get(), FXMDIClient::ID_MDI_MENUWINDOW,   LAYOUT_LEFT ) );
  ErrorChecker ( 0x0 != new FXMDIDeleteButton   ( this->menuBar(), _clientArea.get(), FXMDIClient::ID_MDI_MENUCLOSE,    FRAME_RAISED | LAYOUT_RIGHT ) );
  ErrorChecker ( 0x0 != new FXMDIRestoreButton  ( this->menuBar(), _clientArea.get(), FXMDIClient::ID_MDI_MENURESTORE,  FRAME_RAISED | LAYOUT_RIGHT ) );
  ErrorChecker ( 0x0 != new FXMDIMinimizeButton ( this->menuBar(), _clientArea.get(), FXMDIClient::ID_MDI_MENUMINIMIZE, FRAME_RAISED | LAYOUT_RIGHT ) );

  // File menu.
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->fileMenu(), "&New\tCtl-N\tCreate a new document.", 0x0, this, MdiMainWindow::ID_NEW ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->fileMenu(), "&Quit\tCtl-Q\tQuit the application.", 0x0, this->getApp(), FXApp::ID_QUIT ) );

  // Window menu
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), "Tile &Horizontally",0x0, _clientArea.get(), FXMDIClient::ID_MDI_TILEHORIZONTAL ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), "Tile &Vertically",  0x0, _clientArea.get(), FXMDIClient::ID_MDI_TILEVERTICAL ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), "C&ascade",          0x0, _clientArea.get(), FXMDIClient::ID_MDI_CASCADE ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), "&Close",            0x0, _clientArea.get(), FXMDIClient::ID_MDI_CLOSE ) );
  
  // This seems to have been depreciated.
  //ErrorChecker ( 0x0 != new FXMenuCommand ( _windowMenu, "Close &All",        0x0, _clientArea, FXMDIClient::ID_CLOSE_ALL_DOCUMENTS ) );

  FXMenuSeparator *separator = new FXMenuSeparator ( this->windowMenu() );
  ErrorChecker ( 0x0 != separator );
  separator->setTarget ( _clientArea.get() );
  separator->setSelector ( FXMDIClient::ID_MDI_ANY );

  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), 0x0, 0x0, _clientArea.get(), FXMDIClient::ID_MDI_1 ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), 0x0, 0x0, _clientArea.get(), FXMDIClient::ID_MDI_2 ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), 0x0, 0x0, _clientArea.get(), FXMDIClient::ID_MDI_3 ) );
  ErrorChecker ( 0x0 != new FXMenuCommand ( this->windowMenu(), 0x0, 0x0, _clientArea.get(), FXMDIClient::ID_MDI_4 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiMainWindow::~MdiMainWindow()
{
  // Make sure we release these pointers because FOX is going to delete them.
  _clientArea.release();
  _mdiMenu.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "New" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MdiMainWindow::onCommandNew ( FXObject *, FXSelector, void * )
{
  ErrorChecker ( 0x0 != _clientArea.get() );
  ErrorChecker ( 0x0 != _mdiMenu.get() );

  static unsigned int count ( 0 );
  char name[512];
  ::sprintf ( name, "Child %d", count++ );

  MdiChildWindow *child = new MdiChildWindow ( _clientArea.get(), name, 0x0, _mdiMenu.get(), 0, 20, 20, 300, 200 );
  child->create();

  return 1;
}
