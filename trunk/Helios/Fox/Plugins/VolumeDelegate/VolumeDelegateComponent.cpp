
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

#include "VolumeDelegateComponent.h"
#include "SettingsDialog.h"

#include "Helios/Fox/Views/Canvas.h"
#include "Helios/Fox/Views/TFView.h"

#include "FoxTools/Windows/MdiChildWindow.h"
#include "FoxTools/Functions/GLVisual.h"
#include "FoxTools/Headers/MDIButton.h"

#include "Usul/Interfaces/IFoxClientArea.h"
#include "Usul/Interfaces/IFoxMDIMenu.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IOpenGLState.h"

#include "Usul/Documents/Document.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VolumeDelegateComponent , VolumeDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDelegateComponent::VolumeDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDelegateComponent::~VolumeDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VolumeDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  case Usul::Interfaces::IGetOptions::IID:
    return static_cast < Usul::Interfaces::IGetOptions* > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token
//
/////////////////////////////////////////////////////////////////////////////

bool VolumeDelegateComponent::doesHandle( const std::string& token ) const
{
  return "Volume Document" == token;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void VolumeDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
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

  // Get the options
  Usul::Documents::Document::Options options ( document->options() );

  // Make the view
  OsgFox::Views::Canvas* canvas ( new OsgFox::Views::Canvas ( doc, child.get(), FoxTools::Functions::visual() ) );

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

  Usul::Interfaces::IOpenGLState::ValidQueryPtr state ( canvas->viewer() );

  // Turn off lighting.
  state->lighting( false );
  
  {
    // Make the child
    LocalChildPtr child ( new ChildWindow ( document, clientArea, "Untitled", 0x0, (FX::FXMenuPane * ) mdiMenu, 0 ) ); 

    // Make the view
    OsgFox::Views::TFView* view ( new OsgFox::Views::TFView ( doc, child.get(), FoxTools::Functions::visual() ) );

    // Get pointer to viewer
    Usul::Interfaces::IViewer::ValidQueryPtr viewer ( view );

    // Set the view
    child->view( viewer );

    // Our view is just a listener.
    document->addView ( view );

    // Build scene.
    view->build();

    // Create the child window.
    // Call create after the view has been set in the child window and the document
    child->create();

  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Refresh the view
//
/////////////////////////////////////////////////////////////////////////////

void VolumeDelegateComponent::refreshView  ( Usul::Documents::Document *document, Usul::Interfaces::IViewer *viewer )
{
  Usul::Interfaces::IOsgFoxView::QueryPtr osgFoxView ( viewer );

  if( osgFoxView.valid() )
  {
    OsgTools::Render::Viewer* canvas ( osgFoxView->osgFoxView() );

    Usul::Interfaces::IBuildScene::QueryPtr buildScene ( document );

    if( buildScene.valid() )
      canvas->scene( buildScene->buildScene( document->options() ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the options
//
/////////////////////////////////////////////////////////////////////////////

void VolumeDelegateComponent::getOptions ( Options& options )
{
  //The dialog box
  SettingsDialog dialog;
  dialog.create();

  if ( dialog.execute( FX::PLACEMENT_OWNER ) )
  {
    // Get the options
    dialog.options ( options );
  }
  else
  {
    throw Usul::Exceptions::Canceled ( "User canceled new Volume Document" );
  }
}

