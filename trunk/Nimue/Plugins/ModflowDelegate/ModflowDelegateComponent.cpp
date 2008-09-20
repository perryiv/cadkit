
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/ModflowDelegateComponent.h"
#include "ModflowDelegate/Properties.h"
#include "ModflowDelegate/FileInputDialog.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "QtTools/TreeControl.h"
#include "QtTools/AnimationControl.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Components/Factory.h"
#include "Usul/Documents/Document.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IModifiedObserver.h"
#include "Usul/Interfaces/IModifiedSubject.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/System/Sleep.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QDockWidget"
#include "QtGui/QTextEdit"
#include "QtGui/QMainWindow"
#include "QtGui/QWorkspace"
#include "QtGui/QHBoxLayout"

#include <iostream>

USUL_DECLARE_COMPONENT_FACTORY ( ModflowDelegateComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModflowDelegateComponent, ModflowDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDelegateComponent::ModflowDelegateComponent() : BaseClass(),
  _caller        ( static_cast < Usul::Interfaces::IUnknown * > ( 0x0 ) ),
  _docked        (),
  _layerTree     ( 0x0 ),
  _animation     ( 0x0 ),
  _intersectInfo ( 0x0 ),
  _properties    ( 0x0 ),
  _model         ( static_cast < IModflowModel * > ( 0x0 ) )
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

  // These were just deleted by the parent.
  _layerTree = 0x0;
  _animation = 0x0;
  _intersectInfo = 0x0;
  _properties = 0x0;

  // Done with these.
  _caller = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
  _model  = static_cast < IModflowModel * > ( 0x0 );
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
  case Usul::Interfaces::IPluginInitialize::IID:
    return static_cast < Usul::Interfaces::IPluginInitialize * > ( this );
  case Usul::Interfaces::IStringGridSet::IID:
    return static_cast < Usul::Interfaces::IStringGridSet * > ( this );
  case Usul::Interfaces::IInitNewDocument::IID:
    return static_cast < Usul::Interfaces::IInitNewDocument * > ( this );
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
    QtViewerPtr viewer ( new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller ) );
    parent->addWindow ( viewer.get() );

    // We want mouse-move events even when there are no mouse buttons pressed.
    viewer->setMouseTracking ( true );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid() )
    {
      viewer->viewer()->scene ( build->buildScene ( document->options(), caller ) );
    }

    // Show the window.
    viewer->show();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc )
{
  _model = newDoc;

  if ( 0x0 != _layerTree )
  {
    _layerTree->buildTree ( newDoc );
  }

  if ( 0x0 != _intersectInfo )
  {
    _intersectInfo->clear();
  }

  if ( 0x0 != _properties )
  {
    _properties->document ( newDoc );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a dock window.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::initializePlugin ( Usul::Interfaces::IUnknown *caller )
{
  // Make sure this is only called once.
  if ( false == _docked.empty() )
    return;

  // Set the caller.
  _caller = caller;

  // Layers tree.
  {
    // Build the docking window.
    std::auto_ptr<QDockWidget> dockWidget ( this->_makeDockWindow ( "Modflow Layers", "ModflowLayersDockWidget" ) );

    // Create the tree for the scene graph.
    _layerTree = new QtTools::TreeControl ( dockWidget.get() );

    // Set the docking window's widget.
    dockWidget->setWidget ( _layerTree );

    // Save in the map and release.
    _docked[_layerTree] = dockWidget.release();
  }

  // Intersection window.
  {
    // Build the docking window.
    std::auto_ptr<QDockWidget> dockWidget ( this->_makeDockWindow ( "Modflow Intersection", "ModflowIntersectionDockWidget" ) );

    // Create outer frame.
    QFrame *frame ( new QFrame ( dockWidget.get() ) );
    frame->setFrameStyle ( QFrame::Panel | QFrame::Sunken );

    // Create the label.
    _intersectInfo = new QTextEdit;
    _intersectInfo->setReadOnly ( true );

    // Make layout.
    QHBoxLayout *layout ( new QHBoxLayout );
    layout->setMargin ( 1 );
    layout->addWidget ( _intersectInfo );

    // Set the frame's layout.
    frame->setLayout ( layout );

    // Set the docking window's widget.
    dockWidget->setWidget ( frame );

    // Save in the map and release.
    _docked[_intersectInfo] = dockWidget.release();
  }

  // Properties window.
  {
    // Build the docking window.
    std::auto_ptr<QDockWidget> dockWidget ( this->_makeDockWindow ( "Modflow Properties", "ModflowPropertiesDockWidget" ) );

    // Create the label.
    _properties = new Modflow::Properties;

    // Set the docking window's widget.
    dockWidget->setWidget ( _properties );

    // Save in the map and release.
    _docked[_properties] = dockWidget.release();
  }

  // Animation control window.
  {
    // Build the docking window.
    std::auto_ptr<QDockWidget> dockWidget ( this->_makeDockWindow ( "Modflow Animation", "ModflowAnimationDockWidget" ) );

    // Create the label.
    _animation = new QtTools::AnimationControl ( caller, 0x0 );

    // Set the docking window's widget.
    dockWidget->setWidget ( _animation );

    // Save in the map and release.
    _docked[_animation] = dockWidget.release();
  }
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


///////////////////////////////////////////////////////////////////////////////
//
//  Called when there is an intersection.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::setStringGrid ( const StringGrid &grid )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle bad state.
  if ( 0x0 == _intersectInfo )
  {
    return;
  }

  // Handle empty grid.
  if ( true == grid.empty() )
  {
    _intersectInfo->clear();
    return;
  }

  // Print formatted data.
  std::ostringstream out;
  for ( unsigned int r = 0; r < grid.size(); ++r )
  {
    out << ( ( 0 == r ) ? "" : "\n" );
    const StringGrid::value_type &row ( grid.at(r) );
    for ( unsigned int c = 0; c < row.size(); ++c )
    {
      const std::string &s ( row.at(c) );
      out << ( ( 0 == c ) ? "" : ": " ) << s;
    }
  }

  // Set the label's text.
  const std::string text ( out.str() );
  _intersectInfo->setText ( text.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given document type is handled.
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDelegateComponent::handlesDocumentType ( Usul::Interfaces::IUnknown *document )
{
  USUL_TRACE_SCOPE;
  // Re-entrant.

  Usul::Interfaces::IDocument::QueryPtr doc ( document );
  return ( ( true == doc.valid() ) ? this->doesHandle ( doc->typeName() ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::initNewDocument ( Unknown *document, Unknown *caller )
{
  USUL_TRACE_SCOPE;
 
  // Get the parent.
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );
  QWidget *parent ( mainWindow.valid() ? mainWindow->mainWindow() : 0x0 );

  // Make the dialog.
  Modflow::FileInputDialog dialog ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

  // Get the xml.
  std::string xml ( dialog.buildXml() );

  // The filename.
  std::string filename;
  
  // Write the temp file.
  {
    Usul::File::Temp temp;
    temp.stream() << xml;
    temp.stream().flush();

    // Capture the filename.
    filename = temp.name();

    // Do not delete file.
    temp.release();
  }

  // Make sure file gets deleted.
  Usul::Scope::RemoveFile remove ( filename );

  // Read the file.
  Usul::Interfaces::IRead::QueryPtr read ( document );
  if ( false == read.valid() )
    return;

  // Create a job to read the file.
  Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create 
    ( Usul::Adaptors::bind3 ( filename, caller, static_cast < Usul::Interfaces::IUnknown * > ( 0x0 ), 
      Usul::Adaptors::memberFunction ( read, &Usul::Interfaces::IRead::read ) ) ) );
  if ( false == job.valid() )
    return;

  // Add job to manager.
  Usul::Jobs::Manager::instance().addJob ( job.get() );

  // Spin until the job is done. This keeps the GUI responsive.
  QEventLoop loop;
  while ( false == job->isDone() )
  {
    loop.processEvents();
  }

  // Set document state.
  Usul::Interfaces::IDocument::QueryPtr doc ( document );
  if ( doc.valid() )
  {
    doc->fileValid ( false );
    doc->modified ( true );
  }
}
