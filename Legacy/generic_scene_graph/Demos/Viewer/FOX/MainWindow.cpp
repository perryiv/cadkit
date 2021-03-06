
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

#include "Precompiled.h"
#include "MainWindow.h"
#include "ChildWindow.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
/////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( MainWindow ) WindowMap[] = 
{
  // Message_Type          ID                  Message_Handler.
  FXMAPFUNC ( SEL_COMMAND, MainWindow::ID_NEW, MainWindow::onCommandNew ),
};

FXIMPLEMENT ( MainWindow, MainWindow::BaseClass, WindowMap, ARRAYNUMBER ( WindowMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : BaseClass()
{
  // Don't use this constructor.
  GSG::ErrorChecker ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow ( FXApp *app, const FXString &name, const FXint &w, const FXint &h ) : 
  BaseClass ( app, name, NULL, NULL, DECOR_ALL, 0, 0, w, h )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
  // Nothing to delete.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "New" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandNew ( FXObject *, FXSelector, void * )
{
  GSG::Lock lock ( this );
  GSG::ErrorChecker ( 0x0 != this->clientArea() );
  GSG::ErrorChecker ( 0x0 != this->mdiMenu() );

  // Number the windows.
  static unsigned int count ( 0 );
  char name[512];
  ::sprintf ( name, "Child %d", ++count );

  // Make a new window.
  std::auto_ptr<ChildWindow> child ( new ChildWindow ( name, this ) );
  child->create();

  // Detach the auto-pointer from the window. FOX is keeping track of it.
  child.release();

  // Handled.
  return 1;
}
