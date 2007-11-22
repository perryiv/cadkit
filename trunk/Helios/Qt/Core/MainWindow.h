
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
#include "Helios/Qt/Core/DocumentProxy.h"
#include "Helios/Qt/Core/Menu.h"
#include "Helios/Qt/Core/ProgressBarDock.h"
#include "Helios/Qt/Commands/BaseAction.h"

#include "Usul/Interfaces/IStreamListener.h"
#include "Usul/Interfaces/GUI/ILoadFileDialog.h"
#include "Usul/Interfaces/GUI/ISaveFileDialog.h"
#include "Usul/Interfaces/GUI/IUpdateTextWindow.h"
#include "Usul/Interfaces/GUI/IGUIDelegateNotify.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Interfaces/IQtDockWidgetMenu.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Queue.h"

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
  public Usul::Interfaces::IActiveViewListener
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
               const std::string &icon,
               const std::string &output,
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

  // Parse the command-line.
  void                              parseCommandLine ( int argc, char **argv );

  // Show/hide the splash screen.
  void                              hideSplashScreen();
  void                              showSplashScreen();

  // Restore dock window positions.
  void                              restoreDockWindows();

  // Text window operations.
  void                              textWindowAppend ( const std::string & );
  virtual void                      updateTextWindow ( bool force );

protected:

  void                              _buildMenuKitMenu();
  void                              _initRecentFilesMenu();
  void                              _buildQtMenu();
  void                              _buildTextWindow();
  void                              _buildProgressBarWindow();
  void                              _buildToolBar();
  void                              _buildPluginDockWidgets();

  void                              _clearDocuments();
  void                              _clearMenuBar ();
  void                              _clearRecentFiles ();
  virtual void                      closeEvent ( QCloseEvent *event );
  virtual void                      _closeEvent ( QCloseEvent* event );

  /// Drag events.
  virtual void                      dragEnterEvent ( QDragEnterEvent *event );
  virtual void                      dropEvent      ( QDropEvent      *event );

  std::string                       _formatFilters ( const Filters &filters ) const;

  std::string                       _lastFileDialogDir ( const std::string &title ) const;
  void                              _lastFileDialogDir ( const std::string &title, const std::string &dir );
  std::string                       _lastFileDialogFilter ( const std::string &title ) const;
  void                              _lastFileDialogFilter ( const std::string &title, const std::string &filter ) const;
  void                              _loadSettings();

  void                              _parseCommandLine ( int argc, char **argv );

  void                              _saveSettings();
 
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
  virtual Unknown*                  createProgressBar();

  // Add dock widget to menu (Usul::Interfaces::IQtDockWidgetMenu).
  virtual void                      addDockWidgetMenu ( QDockWidget * dock );

  /// The active document has changed.
  virtual void                      activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// The active document has changed.
  virtual void                      activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView );

private slots:

  void                              _idleProcess();

  void                              _notifyDocumentFinishedLoading ( DocumentProxy proxy );

  void                              _updateTextWindow();

private:

  typedef std::queue<std::string> StringQueue;
  USUL_DECLARE_QUEUE_CONFIG ( QueueConfig, Mutex, Guard, StringQueue );
  typedef Usul::Threads::Queue<QueueConfig> StreamQueue;
  typedef boost::shared_ptr<StreamQueue> StreamQueuePtr;
  typedef std::pair<QTextEdit*,StreamQueuePtr> TextWindow;
  typedef std::vector < Menu* > Menus;

  // No copying or assignment.
  MainWindow ( const MainWindow & );
  MainWindow &operator = ( const MainWindow & );

  void                              _destroy();

  void                              _notifyFinishedLoading ( Usul::Documents::Document * );

  std::string                       _registryFileName() const;

  mutable Mutex *_mutex;
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
  MenuKit::Menu::RefPtr _dockMenu;
  QTimer *_idleTimer;
  ProgressBarDock::RefPtr _progressBars;
  MenuKit::Menu::RefPtr _menu;
  Menus _menus;
  StringList _recentFiles;
  MenuKit::Menu::RefPtr _recentFilesMenu;
  MenuKit::Menu::RefPtr _newDocumentMenu;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_MAIN_WINDOW_H_
