
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
#include "Helios/Qt/Core/DocumentProxy.h"
#include "Helios/Qt/Core/ProgressBarDock.h"
#include "Helios/Qt/Core/SplashScreen.h"
#include "Helios/Qt/Core/TimerServer.h"

#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IStreamListener.h"
#include "Usul/Interfaces/ILoadFileDialog.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Interfaces/GUI/IUpdateTextWindow.h"
#include "Usul/Interfaces/GUI/IGUIDelegateNotify.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/GUI/IQuestion.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Queue.h"

#include "QtTools/Menu.h"
#include "QtTools/Action.h"

#include "boost/shared_ptr.hpp"

#include <QtGui/QMainWindow>

#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

class QWorkspace;
class QTextEdit;
class QMenu;
class QStringList;
class QTimer;

namespace Usul { namespace Threads { class Thread; } }


namespace CadKit {
namespace Helios {
namespace Core {


class HELIOS_QT_CORE_EXPORT MainWindow : 
  public QMainWindow,
  public Usul::Interfaces::ILoadFileDialog,
  public Usul::Interfaces::ISaveFileDialog,
  public Usul::Interfaces::IUpdateTextWindow,
  public Usul::Interfaces::Qt::IMainWindow,
  public Usul::Interfaces::Qt::IWorkspace,
  public Usul::Interfaces::IGUIDelegateNotify,
  public Usul::Interfaces::IStreamListenerChar,
  public Usul::Interfaces::IProgressBarFactory,
  public Usul::Interfaces::IQtDockWidgetMenu,
  public Usul::Interfaces::IActiveDocumentListener,
  public Usul::Interfaces::IActiveViewListener,
  public Usul::Interfaces::IQuestion
{
  Q_OBJECT

public:

  // Useful typedefs.
  typedef QMainWindow                           BaseClass;
  typedef Usul::Threads::RecursiveMutex         Mutex;
  typedef Usul::Threads::Guard<Mutex>           Guard;
  typedef QtTools::Action                       Action;
  typedef std::set<Action::RefPtr>              Actions;
  typedef std::map<std::string,QToolBar*>       ToolBars;
  typedef Usul::Interfaces::ILoadFileDialog     ILoadFileDialog;
  typedef ILoadFileDialog::FileResult           FileResult;
  typedef ILoadFileDialog::FilesResult          FilesResult;
  typedef ILoadFileDialog::Filters              Filters;
  typedef std::vector<std::string>              PluginFiles;
  typedef Usul::Interfaces::IUnknown            Unknown;
  typedef std::list<std::string>                StringList;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MainWindow );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor and destructor.
  MainWindow ( const std::string &vendor, 
               const std::string &url, 
               const std::string &program, 
               const std::string &manual, 
               const std::string &icon,
               const std::string &output,
               const std::string& about,
               bool showSplash = true );
  virtual ~MainWindow();

  // Get the name of the file to load from
  virtual FileResult                getLoadFileName  ( const std::string &title = "Load", const Filters &filters = Filters() );
  virtual FilesResult               getLoadFileNames ( const std::string &title = "Load", const Filters &filters = Filters() );

  // Get the name of the file to save to.
  virtual FileResult                getSaveFileName  ( const std::string &title = "Save", const Filters &filters = Filters() );

  // Get the mutex.
  Mutex &                           mutex() const { return *_mutex; }

  // Functions for adding, clearing, etc., the plugin configuration files.
  void                              addPluginFile ( const std::string & );
  void                              clearPluginFiles();
  std::string                       defautPluginFile() const;
  PluginFiles                       pluginFiles() const;

  // Functions for loading, releasing, etc., the plugins.
  void                              loadPlugins();
  void                              loadPlugins ( const std::string &configFile );
  void                              loadPlugin  ( const std::string &pluginFile );
  void                              printPlugins() const;
  void                              releasePlugins();
  void                              releasePlugin ( const std::string &pluginFile );
  void                              initPlugins ();

  // Load the file.
  void                              loadFile ( const std::string &file );

  // Functions for getting information about this binary.
  std::string                       directory() const;
  std::string                       icon() const;
  std::string                       programFile() const;
  std::string                       programName() const;
  std::string                       vendor() const;
  std::string                       url() const;
  std::string                       about() const;

  // Parse the command-line.
  void                              parseCommandLine ( int argc, char **argv );

  // Show/hide the splash screen.
  void                              hideSplashScreen();
  void                              showSplashScreen();

  // Get the name of the settings file.
  std::string                       settingsFileName() const;

  // Restore dock window positions.
  void                              restoreDockWindows();

  // Text window operations.
  void                              textWindowAppend ( const std::string & );
  virtual void                      updateTextWindow ( bool force );

protected:

  void                              _appendMacro ( MenuKit::Menu::RefPtr menu, const std::string &file );

  void                              _buildMenuKitMenu();
  void                              _initRecentFilesMenu();
  void                              _buildQtMenu();
  void                              _buildTextWindow();
  void                              _buildProgressBarWindow();
  void                              _buildToolBar();
  void                              _buildQtToolBar( const MenuKit::Menu& toolBar );
  
  void                              _childWindowsCascade();
  void                              _childWindowsTile();
  void                              _childWindowsCloseActive();
  void                              _childWindowsCloseAll();
  bool                              _childWindowsHas() const;

  void                              _clearDocuments();
  void                              _clearMenuBar();
  void                              _clearRecentFiles();
	void                              _clearToolBar();
  virtual void                      closeEvent ( QCloseEvent *event );
  virtual void                      _closeEvent ( QCloseEvent* event );

  /// Drag events.
  virtual void                      dragEnterEvent ( QDragEnterEvent *event );
  virtual void                      dropEvent      ( QDropEvent      *event );

  void                              _loadSettings();

  void                              _parseCommandLine ( int argc, char **argv );

  void                              _saveSettings();
  
  static void                       _waitForJobs();
 
  // Usul::Interfaces::Qt::IMainWindow
  virtual QMainWindow *             mainWindow();
  virtual const QMainWindow*        mainWindow() const;

  // Usul::Interfaces::Qt::IWorkspace
  virtual QWorkspace *              workspace();
  virtual const QWorkspace*         workspace() const;

  // Usul::Interfaces::IGUIDelegateNotify
  virtual void                      notifyDocumentFinishedLoading ( Usul::Documents::Document* document );

  // Usul::Interfaces::IStreamListenerChar
  virtual void                      notify ( Usul::Interfaces::IUnknown *caller, const char *values, unsigned int numValues );

  // Create a progress bar (Usul::Interfaces::IProgressBarFactory).
  virtual Unknown::RefPtr           createProgressBar ( bool waitIfNotGuiThread = false );

  // Add dock widget to menu (Usul::Interfaces::IQtDockWidgetMenu).
  virtual void                      addDockWidgetMenu ( QDockWidget * dock );

  /// The active document has changed.
  virtual void                      activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// The active document has changed.
  virtual void                      activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView );

  /// Prompt the user (IQuestion).
  virtual std::string               question ( const std::string &buttons, const std::string &title, const std::string &text );
  
private slots:

  void                              _idleProcess();

  void                              _notifyDocumentFinishedLoading ( DocumentProxy proxy );

private:

  typedef std::queue<std::string> StringQueue;
  USUL_DECLARE_QUEUE_CONFIG ( QueueConfig, Mutex, Guard, StringQueue );
  typedef Usul::Threads::Queue<QueueConfig> StreamQueue;
  typedef boost::shared_ptr<StreamQueue> StreamQueuePtr;
  typedef std::pair<QTextEdit*,StreamQueuePtr> TextWindow;
  typedef std::vector < QtTools::Menu* > Menus;

  // No copying or assignment.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  void                              _destroy();

  void                              _notifyFinishedLoading ( Usul::Documents::Document * );

  mutable Mutex *_mutex;
  Actions _toolBarActions;
  ToolBars _toolBars;
  unsigned long _refCount;
  PluginFiles _pluginFiles;
  std::string _vendor;
  std::string _url;
  std::string _program;
  std::string _manual;
  std::string _icon;
  std::string _output;
  std::string _about;
  SplashScreen::RefPtr _splash;
  QWorkspace *_workSpace;
  TextWindow _textWindow;
  MenuKit::Menu::RefPtr _dockMenu;
  QTimer *_idleTimer;
  ProgressBarDock::RefPtr _progressBars;
  MenuKit::Menu::RefPtr _menu;
  Menus _menus;
  StringList _recentFiles;
  MenuKit::Menu::RefPtr _recentFilesMenu;
  MenuKit::Menu::RefPtr _newDocumentMenu;
  TimerServer::RefPtr _timerServer;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_
