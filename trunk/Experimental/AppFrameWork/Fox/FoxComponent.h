
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

#include "CompileGuard.h"

#include "AppFrameWork/Core/Application.h"

#include "FoxTools/Headers/Object.h"
#include "FoxTools/Headers/MenuPane.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Properties/Attribute.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IGUIServer.h"
#include "Usul/Interfaces/INotifyClose.h"
#include "Usul/Interfaces/ILoadFileDialog.h"

#include "boost/shared_ptr.hpp"

#include <string>
#include <map>

namespace FoxTools { namespace App { class Application; } }
namespace FX { class FXObject; class FXWindow; class FXMainWindow; }
namespace FX { class FXMenuBar; class FXToolBarShell; class FXIcon; }
namespace FX { class FXComposite; }
namespace AFW { namespace Menus { class Button; class MenuGroup; class Frame; } }


class FoxComponent : public Usul::Base::Referenced,
                     public FX::FXObject,
                     public Usul::Interfaces::IPlugin,
                     public Usul::Interfaces::IGUIServer,
                     public Usul::Interfaces::INotifyClose,
                     public Usul::Interfaces::ILoadFileDialog
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

  // Notification that the object is being removed from the scene.
  virtual void                  removeNotify ( AFW::Core::Window * );

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
  long                          onClose   ( FX::FXObject *, FX::FXSelector, void * );
  long                          onCommand ( FX::FXObject *, FX::FXSelector, void * );
  long                          onUpdate  ( FX::FXObject *, FX::FXSelector, void * );
  long                          onDestroy ( FX::FXObject *, FX::FXSelector, void * );

  // Run the application.
  virtual void                  runApplication();

  // Update the application.
  virtual void                  updateApplication();

protected: 

  // Do not copy.
  FoxComponent ( const FoxComponent & );
  FoxComponent &operator = ( const FoxComponent & );

  // Use reference counting.
  virtual ~FoxComponent();

  void                          _buildChildren ( AFW::Core::Frame * );
  void                          _buildMainWindow ( AFW::Core::MainWindow *mainWin );
  void                          _buildMenuBar ( FX::FXMainWindow *, AFW::Menus::MenuBar * );
  void                          _buildMenuButton ( FX::FXComposite *parent, AFW::Menus::Button *button );
  void                          _buildSubMenu ( FX::FXComposite *parent, AFW::Core::Group *group );
  void                          _buildToolBars();
  void                          _buildTopMenu ( FX::FXMainWindow *, FX::FXMenuBar *, AFW::Core::Group * );

  void                          _cleanChildren ( AFW::Core::Group *group );
  void                          _cleanup();

  void                          _deleteFoxObject ( FX::FXObject * );
  void                          _deleteGuiObject ( AFW::Core::Window * );
  void                          _detachAllWindows();

  AFW::Core::Window *           _findWindow ( FX::FXObject * );

  FoxRect                       _initialMainWindowSize();

  FX::FXIcon *                  _makeIcon ( AFW::Core::Window * );

  void                          _newWindow ( FX::FXWindow *, AFW::Core::Window * );

  void                          _registryWrite();

private:

  AFW::Core::Application &_app;
  FoxTools::App::Application *_foxApp;
  WindowsMap _windows;

  FXDECLARE ( FoxComponent );
};


#endif // _FOX_GUI_SERVER_COMPONENT_CLASS_H_
