
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
#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Icons/Factory.h"

#include "Usul/Interfaces/Fox/IFoxClientArea.h"
#include "Usul/Interfaces/Fox/IFoxMDIMenu.h"
#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/Fox/IFoxTabBook.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/IAssemblyManager.h"

#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Documents/Manager.h"

// for now
#include "Usul/Resources/MenuBar.h"
#include "Usul/Interfaces/Fox/IMenuBar.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PhaseFieldDelegateComponent , PhaseFieldDelegateComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( PhaseFieldDelegateComponent ) MessageMap[] = 
{
  //          Message Type     ID                                                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, PhaseFieldDelegateComponent::ID_FORWARD,             PhaseFieldDelegateComponent::onCommandForward       ),
  FXMAPFUNC ( FX::SEL_COMMAND, PhaseFieldDelegateComponent::ID_BACKWARD,            PhaseFieldDelegateComponent::onCommandBackward      ),
};

FOX_TOOLS_IMPLEMENT ( PhaseFieldDelegateComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDelegateComponent::PhaseFieldDelegateComponent() :
_frame ( 0x0 )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDelegateComponent::~PhaseFieldDelegateComponent() 
{
  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( Usul::Resources::menuBar() );

  if( tabItem.valid( ) )
    tabItem->deleteTab( "PF" );

  if( _frame )
    delete _frame;
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
  case Usul::Interfaces::IHandleActivatingDocument::IID:
    return static_cast < Usul::Interfaces::IHandleActivatingDocument * > ( this );
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

  // Build scene.
  child->onBuildScene ( 0x0, 0, 0x0 );

  // Create the child window.
  // Call create after the view has been set in the child window and the document
  child->create();

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


/////////////////////////////////////////////////////////////////////////////
//
//  Given document is no longer active.
//
/////////////////////////////////////////////////////////////////////////////

void PhaseFieldDelegateComponent::noLongerActive ( Usul::Documents::Document* document, const std::string& activeType )
{
  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( Usul::Resources::menuBar() );

  if( tabItem.valid( ) )
    tabItem->deleteTab( "PF" );

  if( _frame )
    delete _frame;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Given document is now active.
//
/////////////////////////////////////////////////////////////////////////////

void PhaseFieldDelegateComponent::nowActive ( Usul::Documents::Document* document, const std::string& oldType )
{
  Usul::Interfaces::IDocument::ValidQueryPtr doc ( document );

    //Build the tabs
  this->_buildTab  ( Usul::Resources::menuBar(), doc.get() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Goto next timestep
//
/////////////////////////////////////////////////////////////////////////////

long PhaseFieldDelegateComponent::onCommandForward ( FX::FXObject *, FX::FXSelector, void * )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );

  Usul::Interfaces::ITimeVaryingData::QueryPtr data ( document );

  if ( data.valid () )
  {
    unsigned int current ( data->getCurrentTimeStep() );

    unsigned int num ( data->getNumberOfTimeSteps() );

    if ( ++current != num )
    {
      data->setCurrentTimeStep( current );

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );

      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
  }

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Goto previouse timestep
//
/////////////////////////////////////////////////////////////////////////////

long PhaseFieldDelegateComponent::onCommandBackward ( FX::FXObject *, FX::FXSelector, void * )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );

  Usul::Interfaces::ITimeVaryingData::QueryPtr data ( document );

  if ( data.valid () )
  {
    unsigned int current ( data->getCurrentTimeStep() );

    if ( current != 0 )
    {
      data->setCurrentTimeStep( --current );

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );

      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
  }

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the tab.
//
/////////////////////////////////////////////////////////////////////////////

void PhaseFieldDelegateComponent::_buildTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document )
{
  // Get needed interfaces.
  Usul::Interfaces::IFoxTabItem::QueryPtr         tabItem     ( caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr         foxTabBook  ( caller );

  // Check to see if these interfaces are valid.
  if( tabItem.valid() && foxTabBook.valid() )
  {
    //If we already have a frame, delete it.
    if( _frame )
    {
      tabItem->deleteTab( "PF" );
      delete _frame;
      _frame = 0x0;
    }

    // Add a tab for groups.
    tabItem->addTab ( "PF" );
    FX::FXTabItem *tab ( tabItem->tabItem( "PF" ) );

    tab->create();

    // Get the tab book.
    FX::FXTabBook *tabBook ( foxTabBook->tabBook() );

    _frame = new FX::FXVerticalFrame ( tabBook, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );


    FX::FXHorizontalFrame *hframe ( new FX::FXHorizontalFrame ( _frame, FX::LAYOUT_FILL_X ) );

    typedef FoxTools::Icons::Factory Factory;

    new FX::FXButton ( hframe, "", Factory::instance()->icon ( Factory::ICON_BACKWARD_ARROW ), this, ID_BACKWARD );
    new FX::FXButton ( hframe, "", Factory::instance()->icon ( Factory::ICON_FORWARD_ARROW ), this, ID_FORWARD );

    std::vector < std::string > names;

    Usul::Interfaces::IAssemblyManager::QueryPtr assemblyManager ( document );

    if ( assemblyManager.valid() )
    {
      assemblyManager->getAssemblyNames( names );

      for ( unsigned int i = 0; i < names.size(); ++i )
      {
        new FX::FXCheckButton ( _frame, names.at( i ).c_str() );
      }
    }

    _frame->create();
  }

}
