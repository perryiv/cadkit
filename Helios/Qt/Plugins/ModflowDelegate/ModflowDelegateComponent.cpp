
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegateComponent.h"
#include "LayerTreeControl.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Documents/Document.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IModifiedObserver.h"
#include "Usul/Interfaces/IModifiedSubject.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QDockWidget"
#include "QtGui/QMainWindow"
#include "QtGui/QWorkspace"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModflowDelegateComponent , ModflowDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDelegateComponent::ModflowDelegateComponent() : BaseClass(),
  _caller ( static_cast < Usul::Interfaces::IUnknown * > ( 0x0 ) ),
  _docked(),
  _layerTree ( 0x0 )
{
  USUL_TRACE_SCOPE;

  // We want to be notified when the active document changes.
  Usul::Documents::Manager::instance().addActiveDocumentListener ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDelegateComponent::~ModflowDelegateComponent()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ModflowDelegateComponent::_destroy ), "7258451600" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::_destroy()
{
  USUL_TRACE_SCOPE;

  // Remove this from the list of active document listeners.
  Usul::Documents::Manager::instance().removeActiveDocumentListener ( this );

  // Needed below.
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mw ( _caller );
  QMainWindow *mainWindow ( ( mw.valid() ) ? mw->mainWindow() : 0x0 );

  // Loop through the docking widgets.
  for ( DockedMap::iterator i = _docked.begin(); i != _docked.end(); ++i )
  {
    QDockWidget *dockWidget ( i->second );
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

  // Clear the map.
  _docked.clear();

  // This was just deleted by it's parent.
  _layerTree = 0x0;

  // Done with this.
  _caller = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModflowDelegateComponent::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate * > ( this );
  case Usul::Interfaces::IActiveDocumentListener::IID:
    return static_cast < Usul::Interfaces::IActiveDocumentListener * > ( this );
  case Usul::Interfaces::IAddDockWindow::IID:
    return static_cast < Usul::Interfaces::IAddDockWindow * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token
//
/////////////////////////////////////////////////////////////////////////////

bool ModflowDelegateComponent::doesHandle ( const std::string& token ) const
{
  USUL_TRACE_SCOPE;
  return ( token == "Modflow Document" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  typedef CadKit::Helios::Views::OSG::Viewer QtViewer;

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;

  Usul::Interfaces::Qt::IWorkspace::QueryPtr workspace ( caller );

  if ( workspace.valid() )
  {
    QWorkspace *parent ( workspace->workspace() );
    QtViewerPtr viewer ( new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent ) );
    parent->addWindow ( viewer.get() );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid () )
    {
      viewer->viewer()->scene ( build->buildScene ( document->options(), caller ) );
    }

    // Show the window.
    viewer->show();

    // Make sure the geometry is visible.
    viewer->viewer()->camera ( OsgTools::Render::Viewer::FIT );

    // Attach viewer as a listener to the document.
    Usul::Interfaces::IModifiedObserver::QueryPtr observer ( viewer->viewer() );
    Usul::Interfaces::IModifiedSubject::QueryPtr subject ( document );
    if ( ( true == observer.valid() ) && ( true == subject.valid() ) )
    {
      subject->addModifiedObserver ( observer.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc )
{
  if ( 0x0 != _layerTree )
    _layerTree->buildTree ( newDoc );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a dock window.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::addDockWindow ( Usul::Interfaces::IUnknown *caller )
{
  // Make sure this is only called once.
  if ( false == _docked.empty() )
    return;

  // Set the caller.
  _caller = caller;

  // Build the docking window.
  std::auto_ptr<QDockWidget> dockWidget ( this->_makeDockWindow ( "Modflow Layers", "ModflowDockWidget" ) );

  // Create the tree for the scene graph.
  _layerTree = new LayerTreeControl ( caller, dockWidget.get() );

  // Set the docking window's widget.
  dockWidget->setWidget ( _layerTree );

  // Save in the map and release.
  _docked[_layerTree] = dockWidget.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a dock window.
//
///////////////////////////////////////////////////////////////////////////////

QDockWidget *ModflowDelegateComponent::_makeDockWindow ( const std::string &title, const std::string &name )
{
  // Need a main window.
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );
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
  Usul::Interfaces::IQtDockWidgetMenu::QueryPtr menu ( _caller );
  if ( true == menu.valid() )
    menu->addDockWidgetMenu ( dockWidget.get() );

  // Return the dock widget.
  return dockWidget.release();
}
