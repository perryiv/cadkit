
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
_xyView ( 0x0 ),
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
  // useful typedefs
  typedef OsgTools::Render::Viewer::Corners Corners;

  Usul::Interfaces::Qt::IWorkspace::QueryPtr workspace ( caller );

  if( workspace.valid() )
  {
    QWorkspace *parent ( workspace->workspace() );

    // Add XY window
    _xyView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainers  );
    parent->addWindow ( _xyView.get() );

     // Add XYZ window
    _xyzView = new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller );
    parent->addWindow ( _xyzView.get() );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid () )
    {
      // create and set the 3D widow build options
      Usul::Documents::Document::Options options3D;
      options3D["Dimension"] = "3D";
      _xyzView->viewer()->scene ( build->buildScene ( options3D, caller ) );

      // create and set the 2D window options
      Usul::Documents::Document::Options options2D;
      options2D["Dimension"] = "2D";
      _xyView->viewer()->scene  ( build->buildScene ( options2D, caller ) );

    }

    // Set the titles
    _xyzView->setTitle( "3D Scene View" );

    _xyView->setTitle ( "2D Grid View" );
    _xyView->viewer()->camera( OsgTools::Render::Viewer::TOP );

    // the color white
    osg::Vec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );

    // Get the viewer.
    OsgTools::Render::Viewer::RefPtr xyzViewer ( _xyzView->viewer() );

    // if valid, set the background color to white
    if( true == xyzViewer.valid() )
    {
      // Top left corner.
      xyzViewer->backgroundCorners ( Corners::ALL );
      xyzViewer->backgroundColor ( white );
    }

    // Get the viewer.
    OsgTools::Render::Viewer::RefPtr xyViewer ( _xyView->viewer() );

    // if valid, set the background color to white
    if( true == xyViewer.valid() )
    {
      // Top left corner.
      xyViewer->backgroundCorners ( Corners::ALL );
      xyViewer->backgroundColor ( white );
    }

       // Get the bounds of the parent window
    int w ( parent->width()  * 0.5 );
    int h ( parent->height() * 0.5 );

    // Show the windows
    _xyzView->show();
    _xyView->show();

    // swap the y and Z axis labels on the 2d viewer
    _xyView->viewer()->axesLabels( "X", "Z", "Y" );

    // swap the y and Z axis labels on the 3d viewer
    _xyzView->viewer()->axesLabels( "X", "Z", "Y" );

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
  MenuKit::Menu::RefPtr fileMenu ( menu.find ( "File", true ) );

  // Add Window Building button
  fileMenu->append ( MenuKit::Button::create ( "Save/Load Preferences", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::saveLoadPrefs ) ) );

  // Make the menu.
  MenuKit::Menu::RefPtr editMenu ( new MenuKit::Menu ( "Edit" ) );
  
  // Add Window Building button
  editMenu->append ( MenuKit::Button::create ( "Building", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editBuilding) ) );
 
  // Add Window Chemicals button
  editMenu->append ( MenuKit::Button::create ( "Chemicals", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editChemicals ) ) );
 
  // add editAddChemicalsToSource button
  editMenu->append ( MenuKit::Button::create ( "Soils", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editSoils ) ) );
 
  // Add Window Sources button
  editMenu->append ( MenuKit::Button::create ( "Sources", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editSources ) ) );

  // add settings menu to the tools menu
  editMenu->append ( MenuKit::Button::create ( "Wind", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editWind ) ) );

  // add settings menu to the tools menu
  editMenu->append ( MenuKit::Button::create ( "Settings", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::editSettings ) ) );

  // Add the window menu to the main menu
  menu.append( editMenu.get() );

  // Make the run menu.
  MenuKit::Menu::RefPtr runMenu ( new MenuKit::Menu ( "Run" ) );

  // add send button
  runMenu->append ( MenuKit::Button::create ( "Send to Server", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::sendFile ) ) );
 
  // Add the window menu to the main menu
  menu.append( runMenu.get() );
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

  // Make the dialog.
  NewVaporIntrusion nd ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != nd.exec() )
    throw Usul::Exceptions::Canceled();

  // Make the dialog.
  GridSpaceDialog dialog ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();
  
  
  Usul::Math::Vec3ui d ( dialog.x(), static_cast< unsigned int > ( dialog.depth() * 100 ), dialog.y() );
  Usul::Math::Vec3f spacing( 1.6f, 0.8f, 1.6f );

  // get the document
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr doc ( document );

  if( true == doc.valid() )
  {    
    // set the initial grid spacing
    doc->setInitialSpacing( spacing );

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
// Edit the Wind values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editWind()
{
  // Make the dialog.
  WindDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // get the min and max
  std::pair< float, float > mm ( document->pressureMinMax() );

  // set the min and max in the editor
  editor.minMax( mm.first, mm.second );

  // populate the chemical list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  // finalize the dialog
  editor.finalize();

  // update the min and max
  mm = editor.minMax();

  // set the document min and max
  document->pressureMinMax( mm.first, mm.second );

  // set the document soils
  document->windDirection( editor.direction() );

  // rebuild the scene
  document->rebuildScene();

}

///////////////////////////////////////////////////////////////////////////////
//
// edit the Soil values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editSoils()
{
  // Make the dialog.
  SoilLayerDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the chemicals
  editor.soils( document->soils() );

  // set the soil library
  editor.library( document->soilLibrary() );

  // populate the chemical list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  // set the document soils
  document->soils( editor.soils() );

  // set the document soil library
  document->soilLibrary( editor.library() );

  // rebuild the scene
  document->rebuildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
// edit the Soil values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editCracks()
{
   // Make the dialog.
  CracksDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the chemicals
  editor.cracks( document->cracks() );

  // populate the chemical list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  document->cracks( editor.cracks() );

  document->rebuildScene();
}
///////////////////////////////////////////////////////////////////////////////
//
// Edit the Chemical values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editChemicals()
{
  // Make the dialog.
  ChemicalDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the chemicals
  editor.chemicals( document->chemicals() );

  // set the chemical library
  editor.library( document->library() );

  // populate the chemical list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  // set the chemicals and library
  document->chemicals( editor.getSelectedChemicals() );
  document->library( editor.library() );

  document->rebuildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the file to the server
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::sendFile()
{
  Guard guard ( this );

  // Make the dialog.
  SendDialog dialog;

  // initialize
  dialog.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add/Remove/Modify points on the grid
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editGridAxisPoints()
{
  Guard guard ( this );

   // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )    return;

  // Make the dialog.
  ModifyGridPointsDlg dialog;

  // set the grid axis points
  dialog.gridAxisPoints( document->gridAxisPoints() );

  // initialize
  dialog.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

  // set the points back in the document
  document->gridAxisPoints( dialog.gridAxisPoints() );

  // tell the document's scene to rebuild to reflect the changes
  document->rebuildScene();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the grid space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editBuilding()
{
  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;
  
  // Make the dialog.
  BuildingDialog editor;

  // set the building information
  editor.building( document->building() );

  // initialize
  editor.initialize( document->useBuilding() );

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  // set the grid symmetry
  document->symmetricalGrid( editor.symmetricGrid() );

  // get the document's building
  Building bDoc ( document->building() );

  // get the editor's building
  Building bDlg ( editor.building() );

  // set the appropriate parameters from the dialog
  bDoc.thickness = bDlg.thickness;
  bDoc.xrate = bDlg.xrate;
  bDoc.v = bDlg.v;
  bDoc.depth = bDlg.depth;

  // reset the building information
  document->building( bDoc );

  // set the use building state
  document->useBuilding( editor.useBuilding() );

  // redraw
  document->rebuildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
// edit the Chemical values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editAddChemicalsToSource()
{
  //// Query the active document for IVaporIntrusionGUI
  //VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  //// Check for a valid document
  //if( false == document.valid() )
  //  return;

  //// Make the AddChemicalsToSource dialog
  //AddContaminantsToSourceDialog dialog;

  //// set the sources and chemicals in the dialog
  //dialog.sources( document->sources() );
  //dialog.chemicals( document->chemicals() );

  //// initialize the AddChemicalsToSource dialog
  //dialog.initialize();

  //// Show the dialog.
  //if ( QDialog::Accepted != dialog.exec() )
  //  throw Usul::Exceptions::Canceled();

  //// set the sources and chemicals in the document
  //document->sources( dialog.sources() );
  //document->chemicals( dialog.chemicals() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the grid space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editSettings()
{
  
  // Make the dialog.
  SettingsDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the colors in the dialog
  editor.colors( document->colorInformation() );

  // set the transparencies
  editor.transparencies( document->transparencies() );

  // populate the chemical list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  // finalize the dialog
  editor.finalize();

  // set the document color information
  document->colorInformation( editor.colors() );

  // set the document transparency information
  document->transparencies( editor.transparencies() );

  // rebuild the scene
  document->rebuildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the grid space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::editSources()
{
  
  // Make the dialog.
  SourceConcentrationDialog editor;

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // set the sources
  editor.sources( document->sources() );

  // populate the chemical list
  editor.initialize();

  // Show the dialog.
  if ( QDialog::Accepted != editor.exec() )
    throw Usul::Exceptions::Canceled();

  // set the sources back in the document
  document->sources( editor.sources() );

  // rebuild the scene
  document->rebuildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save or load user preferences
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::saveLoadPrefs()
{
  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // Make the dialog.
  PrefSaveLoadDialog dialog;

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
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
  PrefSaveLoadDialog dialog;

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

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