
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
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IGUIServer.h"
#include "Usul/Interfaces/INotifyClose.h"

#include "boost/shared_ptr.hpp"

#include <string>
#include <map>
#include <list>

namespace FoxTools { namespace App { class Application; } }
namespace FX { class FXObject; class FXWindow; class FXMainWindow; class FXMenuBar; class FXToolBarShell; }
namespace AFW { namespace Menus { class MenuButton; class MenuGroup; class MenuItem; } }


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
  typedef std::pair < FX::FXMainWindow *, FX::FXMenuBar * > MenuBarPaneKey;
  typedef std::list < MenuPanePtr > MenuPaneList;
  typedef std::map < MenuBarPaneKey, MenuPaneList > MenuPaneMap;

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

  // Return name of plugin
  virtual std::string           getPluginName() const { return "FOX GUI Server"; }

  // FOX message callback.
  long                          onCommandClose ( FX::FXObject *, FX::FXSelector, void * );

  // Notify the component.
  virtual bool                  notifyClose ( Usul::Interfaces::IUnknown *caller );

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
  void                          _buildMenuButton ( FX::FXMenuPane *pane, AFW::Menus::MenuButton * );
  void                          _buildSubMenu    ( FX::FXMenuPane *pane, AFW::Menus::MenuGroup * );
  void                          _buildTopMenu    ( AFW::Menus::MenuGroup * );

  void                          _cleanTopMenus();

  std::string                   _formatItemText ( AFW::Menus::MenuItem *item ) const;

  FoxRect                       _initialMainWindowSize();

  void                          _registryWrite();

private:

  AFW::Core::Application &_app;
  FoxTools::App::Application *_foxApp;
  FX::FXMainWindow *_mainWin;
  FX::FXMenuBar *_menuBar;
  MenuPaneMap _menuPanes;

  FXDECLARE ( FoxComponent );
};


#endif // _FOX_GUI_SERVER_COMPONENT_CLASS_H_
