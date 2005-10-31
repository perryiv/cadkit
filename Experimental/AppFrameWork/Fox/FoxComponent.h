
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

#include "boost/shared_ptr.hpp"

#include <string>
#include <map>
#include <list>

namespace FoxTools { namespace App { class Application; } }
namespace FX { class FXObject; class FXWindow; class FXMainWindow; class FXMenuBar; class FXToolBarShell; class FXIcon; }
namespace AFW { namespace Menus { class Button; class MenuGroup; } }


class FoxComponent : public Usul::Base::Referenced,
                     public FX::FXObject,
                     public Usul::Interfaces::IPlugin,
                     public Usul::Interfaces::IGUIServer,
                     public Usul::Interfaces::INotifyClose
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vector4 < FX::FXint > FoxRect;
  typedef boost::shared_ptr < FX::FXMenuPane > MenuPanePtr;
  typedef std::pair < FX::FXWindow *, FX::FXWindow * > WindowMapKey;
  typedef std::list < MenuPanePtr > MenuPaneList;
  typedef std::map < WindowMapKey, MenuPaneList > MenuPaneMap;
  typedef std::map < FX::FXObject *, AFW::Core::Window::RefPtr > WindowsMap;
  typedef Usul::Properties::Attribute < FX::FXObject * > FoxObjectWrapper;

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

  // Enable/disable the window.
  virtual void                  enableWindow ( bool state, AFW::Core::Window *window, Usul::Base::Referenced *data );

  // Return name of plugin
  virtual std::string           getPluginName() const { return "FOX GUI Server"; }

  // Return the enabled/disabled state.
  virtual bool                  isWindowEnabled ( const AFW::Core::Window *window, Usul::Base::Referenced *data );

  // Notify the component.
  virtual bool                  notifyClose ( Usul::Interfaces::IUnknown *caller );

  // FOX message callback.
  long                          onCommandClose  ( FX::FXObject *, FX::FXSelector, void * );
  long                          onCommandButton ( FX::FXObject *, FX::FXSelector, void * );
  long                          onUpdateButton  ( FX::FXObject *, FX::FXSelector, void * );
  long                          onDestroyButton ( FX::FXObject *, FX::FXSelector, void * );

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

  void                          _buildMainWindow();
  void                          _buildMenuBar();
  void                          _buildMenuButton ( FX::FXMenuPane *pane, AFW::Menus::Button * );
  void                          _buildSubMenu    ( FX::FXMenuPane *pane, AFW::Core::Group * );
  void                          _buildTopMenu    ( AFW::Core::Group * );

  void                          _cleanTopMenus();

  FoxRect                       _initialMainWindowSize();

  FX::FXIcon *                  _makeIcon ( AFW::Core::Window * );

  void                          _registryWrite();

private:

  AFW::Core::Application &_app;
  FoxTools::App::Application *_foxApp;
  FX::FXMainWindow *_mainWin;
  FX::FXMenuBar *_menuBar;
  MenuPaneMap _menuPanes;
  WindowsMap _windows;

  FXDECLARE ( FoxComponent );
};


#endif // _FOX_GUI_SERVER_COMPONENT_CLASS_H_
