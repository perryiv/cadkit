
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
#include "Minerva/Plugins/LayerManager/Favorites.h"

#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"
#include "Usul/Interfaces/IModifiedSubject.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Components/Factory.h"

#include "QtGui/QDockWidget"
#include "QtGui/QMainWindow"

USUL_DECLARE_COMPONENT_FACTORY ( LayerManagerComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( LayerManagerComponent, LayerManagerComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

LayerManagerComponent::LayerManagerComponent() : BaseClass (),
_caller ( static_cast < Usul::Interfaces::IUnknown * > ( 0x0 ) ),
_docks (),
_layers ( 0x0 ),
_favorites ( 0x0 )
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

  // Needed below.
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mw ( _caller );
  QMainWindow *mainWindow ( ( mw.valid() ) ? mw->mainWindow() : 0x0 );
  
  // Loop through the docking widgets.
  for ( Docks::iterator i = _docks.begin(); i != _docks.end(); ++i )
  {
    QDockWidget *dockWidget ( *i );
    if ( 0x0 != dockWidget )
    {
      // Remove the dock widget from the main window.
      if ( 0x0 != mainWindow )
      {
        mainWindow->removeDockWidget ( dockWidget );
      }
      
      // Make sure there is no contained widget.
      dockWidget->setWidget ( 0x0 );
      
      // Delete the DockWidget.
      delete dockWidget;
    }
  }
  
  // Clear the vector.
  _docks.clear();

  _layers = 0x0;
  _favorites = 0x0;
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
  case Usul::Interfaces::IModifiedObserver::IID:
    return static_cast < Usul::Interfaces::IModifiedObserver* > ( this );
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
  {
    Usul::Interfaces::IModifiedSubject::QueryPtr ms ( oldDoc );
    if ( ms.valid() )
      ms->removeModifiedObserver ( Usul::Interfaces::IModifiedObserver::QueryPtr ( this ) );
  }
  
  {
    Usul::Interfaces::IModifiedSubject::QueryPtr ms ( newDoc );
    if ( ms.valid() )
      ms->addModifiedObserver ( Usul::Interfaces::IModifiedObserver::QueryPtr ( this ) );
  }
  
  if ( 0x0 != _layers )
    _layers->buildTree ( newDoc );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The document is modified.
//
///////////////////////////////////////////////////////////////////////////////

void LayerManagerComponent::subjectModified ( Usul::Interfaces::IUnknown *caller )
{
  if ( 0x0 != _layers )
    _layers->buildTree ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a dock window.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  
  QDockWidget* makeDockWindow( const std::string& title, const std::string& name, Usul::Interfaces::IUnknown *caller )
  {
    // Need a main window.
    Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );
    if ( false == mainWindow.valid() )
      return 0x0;
    
    // Get the main window.
    QMainWindow *main ( mainWindow->mainWindow() );
    if ( 0x0 == main )
      return 0x0;
    
    // Build the docking window.
    std::auto_ptr<QDockWidget> dockWidget ( new QDockWidget ( QObject::tr ( title.c_str() ), main ) );
    dockWidget->setAllowedAreas ( Qt::AllDockWidgetAreas );
    
    // Add the dock to the main window.
    main->addDockWidget ( Qt::LeftDockWidgetArea, dockWidget.get() );
    
    // Set the object name.
    dockWidget->setObjectName ( name.c_str() );
    
    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr menu ( caller );
    if ( true == menu.valid() )
      menu->addDockWidgetMenu ( dockWidget.get() );
    
    // Return the dock widget.
    return dockWidget.release();
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a dock window.
//
///////////////////////////////////////////////////////////////////////////////

void LayerManagerComponent::addDockWindow ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  {
    // Build the docking window.
    std::auto_ptr<QDockWidget> dock ( Detail::makeDockWindow ( "GIS Layers", "GISLayersDockWidget", caller ) );

    // Create the tree for the layers.
    _layers = new LayersTree ( caller, dock.get() );

    // Add the dock to the main window.
    dock->setWidget( _layers );
    
    // Add to our list.
    _docks.push_back ( dock.release() );
  }
  
  {
    // Build the docking window.
    std::auto_ptr<QDockWidget> dock ( Detail::makeDockWindow ( "GIS Favorites", "GISFavoritesDockWidget", caller ) );
    
    // Create the tree for the favorites.
    _favorites = new Favorites ( caller, dock.get() );
    
    // Add the dock to the main window.
    dock->setWidget( _favorites );
    
    // Add to our list.
    _docks.push_back ( dock.release() );
  }
  
  QObject::connect ( _layers,    SIGNAL ( addLayerFavorites ( Usul::Interfaces::IUnknown * ) ), _favorites, SLOT ( addLayer ( Usul::Interfaces::IUnknown * ) ) );
  QObject::connect ( _favorites, SIGNAL ( layerAdded ( Usul::Interfaces::IUnknown * ) ),        _layers,    SLOT ( addLayer ( Usul::Interfaces::IUnknown * ) ) );
}
