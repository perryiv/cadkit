
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleDelegateComponent.h"

#include "Usul/Documents/Document.h"

#include "Helios/Fox/Views/Canvas.h"

#include "OsgTools/Triangles/Loop.h"

#include "FoxTools/Windows/MdiChildWindow.h"
#include "FoxTools/Functions/GLVisual.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/ToolBar/ToggleButton.h"
#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/ToolBar/Factory.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Functions/Check.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/List.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/CheckButton.h"
#include "FoxTools/Headers/MDIButton.h"
#include "FoxTools/Headers/ScrollWindow.h"
#include "FoxTools/Dialogs/Message.h"
#include "FoxTools/Functions/MainWindow.h"

#include "Usul/Interfaces/IFoxClientArea.h"
#include "Usul/Interfaces/IFoxMDIMenu.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ICancelButton.h"
#include "Usul/Interfaces/IFlushEvents.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IFoxTabItem.h"
#include "Usul/Interfaces/IFoxTabBook.h"
#include "Usul/Interfaces/IAnimate.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IAddTriangle.h"
#include "Usul/Interfaces/IFindLoops.h"
#include "Usul/Interfaces/IGetLoops.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IClippingPlanes.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IGroupPrimitives.h"
#include "Usul/Interfaces/IPrimitiveGroup.h"

#include "Usul/Registry/Constants.h"
#include "Usul/Properties/Attribute.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Components/Manager.h"
#include "Usul/Loops/FindLoops.h"
#include "Usul/State/Busy.h"
#include "Usul/Types/Types.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Resources/StatusBar.h"

#include "OsgTools/State/StateSet.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"
#include "osg/Group"
#include "osg/Vec3f"
#include "osg/BoundingBox"
#include "osgText/Text"


#include <vector>
#include <sstream>
#include <algorithm>


// for now
#include "Usul/Resources/MenuBar.h"
#include "Usul/Interfaces/IMenuBar.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( TriangleDelegateComponent ) MessageMap[] = 
{
  //          Message Type     ID                                                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_FACET,                 TriangleDelegateComponent::onCommandFacet               ),
  FXMAPFUNC ( FX::SEL_UPDATE,  TriangleDelegateComponent::ID_FACET,                 TriangleDelegateComponent::onUpdateFacet                ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_VERTEX,                TriangleDelegateComponent::onCommandVertex              ),
  FXMAPFUNC ( FX::SEL_UPDATE,  TriangleDelegateComponent::ID_VERTEX,                TriangleDelegateComponent::onUpdateVertex               ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_BOUNDING_BOX,          TriangleDelegateComponent::onCommandBoundingBox         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  TriangleDelegateComponent::ID_BOUNDING_BOX,          TriangleDelegateComponent::onUpdateBoundingBox          ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_GLASS_BOUNDING_BOX,    TriangleDelegateComponent::onCommandGlassBoundingBox    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  TriangleDelegateComponent::ID_GLASS_BOUNDING_BOX,    TriangleDelegateComponent::onUpdateGlassBoundingBox     ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_GOTO_LOOP,             TriangleDelegateComponent::onCommandGotoLoop           ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_GOTO_SUCCEEDED_LOOP,   TriangleDelegateComponent::onCommandGotoSucceededLoop  ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_TRIANGULATE,           TriangleDelegateComponent::onCommandTriangulate        ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_TRIANGULATE_ALL,       TriangleDelegateComponent::onCommandTriangulateAll     ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_LOOP_DONE,             TriangleDelegateComponent::onCommandLoopDone           ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_SHOW_ALL,              TriangleDelegateComponent::onCommandShowAll            ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_HIDE_ALL,              TriangleDelegateComponent::onCommandHideAll            ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_CLIP_BOX,              TriangleDelegateComponent::onCommandClipBox            ),
  FXMAPFUNC ( FX::SEL_UPDATE,  TriangleDelegateComponent::ID_CLIP_BOX,              TriangleDelegateComponent::onUpdateClipBox             ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_TOGGLE_GROUP,          TriangleDelegateComponent::onCommandToggleGroup        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  TriangleDelegateComponent::ID_TOGGLE_GROUP,          TriangleDelegateComponent::onUpdateToggleGroup         ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_HIDE_GROUPS,           TriangleDelegateComponent::onCommandHideGroups         ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_SHOW_GROUPS,           TriangleDelegateComponent::onCommandShowGroups         ),
  FXMAPFUNC ( FX::SEL_COMMAND, TriangleDelegateComponent::ID_GROUP_TRANSPARENCY,    TriangleDelegateComponent::onCommandGroupTransparency  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  TriangleDelegateComponent::ID_GROUP_TRANSPARENCY,    TriangleDelegateComponent::onUpdateGroupTransparency   ),
};

FOX_TOOLS_IMPLEMENT ( TriangleDelegateComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Properties::Attribute < Usul::Documents::Document* > DocAttribute;
typedef FoxTools::ToolBar::Bar ToolBar;
typedef FoxTools::ToolBar::ToggleButton ToggleButton;
typedef FoxTools::ToolBar::Factory Factory;
typedef FoxTools::Icons::Factory Icons;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangleDelegateComponent , TriangleDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleDelegateComponent::TriangleDelegateComponent() : BaseClass(),
_uncappedFrame( 0x0 ),
_succeededFrame ( 0x0 ),
_uncappedList ( 0x0 ),
_succeededList ( 0x0 ),
_useClipBox ( false ),
_activeLoops(),
_groupFrame ( 0x0 ),
_groupList ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleDelegateComponent::~TriangleDelegateComponent()
{
  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( Usul::Resources::menuBar() );

  if( tabItem.valid( ) )
    tabItem->deleteTab( "Uncapped" );

  if( tabItem.valid( ) )
    tabItem->deleteTab( "Succeeded" );

  if( tabItem.valid( ) )
    tabItem->deleteTab( "Groups" );

  if( _uncappedFrame )
    delete _uncappedFrame;

  if( _succeededFrame )
    delete _succeededFrame;

  if( _groupFrame )
    delete _groupFrame;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TriangleDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  case Usul::Interfaces::IHandleActivatingDocument::IID:
    return static_cast < Usul::Interfaces::IHandleActivatingDocument* > ( this );
  case Usul::Interfaces::IUpdateGUI::IID:
    return static_cast < Usul::Interfaces::IUpdateGUI* > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token.
//
/////////////////////////////////////////////////////////////////////////////

bool TriangleDelegateComponent::doesHandle( const std::string& token ) const
{
  return ( "Triangle Document" == token || token ==  "Paradis Document" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI.
//
/////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  // Typedef
  typedef FoxTools::Windows::MdiChildWindow ChildWindow;

  // Get needed interfaces
  Usul::Interfaces::IFoxClientArea::ValidQueryPtr client ( caller );
  Usul::Interfaces::IFoxMDIMenu::ValidQueryPtr    menu   ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr      doc    ( document );

  FX::FXMDIClient *clientArea ( client->clientArea() );
  FX::FXMDIMenu   *mdiMenu    ( menu->mdiMenu() );

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < ChildWindow, Policy > LocalChildPtr;

  // Make the child
  LocalChildPtr child ( new ChildWindow ( document, clientArea, "Untitled", 0x0, mdiMenu, 0 ) ); 

  // Make the view
  OsgFox::Views::Canvas* canvas ( new OsgFox::Views::Canvas ( doc, child.get(), FoxTools::Functions::visual() ) );

  // Get pointer to viewer
  Usul::Interfaces::IViewer::ValidQueryPtr viewer ( canvas->viewer() );

  // Set the view
  child->view ( viewer );

  // Create the child window.
  // Call create after the view has been set in the child window and the document.
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
//  Refresh the view.
//
/////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::refreshView ( Usul::Documents::Document *document, Usul::Interfaces::IViewer *viewer )
{
  Usul::Interfaces::IOsgFoxView::QueryPtr osgFoxView ( viewer );

  if ( osgFoxView.valid() )
  {
    OsgTools::Render::Viewer* canvas ( osgFoxView->osgFoxView() );

    Usul::Interfaces::IBuildScene::QueryPtr build ( document );

    if ( build.valid () )
      canvas->scene ( build->buildScene ( document->options( viewer ) ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Given document is no longer active.
//
/////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::noLongerActive ( Usul::Documents::Document* document, const std::string& activeType )
{
  // Remove the triangle toolbar.  The tool bar buttons contain references to the document.
  // This is needed so that the document get properly deleted and the update functions don't get called again.
  Factory::instance().remove( "Triangle" );
  Factory::instance().layout();

  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( Usul::Resources::menuBar() );


  //If we already have a frame, delete it
  if( _uncappedFrame )
  {
    tabItem->deleteTab( "Uncapped" );
    delete _uncappedFrame;
    _uncappedFrame = 0x0;
  }

  if( _succeededFrame )
  {
    tabItem->deleteTab( "Succeeded" );
    delete _succeededFrame;
    _succeededFrame = 0x0;
  }

  if( _groupFrame )
  {
    tabItem->deleteTab( "Groups" );
    delete _groupFrame;
    _groupFrame = 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Given document is now active.
//
/////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::nowActive ( Usul::Documents::Document* document, const std::string& oldType )
{
  //Make the triangle toolbar
  ToolBar::ValidRefPtr triangleToolBar ( Factory::instance().create( "Triangle", Factory::TOP ) );

  triangleToolBar->clear();

  ToggleButton::ValidRefPtr b1 ( new ToggleButton ( "Facet",  "Turn on per facet shading.",   Icons::ICON_FLAT,    this, ID_FACET ) );
  b1->userData ( new DocAttribute ( document ) );

  ToggleButton::ValidRefPtr b2 ( new ToggleButton ( "Vertex", "Turn on per vertex shading.",  Icons::ICON_SMOOTH, this, ID_VERTEX  ) );
  b2->userData ( new DocAttribute ( document ) );

  ToggleButton::ValidRefPtr b3 ( new ToggleButton ( "Bounding Box",  "Show the bounding box.",   Icons::ICON_BOUND_BOX,    this, ID_BOUNDING_BOX ) );
  b3->userData ( new DocAttribute ( document ) );

  ToggleButton::ValidRefPtr b4 ( new ToggleButton ( "Glass Bounding Box", "Show the glass bounding box.",  Icons::ICON_GLASS_BOX, this, ID_GLASS_BOUNDING_BOX  ) );
  b4->userData ( new DocAttribute ( document ) );
  
  // Append the buttons
  triangleToolBar->append ( b1.get() );
  triangleToolBar->append ( b2.get() );
  triangleToolBar->append ( b3.get() );
  triangleToolBar->append ( b4.get() );

  triangleToolBar->build();
  triangleToolBar->create();

  Factory::instance().layout();

  Usul::Interfaces::IDocument::ValidQueryPtr doc ( document );

    //Build the tabs
  this->_buildUncappedTab  ( Usul::Resources::menuBar(), doc.get() );
  this->_buildSucceededTab ( Usul::Resources::menuBar(), doc.get() );
  this->_buildGroupTab     ( Usul::Resources::menuBar(), doc.get() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Change normals to per facet.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandFacet ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;

  Document::RefPtr document ( attribute->value() );

  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return 0;

  Document::Options &options ( document->options() );

  options["normals"] = "per-facet";
  options["colors"]  = "per-facet";

  document->sendMessage( Usul::Documents::Document::ID_BUILD_SCENE );
  document->sendMessage( Usul::Documents::Document::ID_RENDER_SCENE );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Change normals to per vertex.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandVertex ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;

  Document::RefPtr document ( attribute->value() );

  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return 0;

  Document::Options &options ( document->options() );

  options["normals"] = "per-vertex";
  options["colors"]  = "per-vertex";

  document->sendMessage( Usul::Documents::Document::ID_BUILD_SCENE );
  document->sendMessage( Usul::Documents::Document::ID_RENDER_SCENE );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update normals per facet button.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onUpdateFacet ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;
  Document::RefPtr document ( attribute->value() );

  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return 0;

  Document::Options options ( document->options() );

  const bool average ( "per-vertex" == options["normals"] );
  FoxTools::Functions::check ( !average, object );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update normals per vertex button.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onUpdateVertex ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;
  Document::RefPtr document ( attribute->value() );

  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return 0;

  Document::Options options ( document->options() );

  const bool average ( "per-vertex" == options["normals"] );
  FoxTools::Functions::check( average, object );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Toggle bounding box.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandBoundingBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;

  Document::RefPtr document ( attribute->value() );

  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return 0;

  Document::Options &options ( document->options() );

  const bool show ( "Show" == options["BoundingBox"] );

  if( show )
    options["BoundingBox"] = "Hide";
  else
    options["BoundingBox"] = "Show";

  document->sendMessage( Usul::Documents::Document::ID_BUILD_SCENE );
  document->sendMessage( Usul::Documents::Document::ID_RENDER_SCENE );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Toggle glass bounding box.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandGlassBoundingBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;

  Document::RefPtr document ( attribute->value() );

  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return 0;

  Document::Options &options ( document->options() );

  const bool show ( "Show" == options["GlassBoundingBox"] );

  if( show )
    options["GlassBoundingBox"] = "Hide";
  else
    options["GlassBoundingBox"] = "Show";

  document->sendMessage( Usul::Documents::Document::ID_BUILD_SCENE );
  document->sendMessage( Usul::Documents::Document::ID_RENDER_SCENE );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update bounding box button.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onUpdateBoundingBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;

  Document::ValidRefPtr document ( attribute->value() );

  Document::Options &options ( document->options() );

  const bool show ( "Show" == options["BoundingBox"] );

  FoxTools::Functions::check( show, object );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update glass bounding box button.
//
/////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onUpdateGlassBoundingBox  ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the button.
  FX::FXToggleButton *b = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  if ( 0x0 == b )
    return 0;

  // Get the command.
  ToggleButton::RefPtr button ( reinterpret_cast < ToggleButton *  > ( b->getUserData() ) );
  DocAttribute::RefPtr attribute ( reinterpret_cast < DocAttribute* > ( button->userData() ) );

  typedef Usul::Documents::Document Document;

  Document::ValidRefPtr document ( attribute->value() );

  Document::Options &options ( document->options() );

  const bool show ( "Show" == options["GlassBoundingBox"] );

  FoxTools::Functions::check( show, object );

  return 1;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update the gui.
//
/////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::updateGUI()
{
  this->_buildSucceededTab ( Usul::Resources::menuBar() );
  this->_buildUncappedTab  ( Usul::Resources::menuBar() );
  this->_buildGroupTab     ( Usul::Resources::menuBar() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tab window to show all the Uncapped loops
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::_buildUncappedTab ( Usul::Interfaces::IUnknown* caller )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );

  this->_buildUncappedTab( caller, document.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tab window to show all the Uncapped loops
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::_buildUncappedTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document )
{
  // Get needed interfaces
  Usul::Interfaces::IFoxTabItem::QueryPtr         tabItem     ( caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr         foxTabBook  ( caller );
  Usul::Interfaces::IGetLoops::QueryPtr           getLoops    ( document );

  // Check to see if these interfaces are valid
  if( tabItem.valid() && foxTabBook.valid() && getLoops.valid() )
  {
    //If we already have a frame, delete it
    if( _uncappedFrame )
    {
      tabItem->deleteTab( "Uncapped" );
      delete _uncappedFrame;
      _uncappedFrame = 0x0;
    }

    // Get the list of uncapped loops
    const Loops &uncapped ( getLoops->getUncappedLoops() );

    //return if there are no loops to add to the tab
    if ( uncapped.empty() )
      return;

    // Add the tab for uncapped loops
    tabItem->addTab ( "Uncapped" );
    FX::FXTabItem *uncappedTab ( tabItem->tabItem( "Uncapped" ) );

    uncappedTab->create();

    // Get the tab book
    FX::FXTabBook *tabBook ( foxTabBook->tabBook() );    

    _uncappedFrame = new FX::FXVerticalFrame ( tabBook, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

    // Let the user now how many loops are uncapped
    std::ostringstream message;
    message << "There are " << uncapped.size() << " uncappped loops.";

    new FX::FXLabel ( _uncappedFrame, message.str().c_str() );

    // Make the list
    _uncappedList = new FX::FXList ( _uncappedFrame, this, TriangleDelegateComponent::ID_GOTO_LOOP, FX::LIST_SINGLESELECT | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

    unsigned int count( 0 );

    // Add each uncapped loop to the list
    for( Loops::const_iterator i = uncapped.begin(); i != uncapped.end(); ++ i )
    {
      std::ostringstream os;
      os << "Loop " << count++;
      if( i->hasInnerLoop() )
        os << "*";
      if( false == i->valid() )
        os << "____";
      _uncappedList->appendItem( os.str().c_str() );
    }
    
    // Add needed buttons
    new FX::FXButton ( _uncappedFrame, "Triangulate",     0x0, this, TriangleDelegateComponent::ID_TRIANGULATE  );
    new FX::FXButton ( _uncappedFrame, "Triangulate All", 0x0, this, TriangleDelegateComponent::ID_TRIANGULATE_ALL  );
    new FX::FXButton ( _uncappedFrame, "Show All Loops",  0x0, this, TriangleDelegateComponent::ID_SHOW_ALL  );
    new FX::FXButton ( _uncappedFrame, "Hide All Loops",  0x0, this, TriangleDelegateComponent::ID_HIDE_ALL  );
    new FX::FXCheckButton ( _uncappedFrame, "Draw Clip Box.", this, TriangleDelegateComponent::ID_CLIP_BOX );

    _uncappedFrame->create();

    Usul::Interfaces::IDocument::ValidQueryPtr doc ( document );
    ActiveLoops::iterator iter = _activeLoops.find( doc.get() );

    if( iter != _activeLoops.end() )
    {
      unsigned int item ( iter->second.first );

      if ( item < static_cast < unsigned int > ( _uncappedList->getNumItems() ) )
        _uncappedList->setCurrentItem( item );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tab window to show all the succeeded loops
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::_buildSucceededTab ( Usul::Interfaces::IUnknown* caller )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );

  this->_buildSucceededTab( caller, document.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tab window to show all the succeeded loops
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::_buildSucceededTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document )
{
  // Get needed interfaces
  Usul::Interfaces::IFoxTabItem::QueryPtr         tabItem     ( caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr         foxTabBook  ( caller );
  Usul::Interfaces::IGetLoops::QueryPtr           getLoops    ( document );

  // Check to see if these interfaces are valid
  if( tabItem.valid() && foxTabBook.valid() && getLoops.valid() )
  {
    //If we already have a frame, delete it
    if( _succeededFrame )
    {
      tabItem->deleteTab( "Succeeded" );
      delete _succeededFrame;
      _succeededFrame = 0x0;
    }

    // Get the loops that are capped
    const Loops &capped ( getLoops->getCappedLoops() );

    //return if there are no loops to add to the tab
    if ( capped.empty() )
      return;

    // Add a tab for succeeded loops
    tabItem->addTab ( "Succeeded" );
    FX::FXTabItem *succeededTab ( tabItem->tabItem( "Succeeded" ) );

    succeededTab->create();

    // Get the tab book
    FX::FXTabBook *tabBook ( foxTabBook->tabBook() );

    _succeededFrame = new FX::FXVerticalFrame ( tabBook, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

    // Let the user now how many loops are capped
    std::ostringstream message;
    message << capped.size() << " loops succeeded.";

    new FX::FXLabel ( _succeededFrame, message.str().c_str() );

    // Make the list
    _succeededList = new FX::FXList ( _succeededFrame, this, TriangleDelegateComponent::ID_GOTO_SUCCEEDED_LOOP, FX::LIST_SINGLESELECT | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

    unsigned int count( 0 );

    // Add each capped loop to the list
    for( Loops::const_iterator i = capped.begin(); i != capped.end(); ++ i )
    {
      std::ostringstream os;
      os << "Loop " << count++;
      if( i->hasInnerLoop() )
        os << "*";
      _succeededList->appendItem( os.str().c_str() );
    }

    // Add needed buttons
    //new FX::FXButton ( _succeededFrame, "Flip Normals", 0x0, this, CapPolygons::ID_FLIP_NORMALS  );
    //new FX::FXButton ( _succeededFrame, "Undo",         0x0, this, CapPolygons::ID_UNDO_LOOP     );
    new FX::FXButton ( _succeededFrame, "Done",         0x0, this, TriangleDelegateComponent::ID_LOOP_DONE     );

    _succeededFrame->create();

    Usul::Interfaces::IDocument::ValidQueryPtr doc ( document );
    ActiveLoops::iterator iter = _activeLoops.find( doc.get() );

    if( iter != _activeLoops.end() )
    {
      unsigned int item ( iter->second.second );

      if ( item < static_cast < unsigned int > ( _succeededList->getNumItems() ) )
        _succeededList->setCurrentItem( item );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to selected loop
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandGotoLoop ( FX::FXObject *object, FX::FXSelector, void* )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::QueryPtr document       ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IGetLoops::QueryPtr getLoops       ( document );

  if( false == getLoops.valid() )
    return 0;

  // Get the loops that are not capped
  Loops &uncapped ( getLoops->getUncappedLoops() );

  //Get which item is selected
  const unsigned int current ( _uncappedList->getCurrentItem() );

  //Go to the loop
  this->_gotoLoop ( uncapped.at( current ), document, current );

  ActiveLoops::iterator iter = _activeLoops.find( document.get() );

  if( iter == _activeLoops.end() )
  {
    UIntPair p ( current, 0 );
    _activeLoops.insert( ActiveLoops::value_type( document.get(), p ) );
  }
  else
  {
    iter->second.first = current;
  }

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to Succeeded loop
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandGotoSucceededLoop ( FX::FXObject *object, FX::FXSelector, void* )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IGetLoops::ValidQueryPtr    getLoops    ( document );

  // Get the loops that have been capped
  Loops &capped ( getLoops->getCappedLoops() );

  //Get which item is selected
  const unsigned int current ( _succeededList->getCurrentItem() );

  //Go to the loop
  this->_gotoLoop ( capped.at( current ), document, current );

  ActiveLoops::iterator iter = _activeLoops.find( document.get() );

  if( iter == _activeLoops.end() )
  {
    UIntPair p ( 0, current );
    _activeLoops.insert( ActiveLoops::value_type( document.get(), p ) );
  }
  else
  {
    iter->second.second = current;
  }

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to given loop
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::_gotoLoop ( Loop& loop, Usul::Interfaces::IUnknown* caller, const int index )
{
  //Needed to build a frame
  osg::Vec3 center ( 0.0, 0.0, 0.0 );
  float distance ( 0.0 );
  osg::Quat rotation ( 0.0, 0.0, 0.0, 0.0 );

  //Get the data needed to make a frame
  loop.getFrameData ( center, distance, rotation, caller );

  //Interface that animates
  Usul::Interfaces::IAnimate::QueryPtr animate ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IAnimate::IID ) );

  //Animate if we have a valid interface
  if( animate.valid() )
    animate->animate( center, distance, rotation );
  
  // Get the active view
  Usul::Interfaces::IViewer::QueryPtr activeView ( Usul::Documents::Manager::instance().active()->activeView() );

  Usul::Interfaces::IGroup::QueryPtr group ( activeView );

  if( group.valid() )
  {
    //Remove the group if it exists
    group->removeGroup ( "Selection_Loop" );

    osg::ref_ptr< osg::Group > g ( group->getGroup ( "Selection_Loop" ) );

    g->addChild ( this->_drawLoop( loop, index, caller ) );

  }

  Usul::Interfaces::IClippingPlanes::QueryPtr clip ( activeView );
  if ( _useClipBox  && clip.valid() )
  {
    clip->removeClippingPlanes();

    const float d ( distance /= 3 );

    osg::Vec3 minBB ( center.x() - d, center.y() - d, center.z() - d );
    osg::Vec3 maxBB ( center.x() + d, center.y() + d, center.z() + d );

    osg::BoundingBox bb ( minBB, maxBB );

    clip->addClippingBox ( bb );

    if( activeView.valid() )
      activeView->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Triangulate selected loop
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandTriangulate ( FX::FXObject *, FX::FXSelector, void* )
{
  // Return now if there are the uncapped list is empty.
  if( _uncappedList->getNumItems() == 0 )
    return 0;

  // Get what item is selected
  const unsigned int current ( _uncappedList->getCurrentItem() );

  // Triangulte the selected loop.  Build on the fly.
  if ( this->_triangulateSingle ( current, true ) )
  {
    // Build the tabs
    this->_buildUncappedTab  ( Usul::Resources::menuBar() );
    this->_buildSucceededTab ( Usul::Resources::menuBar() );

    // Get the interfaces.
    Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );
    Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );

    // Send the messages to render the scene
    sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

    // Document has been modified
    document->modified ( true );
  }
  else
  {
    // Build the message text
    std::ostringstream out;
    out << "Capping loop " << current << " Failed.";

    // Show the dialog.
    FoxTools::Dialogs::Message::error ( "OK", "Triangulation Failed", out.str() );
  }

  // Message handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Triangulate all
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandTriangulateAll ( FX::FXObject *, FX::FXSelector, void* )
{
  // Get the needed interfaces
  Usul::Interfaces::IFlushEvents::ValidQueryPtr flush       ( Usul::Resources::menuBar() );
  Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );
  Usul::Interfaces::IGetLoops::ValidQueryPtr    getLoops    ( document );

  // Get the loops that are not capped
  Loops &uncapped ( getLoops->getUncappedLoops() );

  //Return now if no uncapped loops
  if( uncapped.empty() )
    return 0;

  //Show the cancel button
  Usul::Interfaces::ICancelButton::ShowHide cancel ( Usul::Resources::menuBar() );

  // Status bar Update
  Usul::Interfaces::IStatusBar::UpdateStatusBar status ( Usul::Resources::menuBar() );

  // Progress bar update
  Usul::Interfaces::IProgressBar::UpdateProgressBar updateProgress ( 0.0, 1.0, Usul::Resources::menuBar() );

  //For user feedback
  unsigned int success ( 0 );
  unsigned int failed ( 0 );

  //Get the size because it will change below
  const unsigned int size ( uncapped.size() );

  //For though all of our uncapped loops
  for( unsigned int i = 0; i < uncapped.size(); ++i )
  {
    // Trianglulate the loop.  Do not build on the fly.
    if ( this->_triangulateSingle( i, true ) )
    {
      ++success;

      //Need to decrement because if the loop succeeded it has been removed from uncapped list
      --i;
    }
    else
      ++failed;

    //Build message for user feedback
    std::ostringstream message;
    message << "Success: " << success << " Failed: " << failed << " Number Left: " << ( size - failed - success );
    status ( message.str(), true );

    //Update the progress bar
    updateProgress ( success + failed, size );

    //Make sure app is responsive
    flush->flushEventQueue();
  }

  //Build the tabs
  this->_buildUncappedTab ( Usul::Resources::menuBar() );
  this->_buildSucceededTab( Usul::Resources::menuBar() );

  // Send the message to build and render the scene
  sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
  sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  // Document has been modified
  document->modified( true );

  // Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Triangulate a single loop
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleDelegateComponent::_triangulateSingle( unsigned int position, bool buildOnFly  )
{
  //Get needed interfaces.
  Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IGetLoops::ValidQueryPtr    getLoops    ( document );
 
  // Get both uncapped and capped loops
  Loops &uncapped ( getLoops->getUncappedLoops() );
  Loops &capped   ( getLoops->getCappedLoops()   );

  // Get the loop at the given position
  Loop& loop ( uncapped.at( position ) );
  
  if( false == loop.valid() )
    return false;

  bool succeeded ( false );

  //If one fails, we don't want to abandon the rest of the loops
  try
  {
    // Try to build triangles for this loop
    if ( loop.triangulate ( document, buildOnFly ) )
      succeeded = true;
  }
  //Eat all execptions
  catch( ... )
  {
  }

  if( succeeded )
  {
    //Add this loop to succeeded
    capped.push_back( loop );

    // Remove this loop from uncapped
    uncapped.erase ( ( uncapped.begin() + position ) );
  }

  // Return true if the loop was capped
  return succeeded;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove loop from succeeded list.  Loop is then gone forever
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandLoopDone ( FX::FXObject *, FX::FXSelector, void* )
{
  //Return now if the list is empty
  if( _succeededList->getNumItems() == 0 )
    return 0;

  //Get the current selection
  unsigned int current ( _succeededList->getCurrentItem() );

  // Get needed interfaces
  Usul::Interfaces::IGroup::QueryPtr            group       ( Usul::Documents::Manager::instance().active()->activeView() );
  Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IGetLoops::ValidQueryPtr    getLoops    ( document );

  // Get loops that are capped
  Loops &capped   ( getLoops->getCappedLoops()   );

  //Get iterator to selected loop
  Loops::iterator doomed ( capped.begin() + current );

  //Erase loop from succeeded list
  capped.erase ( doomed );

  //Rebuild the tab
  this->_buildSucceededTab( Usul::Resources::menuBar() );  

  //If we have a valid interface...
  if( group.valid() )
  {
    //Remove the group if it exists
    group->removeGroup ( "Selection_Loop" );
  }

  //Message Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show all the loops.
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandShowAll ( FX::FXObject *, FX::FXSelector, void* )
{
  //Get needed interfaces.
  Usul::Interfaces::IDocument::ValidQueryPtr    document    ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IGetLoops::ValidQueryPtr    getLoops    ( document );
  Usul::Interfaces::IGroup::QueryPtr            group       ( Usul::Documents::Manager::instance().active()->activeView() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );

  // Get both uncapped and capped loops
  Loops &uncapped ( getLoops->getUncappedLoops() );

  if( group.valid() )
  {
    //Remove the group if it exists
    group->removeGroup ( "Selection_Loop" );

    osg::ref_ptr< osg::Group > g ( group->getGroup ( "Selection_Loop" ) );

    //Lets put the index number inside the loop.
    osg::Vec4 layoutColor(1.0f,0.0f,0.0f,1.0f);
    
    for( Loops::iterator iter = uncapped.begin(); iter != uncapped.end(); ++iter )
    {
      g->addChild ( this->_drawLoop( *iter, iter - uncapped.begin(), document ) );
    }
  }

  sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide all the loops.
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandHideAll ( FX::FXObject *, FX::FXSelector, void* )
{
  //Get needed interfaces
  Usul::Interfaces::IGroup::QueryPtr            group       ( Usul::Documents::Manager::instance().active()->activeView() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( Usul::Documents::Manager::instance().active() );

  if( group.valid() )
  {
    //Remove the group if it exists
    group->removeGroup ( "Selection_Loop" );
  }

  sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the loop.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode * TriangleDelegateComponent::_drawLoop ( OsgTools::Triangles::Loop& loop, unsigned int number, Usul::Interfaces::IUnknown *caller )
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  // Get or create the state set
  osg::ref_ptr<osg::StateSet> ss ( geode->getOrCreateStateSet() );

  // Set the line width
  OsgTools::State::StateSet::setLineWidth ( geode.get(), 3.0f );

  // Turn off lighting.
  OsgTools::State::StateSet::setLighting( geode.get(), false );

  // Build the vertex list
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->reserve ( loop.size() );

  osg::Vec3 center;
  
  for( unsigned int i = 0; i < loop.size(); ++ i )
  {
    osg::Vec3 v ( loop.vertex( i, caller ) );
    vertices->push_back( loop.vertex( i, caller ) );

    center += v;
  }

  center /= vertices->size();

  std::ostringstream os;
  os << number;

  osg::ref_ptr <osg::Drawable> text ( this->_makeNumber ( os.str(), center, osg::Vec4 ( 1.0, 0.0, 1.0, 1.0 ), 40.0f ) );
  geode->addDrawable ( text.get() );

  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );

  colors->push_back ( osg::Vec4( 0.0, 1.0, 1.0, 1.0 ) );

  geometry->setVertexArray ( vertices.get() );
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_LOOP, 0, vertices->size() ) );

  // Draw any inner loops.
  for( unsigned int i = 0; i < loop.numInnerLoops(); ++i )
  {
    osg::ref_ptr< osg::Geometry > geom ( new osg::Geometry );

    const Loop l ( loop.points( i ) );

    osg::ref_ptr < osg::Vec3Array > verts ( new osg::Vec3Array );
    verts->reserve ( l.size() );

    for( unsigned int i = 0; i < l.size(); ++ i ) {
      osg::Vec3 v ( l.vertex( i, caller ) );
      verts->push_back( l.vertex( i, caller ) );
    }

    osg::ref_ptr< osg::Vec4Array > c ( new osg::Vec4Array );

    c->push_back ( osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );

    geom->setVertexArray ( verts.get() );
    geom->setColorArray ( c.get() );
    geom->setColorBinding( osg::Geometry::BIND_OVERALL );
    geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_LOOP, 0, verts->size() ) );

    geode->addDrawable( geom.get() );
  }
  
  geode->addDrawable ( geometry.get() );
  
  // Add a numeric Label to each point in the loop which will be the index
  osg::Vec4 layoutColor(1.0f,0.0f,0.0f,1.0f);
  float layoutCharacterSize = 24.0f;  
  for( unsigned int i = 0; i < loop.size(); ++ i )
  {
    std::stringstream ss;
    std::string str;
    ss << i;
    ss >> str;
    geode->addDrawable( this->_makeNumber ( str,  loop.vertex( i, caller ), layoutColor, layoutCharacterSize  ) );
  }
  
  return geode.release();
}

osg::Drawable* TriangleDelegateComponent::_makeNumber ( const std::string& name, const osg::Vec3& pos, const osg::Vec4& color, float size )
{
  osg::ref_ptr< osgText::Text > text ( new osgText::Text );
  text->setColor( color );
  text->setCharacterSize( size );
  text->setPosition ( pos );
  text->setText( name );
  text->setAutoRotateToScreen(true);
  text->update();

  return text.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on clip box.
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandClipBox  ( FX::FXObject *, FX::FXSelector, void* )
{
  // Toggle the clip box flag.
  _useClipBox = !_useClipBox;

  if( false == _useClipBox )
  {
    Usul::Interfaces::IClippingPlanes::QueryPtr clip ( Usul::Documents::Manager::instance().active()->activeView() );
    if( clip.valid() )
      clip->removeClippingPlanes();
  }
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update use clip box check box.
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onUpdateClipBox  ( FX::FXObject *object, FX::FXSelector, void* )
{
  FX::FXCheckButton *check = SAFE_CAST_FOX ( FX::FXCheckButton, object );
  if ( 0x0 == check )
    return 0;

  check->setCheck ( _useClipBox );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tab to hide/show groups.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::_buildGroupTab ( Usul::Interfaces::IUnknown* caller )
{
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );

  this->_buildGroupTab ( caller, document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tab to hide/show groups.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleDelegateComponent::_buildGroupTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document )
{
  // Get needed interfaces.
  Usul::Interfaces::IFoxTabItem::QueryPtr         tabItem     ( caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr         foxTabBook  ( caller );
  Usul::Interfaces::IGroupPrimitives::QueryPtr    groups      ( document );

  // Check to see if these interfaces are valid.
  if( tabItem.valid() && foxTabBook.valid() && groups.valid() )
  {
    //If we already have a frame, delete it.
    if( _groupFrame )
    {
      tabItem->deleteTab( "Groups" );
      delete _groupFrame;
      _groupFrame = 0x0;
    }

    // If there are one or zero groups, don't build the tab.
    if( groups->groupsNumber() <= 1 )
      return;

    // Add a tab for groups.
    tabItem->addTab ( "Groups" );
    FX::FXTabItem *groupTab ( tabItem->tabItem( "Groups" ) );

    groupTab->create();

    // Get the tab book.
    FX::FXTabBook *tabBook ( foxTabBook->tabBook() );

    _groupFrame = new FX::FXVerticalFrame ( tabBook, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

    //FX::FXHorizontalFrame *hframe ( new FX::FXHorizontalFrame ( _groupFrame, FX::LAYOUT_FILL_X ) );

    //FX::FXVerticalFrame *vframe1 ( new FX::FXVerticalFrame ( hframe, FX::LAYOUT_FILL_Y ) );

    //FX::FXVerticalFrame *vframe2 ( new FX::FXVerticalFrame ( hframe, FX::LAYOUT_FILL_Y ) );

    const unsigned int numGroups ( groups->groupsNumber() );

    // Let the user now how many groups there are.
    std::ostringstream message;
    message << numGroups << " connected groups.";

    new FX::FXLabel ( _groupFrame, message.str().c_str() );

    // Make the list
    _groupList = new FX::FXList ( _groupFrame, this, TriangleDelegateComponent::ID_TOGGLE_GROUP, FX::LIST_MULTIPLESELECT | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

    for( unsigned int i = 0; i < numGroups; ++i )
    {
      std::ostringstream os;
      os << "Group " << i;
      _groupList->appendItem( os.str().c_str() );

      Usul::Interfaces::IPrimitiveGroup::QueryPtr primitiveGroup ( groups->getPrimitiveGroup( i ) );

      if( primitiveGroup.valid() )
        _groupList->getItem(i)->setSelected( primitiveGroup->getVisibility() );
    }


    new FX::FXButton ( _groupFrame, "Hide all groups", 0x0, this, TriangleDelegateComponent::ID_HIDE_GROUPS );
    new FX::FXButton ( _groupFrame, "Show all groups", 0x0, this, TriangleDelegateComponent::ID_SHOW_GROUPS );

    //FX::FXHorizontalFrame *transFrame ( new FX::FXHorizontalFrame ( vframe2, FX::LAYOUT_FILL_X ) );
    new FX::FXLabel ( _groupFrame, "Transparency: " );
    FX::FXRealSpinner *spinner ( new FX::FXRealSpinner ( _groupFrame, 3, this, ID_GROUP_TRANSPARENCY ) );
    spinner->setIncrement( 0.05 );
    spinner->setRange( 0.0, 1.0 );

    _groupFrame->create();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the showing of the group.
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandToggleGroup ( FX::FXObject *object, FX::FXSelector, void *data )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );
  Usul::Interfaces::IGroupPrimitives::ValidQueryPtr groups  ( document );

  unsigned int numItems ( _groupList->getNumItems() );

  for ( unsigned int i = 0; i < numItems; ++i )
  {
    Usul::Interfaces::IPrimitiveGroup::QueryPtr primitiveGroup ( groups->getPrimitiveGroup( i ) );

    // If we have a valid pointer...
    if( primitiveGroup.valid() )
    {
      // Set the proper visibility.
      if( _groupList->isItemSelected ( i ) )
      {
        primitiveGroup->setVisibility ( true );
      }
      else
      {
        primitiveGroup->setVisibility ( false );
      }
    }
  }

  sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group shown?
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onUpdateToggleGroup ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IGroupPrimitives::ValidQueryPtr groups  ( document );

  unsigned int numItems ( _groupList->getNumItems() );

  if ( numItems != groups->groupsNumber() )
  {
    return 0;
  }

  for ( unsigned int i = 0; i < numItems; ++i )
  {
    Usul::Interfaces::IPrimitiveGroup::QueryPtr primitiveGroup ( groups->getPrimitiveGroup( i ) );

    // If we have a valid pointer...
    if( primitiveGroup.valid() )
    {
      if( primitiveGroup->getVisibility( ) )
      {
        _groupList->getItem(i)->setSelected( true );
      }
      else
      {
        _groupList->getItem(i)->setSelected( false );
      }
    }
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide all groups.
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandHideGroups ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );
  Usul::Interfaces::IGroupPrimitives::ValidQueryPtr groups  ( document );

  for ( unsigned int i = 0; i < groups->groupsNumber(); ++i )
  {
    Usul::Interfaces::IPrimitiveGroup::QueryPtr primitiveGroup ( groups->getPrimitiveGroup( i ) );

    // If we have a valid pointer...
    if( primitiveGroup.valid() )
    {
      primitiveGroup->setVisibility( false );
    }
  }

  this->onUpdateToggleGroup( 0x0, 0, 0x0 );
  _groupList->repaint();
  _groupList->forceRefresh();
  _groupList->update();
  FoxTools::Functions::application()->flush();

  sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show all groups.
//
///////////////////////////////////////////////////////////////////////////////

long TriangleDelegateComponent::onCommandShowGroups ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );
  Usul::Interfaces::IGroupPrimitives::ValidQueryPtr groups  ( document );

  for ( unsigned int i = 0; i < groups->groupsNumber(); ++i )
  {
    Usul::Interfaces::IPrimitiveGroup::QueryPtr primitiveGroup ( groups->getPrimitiveGroup( i ) );

    // If we have a valid pointer...
    if( primitiveGroup.valid() )
    {
      primitiveGroup->setVisibility( true );
    }
  }

  this->onUpdateToggleGroup( 0x0, 0, 0x0 );
  _groupList->repaint();
  _groupList->forceRefresh();
  _groupList->update();
  FoxTools::Functions::application()->flush();

  sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  return 1;
}


long TriangleDelegateComponent::onCommandGroupTransparency ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner  ( SAFE_CAST_FOX ( FX::FXRealSpinner, object ) );
  if( 0x0 == spinner )
    return 0;

  // Get needed interfaces
  Usul::Interfaces::IDocument::ValidQueryPtr document       ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IGroupPrimitives::ValidQueryPtr groups  ( document );

  unsigned int numItems ( _groupList->getNumItems() );

  if ( numItems != groups->groupsNumber() )
  {
    return 0;
  }

  for ( unsigned int i = 0; i < numItems; ++i )
  {
    Usul::Interfaces::IPrimitiveGroup::QueryPtr primitiveGroup ( groups->getPrimitiveGroup( i ) );

    // If we have a valid pointer...
    if( primitiveGroup.valid() )
    {
      if( _groupList->isItemSelected( i ) )
      {
          primitiveGroup->setTransparency ( spinner->getValue() );
      }
    }
  }


  return 1;
}

long TriangleDelegateComponent::onUpdateGroupTransparency  ( FX::FXObject *object, FX::FXSelector, void * )
{
#if 0
  FX::FXRealSpinner *spinner  ( SAFE_CAST_FOX ( FX::FXRealSpinner, object ) );
  if( 0x0 == spinner )
    return 0;

  int current ( _groupList->getCurrentItem() );
  if( current < 0 )
    spinner->disable();
  else
  {
    spinner->enable();

    std::istringstream in ( _groupList->getItem(current)->getText().text() );

    float t;
    in >> t;

    spinner->setValue ( t );
  }
#endif
  return 1;
}