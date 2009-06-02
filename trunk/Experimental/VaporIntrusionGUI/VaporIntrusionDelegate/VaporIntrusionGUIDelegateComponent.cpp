
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "VaporIntrusionGUIDelegateComponent.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"
#include "Usul/Properties/Attribute.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Exceptions/Canceled.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

#include "QtGui/QWorkspace"
#include "QtGui/QDockWidget"
#include "QtGui/QHeaderView"
#include "QtGui/QMainWindow"
#include "QtGui/QTreeWidget"

#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VaporIntrusionGUIDelegateComponent, VaporIntrusionGUIDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDelegateComponent::VaporIntrusionGUIDelegateComponent() : BaseClass(),
_xyzView( 0x0 ),
//_xyView ( 0x0 ),
//_xzView ( 0x0 ),
//_yzView ( 0x0 ),
_docks   (),
_materialContainers(),
_caller(),
_categories(),
_gridMaterials()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDelegateComponent::~VaporIntrusionGUIDelegateComponent()
{
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

  if( mainWindow.valid() )
  {
    // get a handle to the main window
    QMainWindow * main  ( mainWindow->mainWindow() );

    // Delete the DockWidgets.
    for( Docks::iterator iter = _docks.begin(); iter != _docks.end(); ++iter )
    {
      // Remove the DockWidget from the MainWindow.
      main->removeDockWidget ( (*iter).second );

      // Delete the dock and clear
      delete (*iter).second;
      (*iter).second = 0x0;
    }

  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VaporIntrusionGUIDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd*>(this);
  case Usul::Interfaces::IPluginInitialize::IID:
    return static_cast < Usul::Interfaces::IPluginInitialize*>(this);
  case Usul::Interfaces::IInitNewDocument::IID:
    return static_cast < Usul::Interfaces::IInitNewDocument*>(this);
  case VaporIntrusionGUI::Interfaces::IVPIDelegate::IID:
    return static_cast < VaporIntrusionGUI::Interfaces::IVPIDelegate*>(this);
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token
//
/////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDelegateComponent::doesHandle( const std::string& token ) const
{
  return ( token == "Vapor Intrusion GUI" );
}



/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  //typedef CadKit::Helios::Views::OSG::Viewer QtViewer;

  // Non-ref'ing smart-pointers that throw if given null.
  //typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  //typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;

  Usul::Interfaces::Qt::IWorkspace::QueryPtr workspace ( caller );

  if( workspace.valid() )
  {
    QWorkspace *parent ( workspace->workspace() );

    // Add XYZ window
    _xyzView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainers );
    parent->addWindow ( _xyzView.get() );

    //// Add XY window
    //_xyView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainer  );
    //parent->addWindow ( _xyView.get() );

    //// Add XZ window
    //_xzView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainer  );
    //parent->addWindow ( _xzView.get() );

    //// Add YZ window
    //_yzView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainer  );
    //parent->addWindow ( _yzView.get() );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid () )
    {
      _xyzView->viewer()->scene ( build->buildScene ( document->options(), caller ) );
      //_xyView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
      //_xzView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
      //_yzView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
    }

    // Set the titles
    _xyzView->setTitle( "XYZ -- 3D View" );

    //_xyView->setTitle ( "XY -- Front View" );
    //_xyView->camera( OsgTools::Render::Viewer::FRONT );
    //_xyView->set( Usul::Math::Vec3ui( 1, 1, 0 ) );
    //_xyView->id( 1 );

    //_xzView->setTitle ( "XZ -- Top View" );
    //_xzView->camera( OsgTools::Render::Viewer::TOP );
    //_xzView->set( Usul::Math::Vec3ui( 1, 0, 1 ) );
    //_xzView->id( 2 );

    //_yzView->setTitle ( "YZ -- Left View" );
    //_yzView->camera( OsgTools::Render::Viewer::LEFT );
    //_yzView->set( Usul::Math::Vec3ui( 0, 1, 1 ) );
    //_yzView->id( 3 );

    // Get the bounds of the parent window
    int w ( parent->width()  * 0.5 );
    int h ( parent->height() * 0.5 );

    //// XYZ window is the top left window
    //_xyzView->resize ( w, h );

    //// XY window is the bottom right window
    //_xyView->resize  ( w, h );

    //// XZ window is the bottom left window
    //_xzView->resize  ( w, h );

    //// XY window is the top right window
    //_yzView->resize  ( w, h );

    // Show the windows
    _xyzView->show();
    //_xyView->show();
    //_xzView->show();
    //_yzView->show();

    // Start with all windows tiled
    parent->tile();
}
}


/////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu
//
/////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller)
{
  // Make the menu.
  MenuKit::Menu::RefPtr variableMenu ( new MenuKit::Menu ( "Parameters" ) );
  
  // Add Window arrange button
  variableMenu->append ( new MenuKit::Button ( Usul::Commands::genericCommand ( "Grid", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editGrid ) ) ) );
 
  // Add Scalar Editor button
  variableMenu->append ( new MenuKit::Button ( Usul::Commands::genericCommand ( "Scalar Editor", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editScalar ) ) ) );

  //variableMenu->append ( new MenuKit::Button ( Usul::Commands::genericCommand ( "Input", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editInputParameters ) ) ) );
  
   // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( true == document.valid() )
  {
    // get the categories
    Categories categories = document->categories();
    
    for( unsigned int i = 0; i < categories.size(); ++i )
    {
      // get the menu name
      std::string menuName ( categories.at( i ).name );

      // add the sub menu to the main menu
      //variableMenu->append ( new MenuKit::Button ( Usul::Commands::genericCommand ( menuName, Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editInputParameters ) ) ) );

      // add the sub menu to the main menu
      variableMenu->append ( new MenuKit::Button ( Usul::Commands::genericCommand ( menuName, Usul::Adaptors::bind1<void> ( menuName, Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editInputParameters ) ) ) ) ); 
    }

  }
  // Add the window menu to the main menu
  menu.append( variableMenu.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tab for the scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::initializePlugin ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  //Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  //if ( mainWindow.valid() )
  //{
  //  QMainWindow * main ( mainWindow->mainWindow() );

  //  // Build the docking window.
  //  QDockWidget* dock = new QDockWidget ( QObject::tr ( "Materials Container" ), main );
  //  dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

  //  // Create the widget
  //  MaterialContainer* materialContainer = new MaterialContainer ( dock );

  //  // Add the dock to the main window.
  //  dock->setWidget( materialContainer );
  //  main->addDockWidget ( Qt::LeftDockWidgetArea, dock );

  //  // Set the object name.
  //  dock->setObjectName ( "MaterialsContainerWidget" );

  //  // Add toggle to the menu.
  //  Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( caller );
  //  if ( dwm.valid () )
  //    dwm->addDockWidgetMenu ( dock );

  //  // add the dock to the map of docks
  //  _docks["Materials Container"] = dock;
  //  _materialContainers["Materials Container"] = materialContainer;
  //}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the document.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::initNewDocument ( Unknown *document, Unknown *caller )
{ 
  // Get the parent.
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );
  QWidget *parent ( mainWindow.valid() ? mainWindow->mainWindow() : 0x0 );

  // Make the dialog.
  NewVaporIntrusion dialog ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();
  
  // Get and store the dimensions
  Usul::Math::Vec3ui d ( dialog.x(), dialog.y(), dialog.z() );

  // get the document
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr doc ( document );

  if( true == doc.valid() )
  {

    // set the dimensions
    doc->dimensions( d );

    // initialize the document
    doc->initialize();
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given document type is handled.
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDelegateComponent::handlesDocumentType ( Usul::Interfaces::IUnknown *document )
{
  // Re-entrant.

  Usul::Interfaces::IDocument::QueryPtr doc ( document );
  return ( ( true == doc.valid() ) ? this->doesHandle ( doc->typeName() ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the grid space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editGrid()
{
  
  // Make the dialog.
  GridEditorDialog gridEditor;

  // Show the dialog.
  if ( QDialog::Accepted != gridEditor.exec() )
    throw Usul::Exceptions::Canceled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the scalar values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editScalar()
{  

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // Make the dialog.
  ScalarEditorDialog dialog( document->dimensions() );

  // get the currently checked materials
  MaterialsMap mmap;// ( _materialContainers["Materials Container"]->getCheckedMaterials() );

  //loop through the map of material containers and grab all the checked scalars
  for( MaterialContainers::iterator iter = _materialContainers.begin(); iter != _materialContainers.end(); ++iter )
  {
    // get the checked items from the current material dialog
    MaterialsMap imap ( (*iter).second->getCheckedMaterials() );

    for( MaterialsMap::iterator iter2 = imap.begin(); iter2 != imap.end(); ++iter2 )
    {
      // get the name
      std::string name ( (*iter2).first );

      // get the value
      std::string value ( (*iter2).second );

      mmap[name] = value;
    }

  }

  dialog.materials( mmap );

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

  // ok button was clicked...handle it
  dialog.onOkClicked();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the input parameters
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editInputParameters( const std::string& menuName )
{
  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // get the categories
  Categories categories( document->categories() );

  // find the proper category that matches menuName
  for( unsigned int i = 0; i < categories.size(); ++i )
  {
    // get the category name
    std::string name( categories.at( i ).name );

    if( name == menuName )
    {
      // Make the dialog.
      InputParameterDialog dialog( categories.at( i ) );

      // Show the dialog.
      if ( QDialog::Accepted != dialog.exec() )
        throw Usul::Exceptions::Canceled();
      else
      {
        dialog.okClicked();
      }


      break;
    }
  }
      
}


///////////////////////////////////////////////////////////////////////////////
//
// Add a dock to the main window
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::addDock( const std::string& name )
{
  Guard guard ( this );

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

  if ( mainWindow.valid() )
  {
    QMainWindow * main ( mainWindow->mainWindow() );

    // Build the docking window.
    QDockWidget* dock = new QDockWidget ( QObject::tr ( name.c_str() ), main );
    dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    // Create the widget
    MaterialContainer* materialContainer = new MaterialContainer ( dock );

    // Add the dock to the main window.
    dock->setWidget( materialContainer );
    main->addDockWidget ( Qt::LeftDockWidgetArea, dock );

    // Set the object name.
    dock->setObjectName ( "MaterialsContainerWidget" );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( _caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( dock );

    // add the dock to the map of docks
    _docks[name] = dock;
    _materialContainers[name] = materialContainer;

    if( _docks.size() > 1 )
    {
      main->tabifyDockWidget( (*_docks.begin()).second, dock );
    }
      
  }
  
}


void VaporIntrusionGUIDelegateComponent::addToDock( const std::string& dockName, const std::string& name, const std::string& value )
{
  // check for validity
  if( 0x0 == _materialContainers[dockName] )
    return;

  // add the material to the dock
  _materialContainers[dockName]->add( name, value );
}