
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
_dockMap   (),
_dockList   (),
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
    for( DockList::iterator iter = _dockList.begin(); iter != _dockList.end(); ++iter )
    {
      // Remove the DockWidget from the MainWindow.
      main->removeDockWidget ( (*iter) );

      // Delete the dock and clear
      delete (*iter);
      (*iter) = 0x0;
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
  MenuKit::Menu::RefPtr variableMenu ( new MenuKit::Menu ( "Edit" ) );
  
  // Add Window Grid button
  variableMenu->append ( MenuKit::Button::create ( "Grid", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editGrid ) ) );
 
  // Add Window Building button
  variableMenu->append ( MenuKit::Button::create ( "Building", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editBuilding) ) );
 
  // Add Window Sources button
  variableMenu->append ( MenuKit::Button::create ( "Sources", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editSources ) ) );
  
  // Add Window Contaminants button
  variableMenu->append ( MenuKit::Button::create ( "Contaminants", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editContaminants ) ) );
 
  // add editAddContaminantsToSource button
  variableMenu->append ( MenuKit::Button::create ( "Source Contaminats", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editAddContaminantsToSource ) ) );
 
  // Add Scalar Editor button
  // variableMenu->append ( MenuKit::Button::create ( "Scalar", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editScalar ) ) );

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
      variableMenu->append ( MenuKit::Button::create ( menuName, Usul::Adaptors::bind1<void> ( menuName, Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editInputParameters ) ) ) ); 
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

  // Make the building dialog
  BuildingDialog bd ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != bd.exec() )
    throw Usul::Exceptions::Canceled();

  // Make the contaminant dialog
  ContaminantDialog cd ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != cd.exec() )
    throw Usul::Exceptions::Canceled();

  // Make the Sources dialog
  SourceDialog sd ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != sd.exec() )
    throw Usul::Exceptions::Canceled();

  // Make the AddContaminantsToSource dialog
  AddContaminantsToSourceDialog actsd ( parent );

  // set the sources and contaminants
  actsd.sources( sd.sources() );
  actsd.contaminants( cd.contaminants() );

  // initialize the AddContaminantsToSource dialog
  actsd.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != actsd.exec() )
    throw Usul::Exceptions::Canceled();

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

    // get the user selected building parameters.
    if( true == bd.useBuilding() )
    {
      Building b ( bd.building() );
      doc->building( b.l, b.w, b.h, b.x, b.y, b.z );
      doc->useBuilding( true );
    }
    else
    {
      doc->useBuilding( false );
    }

    // set the source information in the document
    doc->sources( actsd.sources() );

    // set the contaminant information in the document
    doc->contaminants( actsd.contaminants() );

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
// Edit the Contaminant values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editContaminants()
{
  // Make the dialog.
  ContaminantDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the contaminants
  editor.contaminants( document->contaminants() );

  // populate the contaminant list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  document->contaminants( editor.contaminants() );

  document->rebuildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the grid space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editBuilding()
{
  
  // Make the dialog.
  BuildingDialog editor;

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();
}


///////////////////////////////////////////////////////////////////////////////
//
// edit the Contaminant values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editAddContaminantsToSource()
{
  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // Make the AddContaminantsToSource dialog
  AddContaminantsToSourceDialog dialog;

  // set the sources and contaminants in the dialog
  dialog.sources( document->sources() );
  dialog.contaminants( document->contaminants() );

  // initialize the AddContaminantsToSource dialog
  dialog.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

  // set the sources and contaminants in the document
  document->sources( dialog.sources() );
  document->contaminants( dialog.contaminants() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the grid space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editSources()
{
  
  // Make the dialog.
  SourceDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the contaminants
  editor.sources( document->sources() );

  // populate the contaminant list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  document->sources( editor.sources() );

  document->rebuildScene();
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
    dock->setObjectName ( name.c_str() );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( _caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( dock );

    // add the dock to the map of docks
    _dockList.push_back( dock );
    _dockMap[ name ] = _dockList.size() - 1;
    _materialContainers[name] = materialContainer;

    if( _dockList.size() > 1 )
    {
      for( unsigned int i = 0; i < _dockList.size() - 1; ++i )
      {
        if( ( i + 1 ) < _dockList.size() )
        {
          main->tabifyDockWidget( _dockList.at( i ), _dockList.at( i + 1 ) );
        }
      }
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