
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

#include "FoxComponent.h"

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
#include "Usul/Functors/Pair.h"
#include "Usul/Scope/Reset.h"

#include <stdexcept>
#include <iostream>
#include <limits>

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
  _force       ( false )
{
  // Set these resources.
  Usul::Resources::textWindow ( this->_unknown() );
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
  // Clear the map of windows.
  _windows.clear();

  // Tell every window to detach from its gui-object.
  this->_detachAllWindows();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell every window to detach from its gui-object.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_detachAllWindows()
{
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
  // Return now if we are not dirty.
  if ( false == _app.dirty() )
    return;

  // Initialize.
  const bool firstTime ( 0x0 == _foxApp );

  // If this is the first time...
  if ( firstTime )
  {
    // Make the application.
    _foxApp = new FoxTools::App::Application ( _app.name(), _app.vendor() );

    // Initialize it.
    Usul::CommandLine::Arguments &args ( Usul::CommandLine::Arguments::instance() );
    _foxApp->init ( args.argc(), args.argv() );

    // Make a tool tip.
    new FXToolTip ( _foxApp, 0 );
  }

  // Build the main window.
  this->_buildMainWindow ( _app.mainWindow() );

  // If this is the first time...
  if ( firstTime )
  {
    // Create the application.
    _foxApp->create();
  }

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
  // Make sure we are supposed to be here.
  if ( 0x0 == _foxApp || 0x0 == mainWin || false == mainWin->dirty() )
    return;

  // Get the existing fox main window.
  FX::FXMainWindow *foxMainWin ( Detail::FoxObject<FX::FXMainWindow>::get ( mainWin ) );

  // If this is the first time...
  if ( 0x0 == foxMainWin )
  {
    // Application icon.
    typedef FoxTools::Icons::Factory Icons;
    FX::FXIcon *icon ( Icons::instance()->icon ( Icons::ICON_LOGO ) );

    // Get appropriate size.
    FoxRect size ( this->_initialMainWindowSize() );

    // Make main window.
    foxMainWin = new FX::FXMainWindow ( _foxApp, _foxApp->getAppName(), icon, icon, FX::DECOR_ALL, size[0], size[1], size[2], size[3] );
    this->_newWindow ( foxMainWin, mainWin );
  }

  // Make these other components.
  this->_buildMenuBar ( foxMainWin, mainWin->menuBar() );
  this->_buildToolBars();

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
//  Build the top-level menu.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildTopMenu ( FX::FXMainWindow *foxMainWin, FX::FXMenuBar *foxMenuBar, AFW::Core::Group *group )
{
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
  // Make sure we are supposed to be here.
  if ( 0x0 == foxMainWin || 0x0 == mainWin || false == mainWin->dirty() || 0 == mainWin->numChildren() )
    return;

  // Make the dock-sites and middle region.
  {
    _dockCircles.clear();
    SplitRegions regions;
    regions[AFW::Core::DockSite::NONE] = foxMainWin;
    const unsigned int numCircles ( 4 );
    for ( unsigned int i = 0; i < numCircles; ++i )
    {
      this->_makeSplitRegions ( regions[AFW::Core::DockSite::NONE], regions );
      _dockCircles.push_back ( regions );
    }
  }

  // Make the mdi-client area.
  FX::FXMDIClient *clientArea ( new FX::FXMDIClient ( _dockCircles.back()[AFW::Core::DockSite::NONE], FX::LAYOUT_FILL ) );
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
    book = new FX::FXTabBook ( parent, this, 0, bookLayout | FX::LAYOUT_FILL );

  // Should always have an even number of children.
  USUL_ASSERT ( 0 == book->numChildren() % 2 );

  // Build the tab.
  const std::string text ( this->_tabItemText ( window ) );
  new FX::FXTabItem ( book, text.c_str(), this->_makeIcon ( window ), itemLayout | FX::LAYOUT_FILL );

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
  // Make sure we are supposed to be here.
  if ( 0x0 == parent || 0x0 == window )
    return 0x0;

  // Common layout.
  const unsigned int layout ( FX::LAYOUT_FILL | FX::LAYOUT_TOP | FX::LAYOUT_LEFT );

  // Get the shutter, add it if we need to.
  FX::FXWindow *child0 ( ( parent->numChildren() > 0 ) ? parent->childAtIndex ( 0 ) : 0x0 );
  FX::FXShutter *shutter ( SAFE_CAST_FOX ( FX::FXShutter, child0 ) );
  if ( 0x0 == shutter )
    shutter = new FX::FXShutter ( parent, this, 0, FX::FRAME_SUNKEN | layout );

  // Build the tab.
  const std::string text ( this->_tabItemText ( window ) );
  FX::FXShutterItem *item ( new FX::FXShutterItem ( shutter, text.c_str(), this->_makeIcon ( window ), layout ) );
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

void FoxComponent::_makeSplitRegions ( FX::FXComposite *parent, SplitRegions &regions )
{
  // Make sure we are supposed to be here.
  if ( 0x0 == parent )
    return;

  // Make the two regions.
  ThreeWaySplit v, h;
  this->_makeThreeWaySplit ( parent, FX::SPLITTER_VERTICAL,   v );
  this->_makeThreeWaySplit ( v[1],   FX::SPLITTER_HORIZONTAL, h );

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

void FoxComponent::_makeThreeWaySplit ( FX::FXComposite *foxParent, unsigned int direction, ThreeWaySplit &split )
{
  // Make sure we are supposed to be here.
  if ( 0x0 == foxParent )
    return;

  // This creates the nested splitters and the contained frame windows.
  FX::FXSplitter  *outer  ( new FX::FXSplitter ( foxParent, direction | FX::LAYOUT_FILL ) );
  FX::FXComposite *dock1  ( this->_makeFrame ( outer ) );
  FX::FXSplitter  *inner  ( new FX::FXSplitter ( outer, direction | FX::SPLITTER_REVERSED | FX::LAYOUT_FILL ) );
  FX::FXComposite *middle ( this->_makeFrame ( inner ) );
  FX::FXComposite *dock2  ( this->_makeFrame ( inner ) );

  // Hide the dock-sites.
  dock1->hide();
  dock2->hide();

  // Set colors.
  this->_setColor ( outer );
  this->_setColor ( dock1 );
  this->_setColor ( inner );
  this->_setColor ( middle );
  this->_setColor ( dock2 );

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
  // Make sure we are supposed to be here.
  if ( 0x0 == splitter )
    return 0x0;

  // Create the appropriate frame. It will be a child of a splitter.
  if ( Usul::Bits::has ( splitter->getSplitterStyle(), FX::SPLITTER_HORIZONTAL ) )
  {
    FX::FXHorizontalFrame *frame ( new FX::FXHorizontalFrame ( splitter, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
    frame->setTarget ( this );
    frame->setSelector ( 0 );
    return frame;
  }
  else
  {
    FX::FXVerticalFrame *frame ( new FX::FXVerticalFrame ( splitter, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
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
  // Make sure we are supposed to be here.
  if ( 0x0 == parent )
    return 0x0;

  // Create the appropriate frame. It will be a child of a splitter.
  if ( AFW::Core::DockSite::TOP == type || AFW::Core::DockSite::BOTTOM == type )
  {
    FX::FXHorizontalFrame *frame ( new FX::FXHorizontalFrame ( parent, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
    frame->setTarget ( this );
    frame->setSelector ( 0 );
    return frame;
  }
  else
  {
    FX::FXVerticalFrame *frame ( new FX::FXVerticalFrame ( parent, FX::LAYOUT_FILL, FRAME_BORDER_SPACING ) );
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
  if ( 0x0 == _foxApp )
    return;

  try
  {
    this->_cleanup();
    Usul::Resources::textWindow ( 0x0 );
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
//  Update the text window(s).
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::updateTextWindow ( bool force )
{
  // Set and reset this flag.
  const bool saved ( _force );
  Usul::Scope::Reset<bool> reset ( _force, true, saved );

  // Make the visitor.
  AFW::Core::BaseVisitor::RefPtr visitor 
    ( AFW::Core::MakeVisitor<AFW::Core::TextWindow>::make 
      ( Usul::Adaptors::memberFunction 
        ( this, &FoxComponent::_callUpdateActions ) ) );

  // Visit the tree.
  AFW::Core::Application::instance().accept ( visitor.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Don't allow this to throw because it may create an infinite loop.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::windowTextAppend ( AFW::Core::Window *window, const std::string &text )
{
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
  // Check input.
  if ( 0x0 == window || 0x0 == text || 0 == length )
    return;

  // Set the text.
  window->setText ( text, length );

  // Scroll the window.
  this->_scrollWindowToEnd ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the window's text.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_windowTextAppend ( FX::FXText *window, const char *text, unsigned int length )
{
  // Check input.
  if ( 0x0 == window || 0x0 == text || 0 == length )
    return;

  // Set the text.
  window->appendText ( text, length );

  // Scroll the window.
  this->_scrollWindowToEnd ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scroll the window to the end.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_scrollWindowToEnd ( FX::FXText *window )
{
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
  // Make sure there is an application.
  if ( 0x0 == _foxApp )
    return;

  // If we have a main window...
  FX::FXMainWindow *foxMainWin ( Detail::FoxObject<FX::FXMainWindow>::get ( _app.mainWindow() ) );
  if ( 0x0 != foxMainWin )
  {
    // Is it maximized?
    FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::MAXIMIZED, foxMainWin->isMaximized() );

    // If the main window is not maximized...
    if ( false == foxMainWin->isMaximized() )
    {
      // Write the size and position to the registry.
      FX::FXint x ( Usul::Math::maximum ( ( foxMainWin->getX() ), 0 ) );
      FX::FXint y ( Usul::Math::maximum ( ( foxMainWin->getY() ), 0 ) );
      FX::FXint w ( foxMainWin->getWidth() );
      FX::FXint h ( foxMainWin->getHeight() );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::X,      x );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::Y,      y );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::WIDTH,  w );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::HEIGHT, h );
    }
  }

  // Flush the cached values and actually write them to the registry.
  _foxApp->reg().write();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main window is being closed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onClose ( FXObject *object, FXSelector selector, void *data )
{
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
  // Find the window.
  AFW::Core::Window::RefPtr window ( this->_findWindow ( object ) );
  if ( true == window.valid() )
  {
    // This window no longer has a corresponding gui-object.
    window->guiObject ( 0x0 );
  }

  // Erase the entry if there is any.
  _windows.erase ( object );

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
  FX::FXEvent *event ( reinterpret_cast < FX::FXEvent * > ( data ) );
  if ( event )
  {
    std::string name ( ( object ) ? object->getClassName() : "Unknown" );
    std::cout << "FoxComponent::onMouseMotion(), object = " << name << ", x = " << event->win_x << ", y = " << event->win_y << Usul::Resources::TextWindow::endl;
  }

  // Continue propagation of message.
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the size to make the main window.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FoxRect FoxComponent::_initialMainWindowSize()
{
  // Get dimensions of screen.
  const FXint sw ( USUL_UNSAFE_CAST ( FXint, Usul::System::Screen::width()  ) );
  const FXint sh ( USUL_UNSAFE_CAST ( FXint, Usul::System::Screen::height() ) );

  // Default sizes. (They are correct!)
  const FX::FXint dx ( USUL_UNSAFE_CAST ( FXint, ( 0.1 * sw ) ) );
  const FX::FXint dy ( USUL_UNSAFE_CAST ( FXint, ( 0.1 * sh ) ) );
  const FX::FXint dw ( USUL_UNSAFE_CAST ( FXint, ( 0.8 * sw ) ) );
  const FX::FXint dh ( USUL_UNSAFE_CAST ( FXint, ( 0.8 * sh ) ) );

  // Get dimensions of main window.
  FX::FXint x ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::X,      dx ) );
  FX::FXint y ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::Y,      dy ) );
  FX::FXint w ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::WIDTH,  dw ) );
  FX::FXint h ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::HEIGHT, dh ) );

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
  // If we have an icon...
  if ( w && w->icon() )
  {
    // If the file exists...
    const std::string file ( w->icon()->file() );
    Usul::Predicates::FileExists exists;
    if ( exists ( file ) )
    {
      // Make the icon.
      typedef FoxTools::Icons::Factory Icons;
      return Icons::instance()->icon ( file );
    }
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that the object is being destroyed.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::destroyNotify ( AFW::Core::Window *window )
{
  this->_deleteGuiObject ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that the object is being removed from the scene.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::removeNotify ( AFW::Core::Window *window )
{
  this->_deleteGuiObject ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the gui-object, if any.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_deleteGuiObject ( AFW::Core::Window *window )
{
  // Handle null input.
  if ( 0x0 == window )
    return;

  // Get the fox-object.
  FX::FXObject *fox ( Detail::FoxObject<FX::FXObject>::get ( window ) );
  if ( 0x0 == fox )
    return;

  // Remove the object from our map.
  _windows.erase ( fox );

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
  const char *typeName ( window->typeId().name() );
  return ( ( 0x0 != window && false == window->title().empty() ) ? window->title() : ( ( typeName ) ? typeName : "" ) );
}
