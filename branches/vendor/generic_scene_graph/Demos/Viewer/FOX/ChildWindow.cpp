
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI Child window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "ChildWindow.h"
#include "MainWindow.h"

#include "GSG/FOX/View.h"

FXIMPLEMENT ( ChildWindow, ChildWindow::BaseClass, 0x0, 0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChildWindow::ChildWindow() : BaseClass()
{
  // Don't use this constructor.
  GSG::ErrorChecker ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChildWindow::ChildWindow ( const FXString &name, MainWindow *parent ) : 
  BaseClass ( parent->clientArea(), name, NULL, parent->mdiMenu(), 0, 20, 20, 400, 300 )
{
  // Build the scene.
  GSG::Factory::ValidPtr factory ( new GSG::Factory );
  this->root()->append ( factory->sphere() );
  this->view()->viewAll();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChildWindow::~ChildWindow()
{
  // Nothing to delete.
}