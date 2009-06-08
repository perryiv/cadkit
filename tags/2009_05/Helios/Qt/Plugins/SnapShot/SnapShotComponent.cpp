
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The SnapShotComponent class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Plugins/SnapShot/SnapShotComponent.h"
#include "Helios/Qt/Plugins/SnapShot/SnapShotWidget.h"

#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"

#include "Usul/Documents/Manager.h"

#include "QtGui/QTreeWidget"
#include "QtGui/QMainWindow"
#include "QtGui/QDockWidget"
#include "QtGui/QHeaderView"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SnapShotComponent, SnapShotComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SnapShotComponent::SnapShotComponent() : BaseClass(),
  _caller(),
  _dock ( 0x0 )
{
  // We want to be notified when the active view changes.
  Usul::Documents::Manager::instance().addActiveViewListener ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SnapShotComponent::~SnapShotComponent()
{
  // Remove this from the list of active view listeners.
  Usul::Documents::Manager::instance().removeActiveViewListener ( this );

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

  if( mainWindow.valid() )
  {
    // Remove the DockWidget from the MainWindow.
    QMainWindow * main  ( mainWindow->mainWindow() );
    main->removeDockWidget ( _dock );
  }

  // Delete the DockWidget.
  delete _dock;
  _dock = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SnapShotComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPluginInitialize::IID:
    return static_cast< Usul::Interfaces::IPluginInitialize*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IActiveViewListener::IID:
    return static_cast < Usul::Interfaces::IActiveViewListener* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tab for the scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void SnapShotComponent::initializePlugin ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  if ( true == mainWindow.valid() )
  {
    QMainWindow * main  ( mainWindow->mainWindow() );

    // Build the docking window.
    _dock = new QDockWidget ( QObject::tr ( "Screenshot" ), main );
    _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    SnapShotWidget *widget = new SnapShotWidget ( _dock, _caller );

    // Add the dock to the main window.
    _dock->setWidget( widget );
    main->addDockWidget ( Qt::BottomDockWidgetArea, _dock );

    // Set the object name.
    _dock->setObjectName ( "SnapShotDockWidget" );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( _dock );
  } 
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active view has changed.
//
///////////////////////////////////////////////////////////////////////////////

void SnapShotComponent::activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView )
{
}
