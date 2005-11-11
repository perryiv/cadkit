
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_APPLICATION_CLASS_H_
#define _APP_FRAME_WORK_APPLICATION_CLASS_H_

#include "AppFrameWork/Core/Export.h"
#include "AppFrameWork/Core/MainWindow.h"

#include "Usul/Interfaces/IGUIServer.h"

#include <set>
#include <string>

namespace AFW { namespace Core { class BaseVisitor; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Application
{
public:

  // Typedefs.
  typedef std::set < std::string > PluginExtensions;
  typedef Usul::Interfaces::IGUIServer IGUIServer;

  // Possible application types.
  enum Type
  {
    MULTIPLE_DOCUMENT_INTERFACE,
    SINGLE_DOCUMENT_INTERFACE
  };

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Build a default GUI.
  bool                                buildDefault();

  // Clean up all that we can.
  bool                                cleanup();

  // Notification that the window is being destroyed.
  void                                destroyNotify ( Window * );

  // Notification that the window is being removed from the scene.
  void                                removeNotify ( Window * );

  // Set/get the dirty flag.
  void                                dirty ( bool );
  bool                                dirty() const;

  // Enable/disable the window.
  void                                enableWindow ( bool state, Window *window );

  // Set/get the GUI factory.
  void                                guiServer ( IGUIServer *gui ) { _gui = gui; }
  IGUIServer *                        guiServer() { return _gui.get(); }

  // It's a singleton.
  static Application &                instance();

  // Return the enabled/disabled state.
  bool                                isWindowEnabled ( const Window *window );

  // Set/get the main window.
  const MainWindow *                  mainWindow() const { return _mainWindow.get(); }
  MainWindow *                        mainWindow()       { return _mainWindow.get(); }
  void                                mainWindow ( MainWindow *mw ) { _mainWindow = mw; this->dirty ( true ); }

  // Set/get the name of the application.
  const std::string &                 name() const { return _name; }
  void                                name ( const std::string &n ) { _name = n; }

  // Set/get the plugin extensions.
  PluginExtensions                    pluginExtensions() const { return _plugExts; }
  void                                pluginExtensions ( const PluginExtensions &p ) { _plugExts = p; }

  // Set/get the name of the file that stdio and stderr are redirected to.
  const std::string &                 redirect() const { return _redirect; }
  void                                redirect ( const std::string &n ) { _redirect = n; }

  // Run the application.
  bool                                run();

  // Set/get the type of the application.
  Type                                type() const { return _type; }
  void                                type ( const Type &t ) { _type = t; }

  // Set/get the name of the vendor.
  const std::string &                 vendor() const { return _vendor; }
  void                                vendor ( const std::string &v ) { _vendor = v; }

  // Update the text of the window's gui-object.
  void                                windowTextAppend ( Window *window, const std::string & );
  void                                windowTextAppend ( Window *window, const char *, unsigned int length );
  void                                windowTextGet    ( const Window *window, std::string & );
  void                                windowTextSet    ( Window *window, const std::string & );
  void                                windowTextSet    ( Window *window, const char *, unsigned int length );

protected:

  // Constructor, destructor.
  Application();
  virtual ~Application();

  void                                _buildDefault();

  void                                _cleanup();

  void                                _initGuiFactory();

  void                                _loadPlugins();

  void                                _notifyClose();

  void                                _run();

  void                                _registryRead();
  void                                _registryWrite();

private:

  // No copying.
  Application ( const Application & );
  Application &operator = ( const Application & );

  static Application *_instance;
  std::string _name;
  std::string _redirect;
  std::string _vendor;
  PluginExtensions _plugExts;
  Type _type;
  MainWindow::RefPtr _mainWindow;
  IGUIServer::QueryPtr _gui;
  AFW::Core::State::Type _flags;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_APPLICATION_CLASS_H_
