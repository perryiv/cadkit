
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

#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

#include "QtGui/QWorkspace"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VaporIntrusionGUIDelegateComponent, VaporIntrusionGUIDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDelegateComponent::VaporIntrusionGUIDelegateComponent() : BaseClass(),
xyzView( 0x0 ),
xyView ( 0x0 ),
xzView ( 0x0 ),
yzView ( 0x0 )
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
    xyzView = new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller );
    parent->addWindow ( xyzView.get() );

    // Add XY window
    xyView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller );
    parent->addWindow ( xyView.get() );

    // Add XZ window
    xzView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller );
    parent->addWindow ( xzView.get() );

    // Add YZ window
    yzView = new VIGUIViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller );
    parent->addWindow ( yzView.get() );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid () )
    {
      xyzView->viewer()->scene ( build->buildScene ( document->options(), caller ) );
      xyView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
      xzView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
      yzView->viewer()->scene  ( build->buildScene ( document->options(), caller ) );
    }

    // Set the titles
    xyzView->setTitle( "XYZ -- 3D View" );

    xyView->setTitle ( "XY -- Front View" );
    xyView->camera( OsgTools::Render::Viewer::FRONT );
    xyView->set( Usul::Math::Vec3ui( 1, 1, 0 ) );
    xyView->id( 1 );

    xzView->setTitle ( "XZ -- Top View" );
    xzView->camera( OsgTools::Render::Viewer::TOP );
    xzView->set( Usul::Math::Vec3ui( 1, 0, 1 ) );
    xzView->id( 2 );

    yzView->setTitle ( "YZ -- Left View" );
    yzView->camera( OsgTools::Render::Viewer::LEFT );
    yzView->set( Usul::Math::Vec3ui( 0, 1, 1 ) );
    yzView->id( 3 );

    // Get the bounds of the parent window
    int w ( parent->width()  * 0.5 );
    int h ( parent->height() * 0.5 );

    // XYZ window is the top left window
    xyzView->resize  ( w, h );

    // XY window is the bottom right window
    xyView->resize  ( w, h );

    // XZ window is the bottom left window
    xzView->resize  ( w, h );

    // XY window is the top right window
    yzView->resize  ( w, h );

    // Show the windows
    xyzView->show();
    xyView->show();
    xzView->show();
    yzView->show();
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


/////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu
//
/////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDelegateComponent::tile( Usul::Interfaces::IUnknown* caller )
{
#if 1
  Usul::Interfaces::Qt::IWorkspace::QueryPtr workspace ( caller );

  if( workspace.valid() )
  {
    QWorkspace *parent ( workspace->workspace() );
    parent->tile();
  }
#else

  typedef CadKit::Helios::Views::OSG::Viewer QtViewer;

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;

  if( 0x0 == caller )
    return;

  Usul::Interfaces::Qt::IWorkspace::QueryPtr workspace ( caller );

  if( workspace.valid() )
  {
    QWorkspace *parent ( workspace->workspace() );


    // Get the bounds of the parent window
    int x ( parent->x() );
    int y ( parent->y() );
    int w ( parent->width() );
    int h ( parent->height() );
    
    int childWidth( w * 0.5 );
    int childHeight ( h * 0.5 );

    // Set the bounds of the child windows
    // XYZ window is the top left window
    //xyzView->setGeometry( 0, 0, childWidth, childHeight );
    xyzView->move( 0, 0 );
    xyzView->resize  ( childWidth, childHeight );
    xyzView->adjustSize();

    // XY window is the bottom right window
    //xyView->setGeometry( childWidth, childHeight, childWidth, childHeight );
    xyView->move( childWidth, childHeight );
    xyView->resize  ( childWidth, childHeight );
    xyView->adjustSize();

    // XZ window is the bottom left window
    //xzView->setGeometry( 0, childHeight, childWidth, childHeight );
    xzView->move( 0, childHeight );
    xzView->resize  ( childWidth, childHeight );
    xzView->adjustSize();

    // XY window is the top right window
    //yzView->setGeometry( childWidth, 0, childWidth, childHeight );
    yzView->move( childWidth, 0 );
    yzView->resize  ( childWidth, childHeight );
    yzView->adjustSize();
  }
#endif
}