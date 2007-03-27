
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

#ifndef _FOX_GUI_SERVER_COMPONENT_CLASS_H_
#define _FOX_GUI_SERVER_COMPONENT_CLASS_H_

#include "AppFrameWork/Fox/CompileGuard.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/Constants.h"

#include "FoxTools/Headers/Object.h"
#include "FoxTools/Headers/MenuPane.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Properties/Attribute.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IGUIServer.h"
#include "Usul/Interfaces/INotifyClose.h"
#include "Usul/Interfaces/ILoadFileDialog.h"
#include "Usul/Interfaces/IUpdateTextWindow.h"

#include "boost/shared_ptr.hpp"

#include <string>
#include <map>

namespace FoxTools { namespace App { class Application; } }
namespace FX { class FXObject; class FXWindow; class FXMainWindow; }
namespace FX { class FXMenuBar; class FXToolBarShell; class FXIcon; }
namespace FX { class FXComposite; class FXSplitter; class FXPacker; class FXText; }
namespace AFW { namespace Core { class Frame; class Group; class TextWindow; } }
namespace AFW { namespace Menus { class Button; class MenuGroup; } }


class FoxComponent : public Usul::Base::Referenced,
                     public FX::FXObject,
                     public Usul::Interfaces::IPlugin,
                     public Usul::Interfaces::IGUIServer,
                     public Usul::Interfaces::INotifyClose,
                     public Usul::Interfaces::ILoadFileDialog,
                     public Usul::Interfaces::IUpdateTextWindow
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vector4 < FX::FXint > FoxRect;
  typedef boost::shared_ptr < FX::FXMenuPane > MenuPanePtr;
  typedef std::map < FX::FXObject *, AFW::Core::Window * > WindowsMap; // Not reference-counted by design!
  typedef Usul::Properties::Attribute < FX::FXObject * > FoxObjectWrapper;
  typedef AFW::Core::Window::GuiObject GuiObject;
  typedef AFW::Core::Window::GuiObjectPtr GuiObjectPtr;
  typedef Usul::Math::Vector3 < FX::FXComposite * > ThreeWaySplit;
  typedef std::map < AFW::Core::DockSite::Type, FX::FXComposite * > SplitRegions;
  typedef std::vector < SplitRegions > DockCircles;
  typedef std::map < FX::FXSplitter *, std::string > Splitters;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Fox );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FoxComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  FoxComponent();

  // Build the application.
  virtual void                  buildApplication();

  // Destroy the application.
  virtual void                  destroyApplication();

  // Notification that the object is being destroyed.
  virtual void                  destroyNotify ( AFW::Core::Window * );

  // Enable/disable the window.
  virtual void                  enableWindow ( bool state, AFW::Core::Window *window );

  virtual FileResult            getLoadFileName  ( const std::string &title = "Load", const Filters &filters = Filters() );
  virtual FilesResult           getLoadFileNames ( const std::string &title = "Load", const Filters &filters = Filters() );

  // Return name of plugin
  virtual std::string           getPluginName() const { return "FOX GUI Server"; }

  // Return the enabled/disabled state.
  virtual bool                  isWindowEnabled ( const AFW::Core::Window *window );

  // Notify the component.
  virtual bool                  notifyClose ( Usul::Interfaces::IUnknown *caller );

  // FOX message callback.
  long                          onChore       ( FX::FXObject *, FX::FXSelector, void * );
  long                          onClose       ( FX::FXObject *, FX::FXSelector, void * );
  long                          onCommand     ( FX::FXObject *, FX::FXSelector, void * );
  long                          onUpdate      ( FX::FXObject *, FX::FXSelector, void * );
  long                          onDestroy     ( FX::FXObject *, FX::FXSelector, void * );
  long                          onMouseMotion ( FX::FXObject *, FX::FXSelector, void * );

  // Notification that the object is being removed from the scene.
  virtual void                  removeNotify ( AFW::Core::Window * );

  // Run the application.
  virtual void                  runApplication();
  virtual void                  runWhileEvents();

  // Quit the application.
  virtual void                  quitApplication();

  // Scroll the window.
  virtual void                  scrollWindowToEnd ( AFW::Core::Window *window );

  // Update the text window(s).
  virtual void                  updateTextWindow ( bool force );

  // Update the window's text.
  virtual void                  windowTextAppend ( AFW::Core::Window *window, const std::string & );
  virtual void                  windowTextAppend ( AFW::Core::Window *window, const char *, unsigned int length );
  virtual void                  windowTextGet    ( const AFW::Core::Window *window, std::string & );
  virtual void                  windowTextSet    ( AFW::Core::Window *window, const std::string & );
  virtual void                  windowTextSet    ( AFW::Core::Window *window, const char *, unsigned int length );

protected: 

  // Do not copy.
  FoxComponent ( const FoxComponent & );
  FoxComponent &operator = ( const FoxComponent & );

  // Use reference counting.
  virtual ~FoxComponent();

  void                          _buildChildren ( FX::FXMainWindow *foxMainWin, AFW::Core::MainWindow *mainWin );
  void                          _buildDockedWindow ( AFW::Core::Window * );
  void                          _buildTextWindow ( FX::FXComposite *parent, AFW::Core::TextWindow *text );
  void                          _buildMainWindow ( AFW::Core::MainWindow *mainWin );
  void                          _buildMenuBar ( FX::FXMainWindow *, AFW::Menus::MenuBar * );
  void                          _buildMenuButton ( FX::FXComposite *parent, AFW::Menus::Button *button );
  void                          _buildStatusBar ( FX::FXMainWindow *, AFW::Core::StatusBar * );
  void                          _buildSubMenu ( FX::FXComposite *parent, AFW::Core::Group *group );
  void                          _buildToolBars();
  void                          _buildTopMenu ( FX::FXMainWindow *, FX::FXMenuBar *, AFW::Core::Group * );

  void                          _callUpdateActions ( AFW::Core::Window * );
  void                          _cleanChildren ( AFW::Core::Group *group );
  void                          _cleanup();

  void                          _deleteFoxObject ( FX::FXObject * );
  void                          _deleteGuiObject ( AFW::Core::Window * );
  void                          _detachAllWindows();

  FX::FXComposite *             _dockSiteParent ( AFW::Core::Window *window );

  AFW::Core::Window *           _findWindow ( FX::FXObject * );
  void                          _flush();

  FoxRect                       _initialMainWindowSize ( AFW::Core::MainWindow *mainWin );

  FX::FXComposite *             _makeFrame ( FX::FXSplitter *parent );
  FX::FXComposite *             _makeFrame ( FX::FXComposite *parent, AFW::Core::DockSite::Type type );
  FX::FXIcon *                  _makeIcon ( const AFW::Core::Window * ) const;
  FX::FXIcon *                  _makeIcon ( const std::string & ) const;
  FX::FXComposite *             _makeTabItem ( const AFW::Core::Window *window, FX::FXComposite *parent );
  void                          _makeThreeWaySplit ( unsigned int circle, FX::FXComposite *parent, unsigned int direction, ThreeWaySplit & );
  FX::FXComposite *             _makeShutterItem ( const AFW::Core::Window *window, FX::FXComposite *parent );
  void                          _makeSplitRegions ( unsigned int circle, FX::FXComposite *parent, SplitRegions & );
  std::string                   _makeSplitterRegistrySection ( unsigned int circle, unsigned int direction, const std::string &name ) const;

  void                          _newWindow ( FX::FXWindow *, AFW::Core::Window * );

  void                          _registryWrite();

  void                          _scrollWindowToEnd ( FX::FXText *window );
  void                          _setColor ( FX::FXWindow * ) const;
  void                          _showSplashScreen();

  std::string                   _tabItemText ( const AFW::Core::Window * ) const;

  Usul::Interfaces::IUnknown *  _unknown();

  void                          _updateWindowText ( AFW::Core::Window *window );

  void                          _windowTextAppend ( FX::FXText *, const char *, unsigned int length );
  void                          _windowTextGet    ( const FX::FXText *, std::string & ) const;
  void                          _windowTextSet    ( FX::FXText *, const char *, unsigned int length );

private:

  AFW::Core::Application &_app;
  FoxTools::App::Application *_foxApp;
  WindowsMap _windows;
  DockCircles _dockCircles;
  Splitters _splitters;
  bool _force;
  mutable Mutex _mutex;

  FXDECLARE ( FoxComponent );
};


#endif // _FOX_GUI_SERVER_COMPONENT_CLASS_H_
