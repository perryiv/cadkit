
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_
#define _CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_

#include "Helios/Qt/Core/Export.h"
#include "Helios/Qt/Core/SplashScreen.h"
#include "Helios/Qt/Commands/BaseAction.h"

#include "Usul/Interfaces/GUI/ILoadFileDialog.h"
#include "Usul/Interfaces/GUI/IUpdateTextWindow.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"

#include "QtCore/QSettings"
#include "QtGui/QMainWindow"

#include <string>
#include <map>
#include <set>
#include <vector>

class QWorkspace;
class QTextEdit;
class QMenu;
class QTimer;

namespace Usul { namespace Threads { class Thread; } }


namespace CadKit {
namespace Helios {
namespace Core {


class HELIOS_QT_CORE_EXPORT MainWindow : 
  public QMainWindow,
  public Usul::Interfaces::ILoadFileDialog,
  public Usul::Interfaces::IUpdateTextWindow
{
  Q_OBJECT

public:

  // Useful typedefs.
  typedef QMainWindow                           BaseClass;
  typedef Usul::Threads::RecursiveMutex         Mutex;
  typedef Usul::Threads::Guard<Mutex>           Guard;
  typedef CadKit::Helios::Commands::BaseAction  BaseAction;
  typedef std::set<BaseAction::RefPtr>          Actions;
  typedef std::map<std::string,QToolBar*>       ToolBars;
  typedef Usul::Interfaces::ILoadFileDialog     ILoadFileDialog;
  typedef ILoadFileDialog::FileResult           FileResult;
  typedef ILoadFileDialog::FilesResult          FilesResult;
  typedef ILoadFileDialog::Filters              Filters;
  typedef std::vector<std::string>              PluginFiles;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindow );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor and destructor.
  MainWindow ( const std::string &vendor, 
               const std::string &url, 
               const std::string &program, 
               const std::string &icon,
               const std::string &output,
               bool showSplash = true );
  virtual ~MainWindow();

  // Get the name of the file to load from
  virtual FileResult        getLoadFileName  ( const std::string &title = "Load", const Filters &filters = Filters() );
  virtual FilesResult       getLoadFileNames ( const std::string &title = "Load", const Filters &filters = Filters() );

  // Get the mutex.
  Mutex &                   mutex() const { return *_mutex; }

  // Functions for adding, clearing, etc., the plugin configuration files.
  void                      addPluginFile ( const std::string & );
  void                      clearPluginFiles();
  std::string               defautPluginFile() const;
  PluginFiles               pluginFiles() const;

  // Functions for loading, releasing, etc., the plugins.
  void                      loadPlugins();
  void                      loadPlugins ( const std::string &configFile );
  void                      loadPlugin  ( const std::string &pluginFile );
  void                      printPlugins() const;
  void                      releasePlugins();
  void                      releasePlugin ( const std::string &pluginFile );

  // Functions for getting information about this binary.
  std::string               directory() const;
  std::string               icon() const;
  std::string               programFile() const;
  std::string               programName() const;
  std::string               vendor() const;
  std::string               url() const;

  // Get the settings.
  const QSettings &         settings() const;
  QSettings &               settings();

  // Show/hide the splash screen.
  void                      hideSplashScreen();
  void                      showSplashScreen();

  // Update these sub-windows.
  virtual void              updateTextWindow ( bool force );
  
protected:

  void                      _buildMenu();
  void                      _buildTextWindow();
  void                      _buildToolBar();

  void                      _loadSettings();

  void                      _saveSettings();
  
private slots:

  void                      _forceUpdateTextWindow();

  void                      _idleProcess();

private:

  typedef std::pair<QTextEdit*,unsigned int> TextWindow;

  // No copying or assignment.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  void                      _destroy();

  mutable Mutex *_mutex;
  QSettings _settings;
  Actions _actions;
  ToolBars _toolBars;
  unsigned long _refCount;
  PluginFiles _pluginFiles;
  std::string _vendor;
  std::string _url;
  std::string _program;
  std::string _icon;
  std::string _output;
  SplashScreen::RefPtr _splash;
  QWorkspace *_workSpace;
  TextWindow _textWindow;
  QMenu *_dockMenu;
  QTimer *_idleTimer;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_
