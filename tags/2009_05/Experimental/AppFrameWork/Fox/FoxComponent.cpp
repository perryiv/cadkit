
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

#include "AppFrameWork/Fox/FoxComponent.h"

#include "AppFrameWork/Core/Define.h"
#include "AppFrameWork/Core/Constants.h"
#include "AppFrameWork/Core/GenericVisitor.h"
#include "AppFrameWork/Core/TextWindow.h"
#include "AppFrameWork/Menus/MenuGroup.h"
#include "AppFrameWork/Menus/MenuBar.h"
#include "AppFrameWork/Menus/Button.h"

#include "FoxTools/App/Application.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Functions/Create.h"
#include "FoxTools/Functions/Delete.h"
#include "FoxTools/Functions/Enable.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Registry/Registry.h"
#include "FoxTools/Dialogs/FileSelection.h"

#include "FoxTools/Headers/Icon.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/ToolTip.h"
#include "FoxTools/Headers/MenuBar.h"
#include "FoxTools/Headers/MenuTitle.h"
#include "FoxTools/Headers/MenuCommand.h"
#include "FoxTools/Headers/Splitter.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/MDIChild.h"
#include "FoxTools/Headers/Text.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/Shutter.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/SplashWindow.h"

#include "Usul/Errors/Assert.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/System/Screen.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Functors/General/Pair.h"
#include "Usul/Scope/Reset.h"

#include <stdexcept>
#include <iostream>
#include <limits>
#include <sstream>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FoxComponent , FoxComponent::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( FoxComponent );


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trace level.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct SetTraceLevel
  {
    SetTraceLevel()
    {
      #ifdef _DEBUG
      FX::fxTraceLevel = 0;
      #endif
    }
  } _setTraceLevel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Definitions for this file.
//
///////////////////////////////////////////////////////////////////////////////

#define FRAME_BORDER_SPACING 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


///////////////////////////////////////////////////////////////////////////////
//
//  Message map.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FoxComponent ) MessageMap[] = 
{
  FXMAPFUNC ( FX::SEL_CHORE,   0, FoxComponent::onChore   ),
  FXMAPFUNC ( FX::SEL_CLOSE,   0, FoxComponent::onClose   ),
  FXMAPFUNC ( FX::SEL_COMMAND, 0, FoxComponent::onCommand ),
  FXMAPFUNC ( FX::SEL_UPDATE,  0, FoxComponent::onUpdate  ),
  FXMAPFUNC ( FX::SEL_DESTROY, 0, FoxComponent::onDestroy ),
  FXMAPFUNC ( FX::SEL_MOTION,  0, FoxComponent::onMouseMotion ),
};
FOX_TOOLS_IMPLEMENT ( FoxComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FoxComponent() : BaseClass(),
  _app         ( AFW::Core::Application::instance() ),
  _foxApp      ( 0x0 ),
  _windows     (),
  _dockCircles (),
  _splitters   (),
  _force       ( false ),
  _mutex       ()
{
  // Set this resource.
  Usul::Resources::textWindow ( this->_unknown() );

  // Load factory functions.
  //AFW::Core::Application::instance().factory ( typeid ( AFW::Core::MainWindow ), new FoxComponent::NewMainWindow );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::~FoxComponent()
{
  try
  {
    this->destroyApplication();
  }
  AFW_CATCH_BLOCK ( "3473786244", "4269789571" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FoxComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IGUIServer::IID:
    return static_cast < Usul::Interfaces::IGUIServer * > ( this );
  case Usul::Interfaces::INotifyClose::IID:
    return static_cast < Usul::Interfaces::INotifyClose * > ( this );
  case Usul::Interfaces::ILoadFileDialog::IID:
    return static_cast < Usul::Interfaces::ILoadFileDialog * > ( this );
  case Usul::Interfaces::IUpdateTextWindow::IID:
    return static_cast < Usul::Interfaces::IUpdateTextWindow * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set appropriate color.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_setColor ( FX::FXWindow *window ) const
{
  Guard guard ( _mutex );
#ifdef _DEBUG
  static Usul::Adaptors::Random<double> random ( 0, 255 );
  if ( window )
  {
    unsigned int r ( static_cast < unsigned int > ( random() ) );
    unsigned int g ( static_cast < unsigned int > ( random() ) );
    unsigned int b ( static_cast < unsigned int > ( random() ) );
    window->setBackColor ( FXRGB ( r, g, b ) );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Form the text in the appropriate way.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > inline std::string makeMenuText ( T *t )
  {
    std::string s;
    if ( t )
    {
      t->textGet ( s );
      unsigned short u ( std::numeric_limits<unsigned short>::max() );
      AFW::Menus::Button    *b ( dynamic_cast < AFW::Menus::Button    * > ( t ) );
      AFW::Menus::MenuGroup *g ( dynamic_cast < AFW::Menus::MenuGroup * > ( t ) );
      if ( b )
      {
        u = b->underline();
      }
      else if ( g )
      {
        u = g->underline();
      }
      if ( u < s.length() )
      {
        s.insert ( u, "&" );
      }
    }
    return s;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the fox object.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > struct FoxObject
  {
    typedef FoxComponent::FoxObjectWrapper FoxObjectWrapper;
    static T *get ( AFW::Core::Window *window )
    {
      // See if there is a window.
      if ( 0x0 == window || 0x0 == window->guiObject() )
        return 0x0;

      // See if there is a gui-object.
      FoxComponent::GuiObjectPtr gui ( window->guiObject() );
      if ( false == gui.valid() )
        return 0x0;

      // See if the gui-object is a fox-wrapper.
      FoxObjectWrapper::RefPtr fox ( dynamic_cast < FoxObjectWrapper * > ( gui.get() ) );
      if ( true == fox.valid() )
        return SAFE_CAST_FOX ( T, fox->value() );

      // If we get to here then it did not work.
      return 0x0;
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to hook up the wires.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_newWindow ( FX::FXWindow *fox, AFW::Core::Window *window )
{
  Guard guard ( _mutex );

  // Handle null input.
  if ( 0x0 == fox )
    return;

  // Add it to our map.
  _windows[fox] = window;

  // This will create it now if the parent is created.
  FoxTools::Functions::create ( fox );

  // Have the window hang on to the fox-window.
  window->guiObject ( new FoxObjectWrapper ( fox ) );

  // Send all events here.
  fox->setTarget ( this );
  fox->setSelector ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the component.
//
///////////////////////////////////////////////////////////////////////////////

bool FoxComponent::notifyClose ( Usul::Interfaces::IUnknown * )
{
  Guard guard ( _mutex );

  // Write to the registry.
  this->_registryWrite();

  // Clean this instance.
  this->_cleanup();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up this instance.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_cleanup()
{
  Guard guard ( _mutex );

  // Clear these containers.
  _windows.clear();
  _dockCircles.clear();
  _splitters.clear();

  // Tell every window to detach from its gui-object.
  this->_detachAllWindows();

  // No more text window.
  Usul::Resources::textWindow ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell every window to detach from its gui-object.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_detachAllWindows()
{
  Guard guard ( _mutex );
  typedef AFW::Core::Window Window;
  for ( Window::WindowListItr i = Window::allWindowsBegin(); i != Window::allWindowsEnd(); ++i )
  {
    Window::RefPtr window ( *i );
    if ( window.valid() )
      window->guiObject ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::buildApplication()
{
  Guard guard ( _mutex );

  // Return now if we are not dirty.
  if ( false == _app.dirty() )
    return;

  // If this is the first time...
  if ( 0x0 == _foxApp )
  {
    // Make the application.
    _foxApp = new FoxTools::App::Application ( _app.name(), _app.vendor() );

    // Initialize it.
    Usul::CommandLine::Arguments &args ( Usul::CommandLine::Arguments::instance() );
    _foxApp->init ( args.argc(), args.argv() );

    // Make a tool tip.
    new FXToolTip ( _foxApp, 0 );

    // Create the application.
    _foxApp->create();

    // Show splash screen.
    this->_showSplashScreen();

    // Add idle chore.
    _foxApp->addChore ( this, 0 );
  }

  // Build the main window.
  this->_buildMainWindow ( _app.mainWindow() );

  // Application is no longer dirty.
  _app.dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the main window.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildMainWindow ( AFW::Core::MainWindow *mainWin )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == _foxApp || 0x0 == mainWin || false == mainWin->dirty() )
    return;

  // Get the existing fox main window.
  FX::FXMainWindow *foxMainWin ( Detail::FoxObject<FX::FXMainWindow>::get ( mainWin ) );

  // If this is the first time...
  if ( 0x0 == foxMainWin )
  {
    // Application icon.
    std::auto_ptr<FX::FXIcon> icon ( this->_makeIcon ( mainWin ) );

    // Get appropriate size.
    FoxRect size ( this->_initialMainWindowSize ( mainWin ) );

    // Make main window.
    foxMainWin = new FX::FXMainWindow ( _foxApp, _foxApp->getAppName(), icon.get(), icon.get(), FX::DECOR_ALL, size[0], size[1], size[2], size[3] );
    this->_newWindow ( foxMainWin, mainWin );

    // Main window owns the icon now.
    icon.release();
  }

  // Make these other components.
  this->_buildMenuBar ( foxMainWin, mainWin->menuBar() );
  this->_buildToolBars();
  this->_buildStatusBar ( foxMainWin, mainWin->statusBar() );

  // Build the children.
  this->_buildChildren ( foxMainWin, mainWin );

  // No longer dirty.
  mainWin->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildMenuBar ( FX::FXMainWindow *foxMainWin, AFW::Menus::MenuBar *menuBar )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == foxMainWin || 0x0 == menuBar || false == menuBar->dirty() )
    return;

  // Get the existing fox main window.
  FX::FXMenuBar *foxMenuBar ( Detail::FoxObject<FX::FXMenuBar>::get ( menuBar ) );

  // If this is the first time...
  if ( 0x0 == foxMenuBar )
  {
    // Menu bar.
    foxMenuBar = new FX::FXMenuBar ( foxMainWin, FX::LAYOUT_SIDE_TOP | FX::LAYOUT_FILL_X );
    this->_newWindow ( foxMenuBar, menuBar );
  }

  // Clean existing menus.
  this->_cleanChildren ( menuBar );

  // This will delete the top-level menu-titles.
  FoxTools::Functions::deleteChildren ( foxMenuBar );

  // Build all top-level menus.
  for ( AFW::Core::Group::Itr i = menuBar->begin(); i != menuBar->end(); ++i )
  {
    AFW::Core::Window::ValidRefPtr w ( *i );
    this->_buildTopMenu ( foxMainWin, foxMenuBar, dynamic_cast < AFW::Core::Group * > ( w.get() ) );
  }

  // Not dirty now.
  menuBar->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the status-bar.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildStatusBar ( FX::FXMainWindow *foxMainWin, AFW::Core::StatusBar *statusBar )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == foxMainWin || 0x0 == statusBar || false == statusBar->dirty() )
    return;

  // Get the existing fox main window.
  FX::FXStatusBar *foxStatusBar ( Detail::FoxObject<FX::FXStatusBar>::get ( statusBar ) );

  // If this is the first time...
  if ( 0x0 == foxStatusBar )
  {
    // Menu bar.
    foxStatusBar = new FX::FXStatusBar ( foxMainWin, FX::LAYOUT_SIDE_BOTTOM | FX::LAYOUT_FILL_X );
    this->_newWindow ( foxStatusBar, statusBar );
    foxStatusBar->getStatusLine()->setTarget ( this );
    foxStatusBar->getStatusLine()->setSelector ( 0 );
  }

  // Not dirty now.
  statusBar->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the top-level menu.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildTopMenu ( FX::FXMainWindow *foxMainWin, FX::FXMenuBar *foxMenuBar, AFW::Core::Group *group )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == foxMainWin || 0x0 == foxMenuBar || 0x0 == group || false == group->dirty() )
    return;

  // Handle no text.
  const std::string text ( Detail::makeMenuText ( group ) );
  if ( text.empty() )
    return;

  // Make the menu pane and associate it with the group.
  FX::FXMenuPane *pane ( new FX::FXMenuPane ( foxMainWin ) );
  this->_newWindow ( pane, group );

  // Make the title but do not associate it with the group.
  FX::FXMenuTitle *title ( new FX::FXMenuTitle ( foxMenuBar, text.c_str(), 0x0, pane ) );
  FoxTools::Functions::create ( title );

  // Build all sub-menus.
  for ( AFW::Core::Group::Itr i = group->begin(); i != group->end(); ++i )
  {
    AFW::Core::Window::ValidRefPtr w ( *i );
    this->_buildSubMenu    ( pane, dynamic_cast < AFW::Core::Group   * > ( w.get() ) );
    this->_buildMenuButton ( pane, dynamic_cast < AFW::Menus::Button * > ( w.get() ) );
  }

  // Not dirty now.
  group->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu group.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildSubMenu ( FX::FXComposite *parent, AFW::Core::Group *group )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == parent || 0x0 == group || false == group->dirty() )
    return;

  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu button.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildMenuButton ( FX::FXComposite *parent, AFW::Menus::Button *button )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == parent || 0x0 == button || false == button->dirty() )
    return;

  // Make the button.
  const std::string text ( Detail::makeMenuText ( button ) );
  FX::FXMenuCommand *command ( new FX::FXMenuCommand ( parent, text.c_str(), this->_makeIcon ( button ) ) );
  this->_newWindow ( command, button );

  // Not dirty now.
  button->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the main-window's children.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildChildren ( FX::FXMainWindow *foxMainWin, AFW::Core::MainWindow *mainWin )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == foxMainWin || 0x0 == mainWin || false == mainWin->dirty() || 0 == mainWin->numChildren() )
    return;

  // Make the dock-sites and middle region.
  {
    _dockCircles.clear();
    _splitters.clear();
    SplitRegions regions;
    regions[AFW::Core::DockSite::NONE] = foxMainWin;
    const unsigned int numCircles ( 4 );
    for ( unsigned int i = 0; i < numCircles; ++i )
    {
      this->_makeSplitRegions ( i, regions[AFW::Core::DockSite::NONE], regions );
      _dockCircles.push_back ( regions );
    }
  }

  // Make the mdi-client area.
  FX::FXMDIClient *clientArea ( new FX::FXMDIClient ( _dockCircles.back()[AFW::Core::DockSite::NONE], FX::LAYOUT_FILL ) );
  FoxTools::Functions::create ( clientArea );
  clientArea->setTarget ( this );
  clientArea->setSelector ( 0 );

  // Loop through the children.
  {
    for ( AFW::Core::Group::Itr i = mainWin->begin(); i != mainWin->end(); ++i )
    {
      AFW::Core::Window::RefPtr window ( *i );
      this->_buildDockedWindow ( window.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a docked window.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildDockedWindow ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == window || false == window->dirty() || AFW::Core::DockSite::NONE == window->dockState().first )
    return;

  // Get the docking-site window.
  FX::FXComposite *parent ( this->_dockSiteParent ( window ) );
  if ( 0x0 == parent )
    return;

  // Make sure the parent is shown.
  parent->show();

  // Determine the proper container.
  const bool topOrBottom ( AFW::Core::DockSite::TOP    == window->dockState().first ||
                           AFW::Core::DockSite::BOTTOM == window->dockState().first );

  // Build the tab.
  FX::FXComposite *container ( ( topOrBottom ) ? this->_makeTabItem ( window, parent ) : this->_makeShutterItem ( window, parent ) );
  FX::FXComposite *foxWindow ( this->_makeFrame ( container, window->dockState().first ) );
  this->_setColor ( foxWindow );

  // Build the contents.
  this->_buildTextWindow ( foxWindow, dynamic_cast < AFW::Core::TextWindow * > ( window ) );

  // Not dirty now.
  window->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a text-window.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildTextWindow ( FX::FXComposite *parent, AFW::Core::TextWindow *window )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == parent || 0x0 == window || false == window->dirty() )
    return;

  // Build the text-window.
  const unsigned int layout ( FX::TEXT_READONLY | FX::TEXT_WORDWRAP | FX::TEXT_SHOWACTIVE | FX::LAYOUT_FILL );
  FX::FXText *foxWindow ( new FX::FXText ( parent, this, 0, layout ) );
  this->_newWindow ( foxWindow, window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the fox window for the docking site.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXComposite *FoxComponent::_dockSiteParent ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( true == _dockCircles.empty() || 0x0 == window || AFW::Core::DockSite::NONE == window->dockState().first )
    return 0x0;

  // Determine the site and circle. Keep the circle in range.
  const AFW::Core::DockSite::Type site ( window->dockState().first );
  const unsigned int circle ( Usul::Math::minimum ( window->dockState().second, _dockCircles.size() - 1 ) );

  // Get the region.
  SplitRegions &regions ( _dockCircles.at ( circle ) );
  SplitRegions::iterator i ( regions.find ( site ) );
  return ( ( regions.end() == i ) ? 0x0 : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a tab-item. Make the book too if necessary. Return the book.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXComposite *FoxComponent::_makeTabItem ( const AFW::Core::Window *window, FX::FXComposite *parent )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == parent || 0x0 == window )
    return 0x0;

  // Determine the layout.
  const bool top ( ( AFW::Core::DockSite::TOP == window->dockState().first ) ? true : false );
  const unsigned int bookLayout ( ( top ) ? FX::TABBOOK_TOPTABS : FX::TABBOOK_BOTTOMTABS );
  const unsigned int itemLayout ( ( top ) ? FX::TAB_TOP_NORMAL  : FX::TAB_BOTTOM_NORMAL );

  // Get the tab-book, add it if we need to.
  FX::FXWindow *child0 ( ( parent->numChildren() > 0 ) ? parent->childAtIndex ( 0 ) : 0x0 );
  FX::FXTabBook *book ( SAFE_CAST_FOX ( FX::FXTabBook, child0 ) );
  if ( 0x0 == book )
  {
    book = new FX::FXTabBook ( parent, this, 0, bookLayout | FX::LAYOUT_FILL );
    FoxTools::Functions::create ( book );
  }

  // Should always have an even number of children.
  USUL_ASSERT ( 0 == book->numChildren() % 2 );

  // Build the tab.
  const std::string text ( this->_tabItemText ( window ) );
  FX::FXTabItem *item ( new FX::FXTabItem ( book, text.c_str(), this->_makeIcon ( window ), itemLayout | FX::LAYOUT_FILL ) );
  FoxTools::Functions::create ( item );

  // Return the book.
  return book;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a shutter-item. Make the shutter too if necessary. Return the 
//  contents of the shutter item.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXComposite *FoxComponent::_makeShutterItem ( const AFW::Core::Window *window, FX::FXComposite *parent )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == parent || 0x0 == window )
    return 0x0;

  // Common layout.
  const unsigned int layout ( FX::LAYOUT_FILL | FX::LAYOUT_TOP | FX::LAYOUT_LEFT );

  // Get the shutter, add it if we need to.
  FX::FXWindow *child0 ( ( parent->numChildren() > 0 ) ? parent->childAtIndex ( 0 ) : 0x0 );
  FX::FXShutter *shutter ( SAFE_CAST_FOX ( FX::FXShutter, child0 ) );
  if ( 0x0 == shutter )
  {
    shutter = new FX::FXShutter ( parent, this, 0, FX::FRAME_SUNKEN | layout );
    FoxTools::Functions::create ( shutter );
  }

  // Build the tab.
  const std::string text ( this->_tabItemText ( window ) );
  FX::FXShutterItem *item ( new FX::FXShutterItem ( shutter, text.c_str(), this->_makeIcon ( window ), layout ) );
  FoxTools::Functions::create ( item );
  item->getButton()->setIconPosition ( FX::ICON_BEFORE_TEXT );
  item->getContent()->setTarget ( this );
  item->getContent()->setSelector ( 0 );

  // Return the contents.
  return item->getContent();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create splitters to contain the docked children.
//  See: http://www.codeproject.com/docking/prod_profuis.asp
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_makeSplitRegions ( unsigned int circle, FX::FXComposite *parent, SplitRegions &regions )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == parent )
    return;

  // Make the two regions.
  ThreeWaySplit v, h;
  this->_makeThreeWaySplit ( circle, parent, FX::SPLITTER_VERTICAL,   v );
  this->_makeThreeWaySplit ( circle, v[1],   FX::SPLITTER_HORIZONTAL, h );

  // Fill the regions.
  regions[AFW::Core::DockSite::TOP]    = v[0];
  regions[AFW::Core::DockSite::BOTTOM] = v[2];
  regions[AFW::Core::DockSite::LEFT]   = h[0];
  regions[AFW::Core::DockSite::NONE]   = h[1];
  regions[AFW::Core::DockSite::RIGHT]  = h[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a three-way split.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_makeThreeWaySplit ( unsigned int circle, FX::FXComposite *foxParent, unsigned int direction, ThreeWaySplit &split )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == foxParent )
    return;

  // This creates the nested splitters and the contained frame windows.
  FX::FXSplitter  *outer  ( new FX::FXSplitter ( foxParent, direction | FX::LAYOUT_FILL ) );
  FX::FXComposite *dock1  ( this->_makeFrame ( outer ) );
  FX::FXSplitter  *inner  ( new FX::FXSplitter ( outer, direction | FX::SPLITTER_REVERSED | FX::LAYOUT_FILL ) );
  FX::FXComposite *middle ( this->_makeFrame ( inner ) );
  FX::FXComposite *dock2  ( this->_makeFrame ( inner ) );

  // Create if we should.
  FoxTools::Functions::create ( outer );
  FoxTools::Functions::create ( inner );

  // Hide the dock-sites.
  dock1->hide();
  dock2->hide();

  // Set colors.
  this->_setColor ( outer );
  this->_setColor ( dock1 );
  this->_setColor ( inner );
  this->_setColor ( middle );
  this->_setColor ( dock2 );

  // Save splitters in map.
  _splitters[outer] = this->_makeSplitterRegistrySection ( circle, direction, "outer" );
  _splitters[inner] = this->_makeSplitterRegistrySection ( circle, direction, "inner" );

  // Try to set split distance.
  outer->setSplit ( 0, FoxTools::Registry::read ( _splitters[outer], AFW::Registry::Keys::SPLIT_DISTANCE, outer->getSplit ( 0 ) ) );
  inner->setSplit ( 1, FoxTools::Registry::read ( _splitters[inner], AFW::Registry::Keys::SPLIT_DISTANCE, inner->getSplit ( 1 ) ) );

  // Set the result.
  split.set ( dock1, middle, dock2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the appropriate frame.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXComposite *FoxComponent::_makeFrame ( FX::FXSplitter *splitter )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == splitter )
    return 0x0;

  // Create the appropriate frame. It will be a child of a splitter.
  if ( Usul::Bits::has ( splitter->getSplitterStyle(), FX::SPLITTER_HORIZONTAL ) )
  {
    FX::FXHorizontalFrame *frame ( new FX::FXHorizontalFrame ( splitter, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
    FoxTools::Functions::create ( frame );
    frame->setTarget ( this );
    frame->setSelector ( 0 );
    return frame;
  }
  else
  {
    FX::FXVerticalFrame *frame ( new FX::FXVerticalFrame ( splitter, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
    FoxTools::Functions::create ( frame );
    frame->setTarget ( this );
    frame->setSelector ( 0 );
    return frame;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the appropriate frame.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXComposite *FoxComponent::_makeFrame ( FX::FXComposite *parent, AFW::Core::DockSite::Type type )
{
  Guard guard ( _mutex );

  // Make sure we are supposed to be here.
  if ( 0x0 == parent )
    return 0x0;

  // Create the appropriate frame. It will be a child of a splitter.
  if ( AFW::Core::DockSite::TOP == type || AFW::Core::DockSite::BOTTOM == type )
  {
    FX::FXHorizontalFrame *frame ( new FX::FXHorizontalFrame ( parent, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
    FoxTools::Functions::create ( frame );
    frame->setTarget ( this );
    frame->setSelector ( 0 );
    return frame;
  }
  else
  {
    FX::FXVerticalFrame *frame ( new FX::FXVerticalFrame ( parent, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
    FoxTools::Functions::create ( frame );
    frame->setTarget ( this );
    frame->setSelector ( 0 );
    return frame;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean the children by deleting the gui-objects.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_cleanChildren ( AFW::Core::Group *group )
{
  Guard guard ( _mutex );

  // Handle null input.
  if ( 0x0 == group )
    return;

  // Make the visitor.
  AFW::Core::BaseVisitor::RefPtr visitor 
    ( AFW::Core::MakeVisitor<AFW::Core::Window>::make 
      ( Usul::Adaptors::memberFunction 
        ( this, &FoxComponent::_deleteGuiObject ) ) );

  // Visit the group's children.
  for ( AFW::Core::Group::Itr i = group->begin(); i != group->end(); ++i )
  {
    AFW::Core::Window::RefPtr window ( *i );
    if ( window.valid() )
      window->accept ( visitor.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::destroyApplication()
{
  Guard guard ( _mutex );

  if ( 0x0 == _foxApp )
    return;

  try
  {
    this->_cleanup();
    delete _foxApp;
  }

  catch ( ... )
  {
    _foxApp = 0x0;
    throw;
  }

  _foxApp = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::runApplication()
{
  Guard guard ( _mutex );

  // Make sure the application is built.
  this->buildApplication();

  // Show the main window if we have one.
  FX::FXMainWindow *foxMainWin ( Detail::FoxObject<FX::FXMainWindow>::get ( _app.mainWindow() ) );
  if ( 0x0 != foxMainWin )
    foxMainWin->show ( FX::PLACEMENT_DEFAULT );

  // Run the application.
  _foxApp->run();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::runWhileEvents()
{
  Guard guard ( _mutex );
  _foxApp->runWhileEvents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::quitApplication()
{
  Guard guard ( _mutex );

  // Handle no application.
  if ( 0x0 == _foxApp )
    return;

  // Try to stop using the main window.
  FX::FXMainWindow *foxMainpWin ( Detail::FoxObject<FX::FXMainWindow>::get ( _app.mainWindow() ) );
  if ( foxMainpWin )
  {
    // Does the same as clicking on main window's close button.
    foxMainpWin->onCmdClose ( 0x0, 0, 0x0 );
    return;
  }

  // If we get to here then use the fox application.
  _foxApp->exit();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text window(s).
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::updateTextWindow ( bool force )
{
  Guard guard ( _mutex );

  // Set and reset this flag.
  const bool saved ( _force );
  Usul::Scope::Reset<bool> reset ( _force, force, saved );

  // Make the visitor.
  AFW::Core::BaseVisitor::RefPtr visitor 
    ( AFW::Core::MakeVisitor<AFW::Core::TextWindow>::make 
      ( Usul::Adaptors::memberFunction 
        ( this, &FoxComponent::_callUpdateActions ) ) );

  // Visit the tree.
  _app.accept ( visitor.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scroll the window to the end.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::scrollWindowToEnd ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );
  try { this->_scrollWindowToEnd ( Detail::FoxObject<FXText>::get ( window ) ); }
  AFW_CATCH_BLOCK ( 4194517299, 4200331451 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Don't allow this to throw because it may create an infinite loop.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::windowTextAppend ( AFW::Core::Window *window, const std::string &text )
{
  Guard guard ( _mutex );
  try { this->_windowTextAppend ( Detail::FoxObject<FXText>::get ( window ), text.c_str(), text.size() ); }
  AFW_CATCH_BLOCK ( 4258938551, 3759973417 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Don't allow this to throw because it may create an infinite loop.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::windowTextAppend ( AFW::Core::Window *window, const char *text, unsigned int length )
{
  Guard guard ( _mutex );
  try { this->_windowTextAppend ( Detail::FoxObject<FXText>::get ( window ), text, length ); }
  AFW_CATCH_BLOCK ( 3804786765, 1284080828 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Don't allow this to throw because it may create an infinite loop.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::windowTextSet ( AFW::Core::Window *window, const std::string &text )
{
  Guard guard ( _mutex );
  try { this->_windowTextSet ( Detail::FoxObject<FXText>::get ( window ), text.c_str(), text.size() ); }
  AFW_CATCH_BLOCK ( 1370382751, 3745159501 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Don't allow this to throw because it may create an infinite loop.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::windowTextSet ( AFW::Core::Window *window, const char *text, unsigned int length )
{
  Guard guard ( _mutex );
  try { this->_windowTextSet ( Detail::FoxObject<FXText>::get ( window ), text, length ); }
  AFW_CATCH_BLOCK ( 3969755668, 2414501198 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Don't allow this to throw because it may create an infinite loop.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::windowTextGet ( const AFW::Core::Window *window, std::string &text )
{
  Guard guard ( _mutex );
  try { this->_windowTextGet ( Detail::FoxObject<FXText>::get ( const_cast < AFW::Core::Window * > ( window ) ), text ); }
  AFW_CATCH_BLOCK ( 1114033505, 1989253250 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the window's text.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_windowTextGet ( const FX::FXText *window, std::string &s ) const
{
  Guard guard ( _mutex );
  if ( window )
    s = window->getText().text();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the window's text.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_windowTextSet ( FX::FXText *window, const char *text, unsigned int length )
{
  Guard guard ( _mutex );

  // Check input.
  if ( 0x0 == window || 0x0 == text || 0 == length )
    return;

  // Set the text if it is different.
  window->setText ( text, length );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the window's text.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_windowTextAppend ( FX::FXText *window, const char *text, unsigned int length )
{
  Guard guard ( _mutex );

  // Check input.
  if ( 0x0 == window || 0x0 == text || 0 == length )
    return;

  // Set the text.
  window->appendText ( text, length );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scroll the window to the end.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_scrollWindowToEnd ( FX::FXText *window )
{
  Guard guard ( _mutex );

  // Handle bad input.
  if ( 0x0 == window )
    return;

  // Move cursor to the end.
  window->setCursorPos ( ( window->getLength() > 0 ) ? ( window->getLength() - 1 ) : 0 );

  // Scroll to the end.
  window->makePositionVisible ( window->getCursorPos() );

  // Force it if we should.
  if ( _force )
  {
    window->getParent()->layout();
    window->getParent()->repaint();
    this->_flush();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write to the registry.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_registryWrite()
{
  Guard guard ( _mutex );

  // Make sure there is an application.
  if ( 0x0 == _foxApp )
    return;

  // Main window.
  {
    // Get main window.
    AFW::Core::MainWindow::RefPtr mainWin ( _app.mainWindow() );
    if ( mainWin.valid() )
    {
      // Get persistent name.
      const std::string &section ( mainWin->persistentName() );
      if ( false == section.empty() )
      {
        // Get fox main window.
        FX::FXMainWindow *foxMainWin ( Detail::FoxObject<FX::FXMainWindow>::get ( mainWin ) );
        if ( 0x0 != foxMainWin )
        {
          // Is it maximized?
          FoxTools::Registry::write ( mainWin->persistentName(), AFW::Registry::Keys::MAXIMIZED, foxMainWin->isMaximized() );

          // If the main window is not maximized...
          if ( false == foxMainWin->isMaximized() )
          {
            // Write the size and position to the registry.
            const FX::FXint x ( Usul::Math::maximum ( ( foxMainWin->getX() ), 0 ) );
            const FX::FXint y ( Usul::Math::maximum ( ( foxMainWin->getY() ), 0 ) );
            const FX::FXint w ( foxMainWin->getWidth() );
            const FX::FXint h ( foxMainWin->getHeight() );
            FoxTools::Registry::write ( section, AFW::Registry::Keys::X,      x );
            FoxTools::Registry::write ( section, AFW::Registry::Keys::Y,      y );
            FoxTools::Registry::write ( section, AFW::Registry::Keys::WIDTH,  w );
            FoxTools::Registry::write ( section, AFW::Registry::Keys::HEIGHT, h );
          }
        }
      }
    }
  }

  // Splitters.
  {
    // Loop through the splitters.
    for ( Splitters::const_iterator i = _splitters.begin(); i != _splitters.end(); ++i )
    {
      // Get the window and registry section.
      const FX::FXSplitter *splitter ( i->first );
      const std::string &section ( i->second );

      // Write the split distance.
      const unsigned int index ( ( Usul::Bits::has ( splitter->getSplitterStyle(), FX::SPLITTER_REVERSED ) ) ? 1 : 0 );
      const FX::FXint split ( splitter->getSplit ( index ) );
      FoxTools::Registry::write ( section, AFW::Registry::Keys::SPLIT_DISTANCE, split );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called during idle processing if there is a chore registered.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onChore ( FXObject *object, FXSelector selector, void *data )
{
  Guard guard ( _mutex );

  // Flush event queue.
  _app.eventsFlush();

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main window is being closed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onClose ( FXObject *object, FXSelector selector, void *data )
{
  Guard guard ( _mutex );

  // Close down.
  this->notifyClose ( 0x0 );

  // Continue propagation of message.
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onCommand ( FXObject *object, FXSelector selector, void *data )
{
  Guard guard ( _mutex );

  // Look for a command-action.
  AFW::Core::Window::RefPtr window ( this->_findWindow ( object ) );
  if ( window.valid() )
    window->callCommandActions();

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the button needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onUpdate ( FXObject *object, FXSelector selector, void *data )
{
  Guard guard ( _mutex );

  // Try to call the update actions.
  this->_callUpdateActions ( this->_findWindow ( object ) );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the window is destroyed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onDestroy ( FXObject *object, FXSelector selector, void *data )
{
  Guard guard ( _mutex );

  // Find the window.
  AFW::Core::Window::RefPtr window ( this->_findWindow ( object ) );
  if ( true == window.valid() )
  {
    // This window no longer has a corresponding gui-object.
    window->guiObject ( 0x0 );
  }

  // Erase the entry if there is any.
  _windows.erase ( object );
  _splitters.erase ( SAFE_CAST_FOX ( FX::FXSplitter, object ) );

  // Continue propagation of message.
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the window is destroyed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onMouseMotion ( FXObject *object, FXSelector selector, void *data )
{
  Guard guard ( _mutex );

  FX::FXEvent *event ( reinterpret_cast < FX::FXEvent * > ( data ) );
  if ( event )
  {
    //std::string name ( ( object ) ? object->getClassName() : "Unknown" );
    //std::cout << "FoxComponent::onMouseMotion(), object = " << name << ", x = " << event->win_x << ", y = " << event->win_y << Usul::Resources::TextWindow::endl;
  }

  // Continue propagation of message.
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the size to make the main window.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FoxRect FoxComponent::_initialMainWindowSize ( AFW::Core::MainWindow *mainWin )
{
  Guard guard ( _mutex );

  // Get dimensions of screen.
  const FXint sw ( USUL_UNSAFE_CAST ( FXint, Usul::System::Screen::width()  ) );
  const FXint sh ( USUL_UNSAFE_CAST ( FXint, Usul::System::Screen::height() ) );

  // Default sizes. (They are correct!)
  const FX::FXint dx ( USUL_UNSAFE_CAST ( FXint, ( 0.1 * sw ) ) );
  const FX::FXint dy ( USUL_UNSAFE_CAST ( FXint, ( 0.1 * sh ) ) );
  const FX::FXint dw ( USUL_UNSAFE_CAST ( FXint, ( 0.8 * sw ) ) );
  const FX::FXint dh ( USUL_UNSAFE_CAST ( FXint, ( 0.8 * sh ) ) );

  // Get section name.
  const std::string section ( ( mainWin ) ? mainWin->persistentName() : "main_window" );

  // Get dimensions of main window.
  FX::FXint x ( FoxTools::Registry::read ( section, AFW::Registry::Keys::X,      dx ) );
  FX::FXint y ( FoxTools::Registry::read ( section, AFW::Registry::Keys::Y,      dy ) );
  FX::FXint w ( FoxTools::Registry::read ( section, AFW::Registry::Keys::WIDTH,  dw ) );
  FX::FXint h ( FoxTools::Registry::read ( section, AFW::Registry::Keys::HEIGHT, dh ) );

  // Make sure it fits.
  const bool xFits ( ( x > 0  ) && ( x < ( sw - w ) ) );
  const bool yFits ( ( y > 10 ) && ( y < ( sh - h ) ) );
  const bool wFits ( w < sw );
  const bool hFits ( h < sh );
  const bool fits ( xFits && yFits && wFits && hFits );

  // Return proper rectangle.
  if ( fits )
    return FoxComponent::FoxRect ( x, y, w, h );
  else
    return FoxComponent::FoxRect ( dx, dy, dw, dh ) ;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an icon. Returns null if it fails.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXIcon *FoxComponent::_makeIcon ( const AFW::Core::Window *w ) const
{
  Guard guard ( _mutex );
  return ( ( w && w->icon() ) ? this->_makeIcon ( w->icon()->file() ) : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an icon. Returns null if it fails.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXIcon *FoxComponent::_makeIcon ( const std::string &file ) const
{
  Guard guard ( _mutex );

  // If the file exists...
  Usul::Predicates::FileExists exists;
  if ( exists ( file ) )
  {
    // Make the icon.
    typedef FoxTools::Icons::Factory Icons;
    return Icons::instance()->icon ( file );
  }

  // It did not work.
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable/disable the window.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::enableWindow ( bool state, AFW::Core::Window *window )
{
  Guard guard ( _mutex );
  FX::FXWindow *foxWindow ( Detail::FoxObject<FX::FXWindow>::get ( window ) );
  if ( foxWindow )
    FoxTools::Functions::enable ( state, foxWindow );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the window is enabled or disabled.
//
///////////////////////////////////////////////////////////////////////////////

bool FoxComponent::isWindowEnabled ( const AFW::Core::Window *window )
{
  Guard guard ( _mutex );
  const FX::FXWindow *foxWindow ( Detail::FoxObject<FX::FXWindow>::get ( const_cast < AFW::Core::Window * > ( window ) ) );
  return ( ( foxWindow && foxWindow->isEnabled() ) ? true : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get file name.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FileResult FoxComponent::getLoadFileName ( const std::string &title, const Filters &filters )
{
  Guard guard ( _mutex );
  FilesResult result ( this->getLoadFileNames ( title, filters ) );
  return ( ( result.first.empty() ) ? FileResult() : FileResult ( result.first.front(), result.second ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get list of file names.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FilesResult FoxComponent::getLoadFileNames ( const std::string &title, const Filters &filters )
{
  Guard guard ( _mutex );

  // Need this up here.
  FilesResult result;

  // Get main window.
  FX::FXMainWindow *foxMainWin ( Detail::FoxObject<FX::FXMainWindow>::get ( _app.mainWindow() ) );
  if ( 0x0 == foxMainWin )
    return result;

  // Call the file-dialog.
  typedef FoxTools::Dialogs::FileSelection FileDialog;
  result = FileDialog::askForFileNames ( FileDialog::OPEN, title, filters, foxMainWin );

  // Return the list of file names, which may be empty.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the toolbars.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildToolBars()
{
  Guard guard ( _mutex );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that the object is being destroyed.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::destroyNotify ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );
  this->_deleteGuiObject ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that the object is being removed from the scene.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::removeNotify ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );
  this->_deleteGuiObject ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the gui-object, if any.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_deleteGuiObject ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );

  // Handle null input.
  if ( 0x0 == window )
    return;

  // Get the fox-object.
  FX::FXObject *fox ( Detail::FoxObject<FX::FXObject>::get ( window ) );
  if ( 0x0 == fox )
    return;

  // Remove the object from our maps.
  _windows.erase ( fox );
  _splitters.erase ( SAFE_CAST_FOX ( FX::FXSplitter, fox ) );

  // Delete the fox-object safely.
  this->_deleteFoxObject ( fox );

  // This object no longer has a corresponding gui-object.
  window->guiObject ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the fox object.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_deleteFoxObject ( FX::FXObject *object )
{
  Guard guard ( _mutex );
  try
  {
    delete object;
  }
  AFW_CATCH_BLOCK ( "1912870980", "3050351572" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the window.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::Window *FoxComponent::_findWindow ( FX::FXObject *object )
{
  Guard guard ( _mutex );
  WindowsMap::iterator i ( _windows.find ( object ) );
  return ( ( _windows.end() == i ) ? 0x0 : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flush the event queue.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_flush()
{
  Guard guard ( _mutex );

  // Passing TRUE on unix calls XSync instead of XFlush. No effect on Windows.
  if ( _foxApp )
    _foxApp->flush ( TRUE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return unknown pointer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FoxComponent::_unknown()
{
  return this->queryInterface ( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call the update actions.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_callUpdateActions ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );
  if ( window )
    window->callUpdateActions();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the appropriate text for this window's tab-item.
//
///////////////////////////////////////////////////////////////////////////////

std::string FoxComponent::_tabItemText ( const AFW::Core::Window *window ) const
{
  Guard guard ( _mutex );
  const char *typeName ( window->typeId().name() );
  return ( ( 0x0 != window && false == window->title().empty() ) ? window->title() : ( ( typeName ) ? typeName : "" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show splash screen if we have an icon.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_showSplashScreen()
{
  Guard guard ( _mutex );

  if ( 0x0 == _foxApp )
    return;

  std::auto_ptr<FX::FXIcon> icon ( this->_makeIcon ( _app.splashScreen()->file() ) );
  if ( 0x0 == icon.get() )
    return;

  const unsigned int layout ( FX::SPLASH_SIMPLE );
  std::auto_ptr<FX::FXSplashWindow> splash ( new FX::FXSplashWindow ( _foxApp, icon.get(), layout, 1000 ) );
  FoxTools::Functions::create ( splash.get() );
  splash->show ( FX::PLACEMENT_SCREEN );
  _foxApp->flush ( TRUE );
  _foxApp->runModalWhileShown ( splash.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make appropriate string for the registry section.
//
///////////////////////////////////////////////////////////////////////////////

std::string FoxComponent::_makeSplitterRegistrySection ( unsigned int circle, unsigned int direction, const std::string &name ) const
{
  Guard guard ( _mutex );
  std::ostringstream out;
  const std::string dir ( ( FX::SPLITTER_VERTICAL == direction ) ? "vertical" : "horizontal" );
  out << "circle_" << circle << '_' << name << '_' << dir;
  return out.str();
}
