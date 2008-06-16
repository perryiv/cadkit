
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
//  The ClipPlanesComponent class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Plugins/ClipPlanes/ClipPlanesComponent.h"
#include "Helios/Qt/Plugins/ClipPlanes/ClipPlanes.h"

#include "Usul/Components/Factory.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"

#include "Usul/Documents/Manager.h"

#include "osg/Node"
#include "osg/Drawable"
#include "osg/Group"
#include "osg/Geode"

#include "QtGui/QDockWidget"
#include "QtGui/QHeaderView"
#include "QtGui/QMainWindow"
#include "QtGui/QTreeWidget"

USUL_DECLARE_COMPONENT_FACTORY ( ClipPlanesComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ClipPlanesComponent, ClipPlanesComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipPlanesComponent::ClipPlanesComponent() : BaseClass(),
  _caller(),
  _dock ( 0x0 ),
  _clipPlanes ( 0x0 )
{
  // We want to be notified when the active view changes.
  Usul::Documents::Manager::instance().addActiveViewListener ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipPlanesComponent::~ClipPlanesComponent()
{
  // Remove this from the list of active view listeners.
  Usul::Documents::Manager::instance().removeActiveViewListener ( this );

  if ( 0x0 != _clipPlanes )
  {
    // Clear the ClipPlanes
    _clipPlanes->clear();
  }

  if ( 0x0 != _dock )
  {
    Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

    if( mainWindow.valid() )
    {
      // Remove the DockWidget from the MainWindow.
      QMainWindow * main ( mainWindow->mainWindow() );
      main->removeDockWidget ( _dock );
    }

    _dock->setWidget ( 0x0 );

    // Delete the DockWidget.
    delete _dock;   
  }

  //// Qt deletes the clip planes.
  _clipPlanes = 0x0;
  _dock = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ClipPlanesComponent::queryInterface ( unsigned long iid )
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

void ClipPlanesComponent::initializePlugin ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  if ( mainWindow.valid() )
  {
    QMainWindow * main ( mainWindow->mainWindow() );

    // Build the docking window.
    _dock = new QDockWidget ( QObject::tr ( "Cross Section Tool" ), main );
    _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    // Create the widget
    _clipPlanes = new ClipPlanes ( _dock );

    // Add the dock to the main window.
    _dock->setWidget( _clipPlanes );
    main->addDockWidget ( Qt::LeftDockWidgetArea, _dock );

    // Set the object name.
    _dock->setObjectName ( "ClipPlanesWidget" );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( _dock );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanesComponent::activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView )
{
  if ( 0x0 != _clipPlanes )
    _clipPlanes->build ( newView );
}
