
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////


#include "PhaseFieldDelegateComponent.h"

#include "Usul/Documents/Document.h"

#include "Helios/Fox/Views/Canvas.h"

#include "FoxTools/Windows/MdiChildWindow.h"
#include "FoxTools/Functions/GLVisual.h"
#include "FoxTools/Headers/MDIButton.h"

#include "Usul/Interfaces/Fox/IFoxClientArea.h"
#include "Usul/Interfaces/Fox/IFoxMDIMenu.h"
#include "Usul/Interfaces/IBuildScene.h"

#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PhaseFieldDelegateComponent , PhaseFieldDelegateComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDelegateComponent::PhaseFieldDelegateComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDelegateComponent::~PhaseFieldDelegateComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PhaseFieldDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string PhaseFieldDelegateComponent::getPluginName() const 
{
  return "Phase Field Delegate";
}

	
/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token
//
/////////////////////////////////////////////////////////////////////////////

bool PhaseFieldDelegateComponent::doesHandle( const std::string& token ) const
{
  return ( token ==    "Phase Field Document" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void PhaseFieldDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  // Typedef
  typedef FoxTools::Windows::MdiChildWindow ChildWindow;

  //Get needed interfaces
  Usul::Interfaces::IFoxClientArea::ValidQueryPtr client ( caller );
  Usul::Interfaces::IFoxMDIMenu::ValidQueryPtr    menu   ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr      doc    ( document->queryInterface( Usul::Interfaces::IDocument::IID ) );

  FX::FXMDIClient *clientArea ( client->clientArea() );
  FX::FXMDIMenu   *mdiMenu    ( menu->mdiMenu() );

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < ChildWindow, Policy > LocalChildPtr;

  // Make the child
  LocalChildPtr child ( new ChildWindow ( document, clientArea, "Untitled", 0x0, mdiMenu, 0 ) ); 

  // Make the view
  Helios::Views::Canvas* canvas ( new Helios::Views::Canvas ( doc, child.get(), FoxTools::Functions::visual() ) );

  // Get pointer to viewer
  Usul::Interfaces::IViewer::ValidQueryPtr viewer ( canvas->viewer() );

  // Set the view
  child->view ( viewer );

  // Create the child window.
  // Call create after the view has been set in the child window and the document
  child->create();

  // Build scene.
  child->onBuildScene ( 0x0, 0, 0x0 );

  // Make sure the new scene has the proper display-list settings.
  canvas->viewer()->setDisplayLists();

  // If we should...
  if ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::VIEW_ALL_ON_LOAD ) )
  {
    // Add a chore to fit the camera to the scene.
    canvas->addChoreCameraFit();
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Refresh the view
//
/////////////////////////////////////////////////////////////////////////////

void PhaseFieldDelegateComponent::refreshView ( Usul::Documents::Document *document, Usul::Interfaces::IViewer *viewer )
{
  Usul::Interfaces::IHeliosView::QueryPtr HeliosView ( viewer );

  if ( HeliosView.valid() )
  {
    OsgTools::Render::Viewer* canvas ( HeliosView->HeliosView() );

    Usul::Interfaces::IBuildScene::QueryPtr build ( document );

    if ( build.valid () )
      canvas->scene ( build->buildScene ( document->options(), viewer ) );
  }
}

