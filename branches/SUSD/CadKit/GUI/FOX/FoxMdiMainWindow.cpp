
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

#include "FoxMdiMainWindow.h"
#include "FoxDefine.h"

#include "Standard/SlAssert.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FoxMdiMainWindow ) WindowMap[] = 
{
  // Message_Type          ID                        Message_Handler.
  FXMAPFUNC ( SEL_COMMAND, FoxMdiMainWindow::ID_NEW, FoxMdiMainWindow::onCommandNew ),
};

FXIMPLEMENT ( FoxMdiMainWindow, FoxMainWindow, WindowMap, ARRAYNUMBER ( WindowMap ) );


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

FoxMdiMainWindow::FoxMdiMainWindow() : FoxMainWindow(), INITIALIZER_LIST
{
  // Don't use this constructor.
  SL_ASSERT ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxMdiMainWindow::FoxMdiMainWindow ( 
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
  FoxMainWindow ( app, name, icon, mi, options, x, y, w, h, pl, pr, pt, pb, hs, vs ), 
  INITIALIZER_LIST
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxMdiMainWindow::~FoxMdiMainWindow()
{
  // Make sure we release these pointers because FOX is going to delete them.
  _clientArea.release();
  _mdiMenu.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize. Repeated calls should work (I think).
//
///////////////////////////////////////////////////////////////////////////////

bool FoxMdiMainWindow::init()
{
  // Call the base class's function first.
  GUI_FOX_CHECK_EXPRESSION ( true == FoxMainWindow::init() );

  // MDI client area.
  _clientArea = new FXMDIClient ( this, LAYOUT_FILL_X | LAYOUT_FILL_Y );
  GUI_FOX_CHECK_EXPRESSION ( _clientArea.isValid() );

  // Make MDI Menu
  _mdiMenu = new FXMDIMenu ( this, _clientArea );
  GUI_FOX_CHECK_EXPRESSION ( _mdiMenu.isValid() );

  // MDI buttons in menu.
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMDIWindowButton   ( _menuBar, _mdiMenu, _clientArea, FXMDIClient::ID_MDI_MENUWINDOW,   LAYOUT_LEFT ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMDIDeleteButton   ( _menuBar, _clientArea, FXMDIClient::ID_MDI_MENUCLOSE,    FRAME_RAISED | LAYOUT_RIGHT ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMDIRestoreButton  ( _menuBar, _clientArea, FXMDIClient::ID_MDI_MENURESTORE,  FRAME_RAISED | LAYOUT_RIGHT ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMDIMinimizeButton ( _menuBar, _clientArea, FXMDIClient::ID_MDI_MENUMINIMIZE, FRAME_RAISED | LAYOUT_RIGHT ) );

  // File menu.
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _fileMenu, "&New\tCtl-N\tCreate a new document.", NULL, this, FoxMdiMainWindow::ID_NEW ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _fileMenu, "&Quit\tCtl-Q\tQuit the application.", NULL, this->getApp(), FXApp::ID_QUIT ) );

  // Window menu
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, "Tile &Horizontally",NULL, _clientArea, FXMDIClient::ID_MDI_TILEHORIZONTAL ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, "Tile &Vertically",  NULL, _clientArea, FXMDIClient::ID_MDI_TILEVERTICAL ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, "C&ascade",          NULL, _clientArea, FXMDIClient::ID_MDI_CASCADE ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, "&Close",            NULL, _clientArea, FXMDIClient::ID_MDI_CLOSE ) );
  
  // This seems to have been depreciated.
  //GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, "Close &All",        NULL, _clientArea, FXMDIClient::ID_CLOSE_ALL_DOCUMENTS ) );

  FXMenuSeparator *separator = new FXMenuSeparator ( _windowMenu );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != separator );
  separator->setTarget ( _clientArea );
  separator->setSelector ( FXMDIClient::ID_MDI_ANY );

  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, NULL, NULL, _clientArea, FXMDIClient::ID_MDI_1 ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, NULL, NULL, _clientArea, FXMDIClient::ID_MDI_2 ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, NULL, NULL, _clientArea, FXMDIClient::ID_MDI_3 ) );
  GUI_FOX_CHECK_EXPRESSION ( 0x0 != new FXMenuCommand ( _windowMenu, NULL, NULL, _clientArea, FXMDIClient::ID_MDI_4 ) );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "New" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMdiMainWindow::onCommandNew ( FXObject *, FXSelector, void * )
{
  SL_ASSERT ( this );
  SL_ASSERT ( _clientArea.isValid() );
  SL_ASSERT ( _mdiMenu.isValid() );

  static unsigned int count ( 0 );
  char name[512];
  ::sprintf ( name, "Child %d", count++ );

  FXMDIChild *child = new FXMDIChild ( _clientArea, name, NULL, _mdiMenu, 0, 20, 20, 300, 200 );
  child->create();

  return 1;
}
