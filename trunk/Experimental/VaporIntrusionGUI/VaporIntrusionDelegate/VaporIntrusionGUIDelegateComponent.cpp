
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

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VaporIntrusionGUIDelegateComponent, VaporIntrusionGUIDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDelegateComponent::VaporIntrusionGUIDelegateComponent() : BaseClass(),
_xyzView( 0x0 ),
_xyView ( 0x0 ),
_xzView ( 0x0 ),
_yzView ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDelegateComponent::~VaporIntrusionGUIDelegateComponent()
{
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
    _xyzView = new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller );
    parent->addWindow ( _xyzView.get() );

    // Add XY window
    _xyView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainer  );
    parent->addWindow ( _xyView.get() );

    // Add XZ window
    _xzView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainer  );
    parent->addWindow ( _xzView.get() );

    // Add YZ window
    _yzView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller, _materialContainer  );
    parent->addWindow ( _yzView.get() );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid () )
    {
      _xyzView->viewer()->scene ( build->buildScene ( document->options(), caller ) );
      _xyView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
      _xzView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
      _yzView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
    }

    // Set the titles
    _xyzView->setTitle( "XYZ -- 3D View" );

    _xyView->setTitle ( "XY -- Front View" );
    _xyView->camera( OsgTools::Render::Viewer::FRONT );
    _xyView->set( Usul::Math::Vec3ui( 1, 1, 0 ) );
    _xyView->id( 1 );

    _xzView->setTitle ( "XZ -- Top View" );
    _xzView->camera( OsgTools::Render::Viewer::TOP );
    _xzView->set( Usul::Math::Vec3ui( 1, 0, 1 ) );
    _xzView->id( 2 );

    _yzView->setTitle ( "YZ -- Left View" );
    _yzView->camera( OsgTools::Render::Viewer::LEFT );
    _yzView->set( Usul::Math::Vec3ui( 0, 1, 1 ) );
    _yzView->id( 3 );

    // Get the bounds of the parent window
    int w ( parent->width()  * 0.5 );
    int h ( parent->height() * 0.5 );

    // XYZ window is the top left window
    _xyzView->resize ( w, h );

    // XY window is the bottom right window
    _xyView->resize  ( w, h );

    // XZ window is the bottom left window
    _xzView->resize  ( w, h );

    // XY window is the top right window
    _yzView->resize  ( w, h );

    // Show the windows
    _xyzView->show();
    _xyView->show();
    _xzView->show();
    _yzView->show();
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
  //MenuKit::Menu::RefPtr windowMenu ( new MenuKit::Menu ( "Window" ) );
  
  // Add Window arrange button
  //windowMenu->append ( new MenuKit::Button ( Usul::Commands::genericCommand ( "Arrange", Usul::Adaptors::bind1<void> ( caller,  Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDelegateComponent::tile )  ), Usul::Commands::TrueFunctor() ) ) );
  
  // Add the window menu to the main menu
  //menu.append( windowMenu.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tab for the scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::initializePlugin ( Usul::Interfaces::IUnknown *caller )
{
  //_caller = caller;

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  if ( mainWindow.valid() )
  {
    QMainWindow * main ( mainWindow->mainWindow() );

    // Build the docking window.
    _dock = new QDockWidget ( QObject::tr ( "Materials Container" ), main );
    _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    // Create the widget
    _materialContainer = new MaterialContainer ( _dock );

    // Add the dock to the main window.
    _dock->setWidget( _materialContainer );
    main->addDockWidget ( Qt::LeftDockWidgetArea, _dock );

    // Set the object name.
    _dock->setObjectName ( "MaterialsContainerWidget" );

    // Add toggle to the menu.
    Usul::Interfaces::IQtDockWidgetMenu::QueryPtr dwm ( caller );
    if ( dwm.valid () )
      dwm->addDockWidgetMenu ( _dock );
  }
}