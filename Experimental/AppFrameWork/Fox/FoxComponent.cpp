
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

#include "AppFrameWork/Core/Define.h"
#include "AppFrameWork/Core/Constants.h"
#include "AppFrameWork/Core/GenericVisitor.h"
#include "AppFrameWork/Menus/MenuGroup.h"
#include "AppFrameWork/Menus/MenuBar.h"
#include "AppFrameWork/Menus/Button.h"

#include "Usul/Errors/Assert.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/System/Screen.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Adaptors/MemberFunction.h"

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
//  Message map.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FoxComponent ) MessageMap[] = 
{
  FXMAPFUNC ( FX::SEL_CLOSE,   0, FoxComponent::onClose   ),
  FXMAPFUNC ( FX::SEL_COMMAND, 0, FoxComponent::onCommand ),
  FXMAPFUNC ( FX::SEL_UPDATE,  0, FoxComponent::onUpdate  ),
  FXMAPFUNC ( FX::SEL_DESTROY, 0, FoxComponent::onDestroy ),
};
FOX_TOOLS_IMPLEMENT ( FoxComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FoxComponent() : BaseClass(),
  _app       ( AFW::Core::Application::instance() ),
  _foxApp    ( 0x0 ),
  _windows   ()
{
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
  default:
    return 0x0;
  }
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
      s = t->text();
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
  this->_buildChildren ( mainWin );

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
  if ( 0x0 == foxMainWin || 0x0 == menuBar )
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
//  Update the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::updateApplication()
{
  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write to the registry.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_registryWrite()
{
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
  // Look for a window.
  AFW::Core::Window::RefPtr window ( this->_findWindow ( object ) );
  if ( window.valid() )
    window->callUpdateActions();

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

FX::FXIcon *FoxComponent::_makeIcon ( AFW::Core::Window *w )
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
//  Build the children.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildChildren ( AFW::Core::Frame *parentFrame )
{
  // Get the fox-composite.
  FX::FXComposite *foxParentComposite ( Detail::FoxObject<FX::FXComposite>::get ( parentFrame ) );
  if ( 0x0 == foxParentComposite )
    return;

#if 0

  // Handle no input.
  if ( 0x0 == parentFrame || 0x0 == foxParentComposite )
    return;

  // Are there children?
  if ( 0 == parentFrame->numChildren() )
    return;

  // Initialize.
  FX::FXComposite *childC ( 0x0 );

  // If there is more then one child...
  if ( parentFrame->numChildren() > 1 )
  {
    // Make a splitter to hold the children.
    unsigned int options ( ( AFW::Core::Frame::HORIZONTAL == parentFrame->layout() ) ? FX::SPLITTER_HORIZONTAL : FX::SPLITTER_VERTICAL ); 
    childC = new FX::FXSplitter ( foxParentComposite, options );
  }

  // Otherwise...
  else
  {
    childC = new FX::FXComposite ( foxParentComposite );
  }

  // Hook up the wires.
  this->_newWindow ( childC, parentFrame );

  // For each child...
  for ( AFW::Core::Group::Itr i = parentFrame->begin(); i != parentFrame->end(); ++i )
  {
    // Need to keep making splitters...
  }

  // Not dirty now.
  parent->dirty ( false );

#endif
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


#if 0
clear the maps in notifyClose?
null all gui objects in notifyClose?
set target for all fox objects we make?
use FoxObjectWrappers from the objects, rather than make new ones in callbacks.
#endif
