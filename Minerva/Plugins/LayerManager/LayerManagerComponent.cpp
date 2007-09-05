
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/LayerManagerComponent.h"
#include "Minerva/Plugins/LayerManager/LayersTree.h"

#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"

#include "Usul/Documents/Manager.h"

#include "QtGui/QDockWidget"
#include "QtGui/QMainWindow"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( LayerManagerComponent, LayerManagerComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

LayerManagerComponent::LayerManagerComponent() : BaseClass (),
_caller ( static_cast < Usul::Interfaces::IUnknown * > ( 0x0 ) ),
_dock ( 0x0 ),
_layers ( 0x0 )
{
  // We want to be notified when the active document changes.
  Usul::Documents::Manager::instance().addActiveDocumentListener ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

LayerManagerComponent::~LayerManagerComponent() 
{
  // Remove this from the list of active document listeners.
  Usul::Documents::Manager::instance().removeActiveDocumentListener ( this );

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

  if( mainWindow.valid() )
  {
    // Remove the DockWidget from the MainWindow.
    QMainWindow * main  ( mainWindow->mainWindow() );
    main->removeDockWidget ( _dock );
  }

  _dock->setWidget ( 0x0 );

  // Delete the DockWidget.
  delete _dock;
  _dock = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *LayerManagerComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IActiveDocumentListener::IID:
    return static_cast < Usul::Interfaces::IActiveDocumentListener * > ( this );
  case Usul::Interfaces::IAddDockWindow::IID:
    return static_cast < Usul::Interfaces::IAddDockWindow * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name of this plugin.
//
///////////////////////////////////////////////////////////////////////////////

std::string LayerManagerComponent::getPluginName() const 
{
  return "Layer Manager";
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void LayerManagerComponent::activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a dock window.
//
///////////////////////////////////////////////////////////////////////////////

void LayerManagerComponent::addDockWindow ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  if ( mainWindow.valid() )
  {
    QMainWindow * main ( mainWindow->mainWindow() );

    // Build the docking window.
    _dock = new QDockWidget ( QObject::tr ( "Layers" ), main );
    _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    // Create the tree for the scene graph.
    _layers = new LayersTree ( _dock );

    // Add the dock to the main window.
    _dock->setWidget( _layers );
    main->addDockWidget ( Qt::LeftDockWidgetArea, _dock );

    // Set the object name.
    _dock->setObjectName ( "LayersDockWidget" );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( _dock );
  }
}
