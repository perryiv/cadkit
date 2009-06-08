
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

#include "Helios/Qt/Core/MainWindow.h"
#include "Helios/Qt/Core/Constants.h"
#include "Helios/Qt/Commands/NewDocument.h"
#include "Helios/Qt/Commands/OpenDocument.h"
#include "Helios/Qt/Commands/InsertDocument.h"
#include "Helios/Qt/Commands/SaveDocument.h"
#include "Helios/Qt/Commands/SaveAsDocument.h"
#include "Helios/Qt/Commands/ExportImage.h"
#include "Helios/Qt/Commands/ExportModel.h"
#include "Helios/Qt/Commands/ExportScene.h"
#include "Helios/Qt/Commands/ExitApplication.h"
#include "Helios/Qt/Commands/ToggleView.h"
#include "Helios/Qt/Commands/OpenManual.h"
#include "Helios/Qt/Commands/About.h"
#include "Helios/Qt/Commands/Group.h"
#include "Helios/Qt/Commands/WorkOffline.h"

#include "XmlTree/Document.h"
#include "XmlTree/RegistryIO.h"

#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/CommandVisitor.h"
#include "MenuKit/MenuCommands.h"

#include "QtTools/FileDialog.h"
#include "QtTools/Image.h"
#include "QtTools/Question.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Components/Loader.h"
#include "Usul/Config/Config.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Find.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Interfaces/IBusyState.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IToolBarAdd.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Qt.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Qt.h"
#include "Usul/System/Environment.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"
#include "Usul/User/Directory.h"

#include "QtCore/QStringList"
#include "QtCore/QTimer"
#include "QtGui/QApplication"
#include "QtGui/QDockWidget"
#include "QtGui/QLabel"
#include "QtGui/QMenuBar"
#include "QtGui/QProgressDialog"
#include "QtGui/QPushButton"
#include "QtGui/QStatusBar"
#include "QtGui/QToolBar"
#include "QtGui/QTextEdit"
#include "QtGui/QVBoxLayout"
#include "QtGui/QWorkspace"
#include "QtGui/QDragEnterEvent"
#include "QtGui/QDropEvent"
#include "QtCore/QMetaType"
#include "QtCore/QUrl"

#include <algorithm>
#include <fstream>

using namespace CadKit::Helios::Core;
namespace Sections = CadKit::Helios::Core::Registry::Sections;
namespace Keys = CadKit::Helios::Core::Registry::Keys;
namespace Defaults = CadKit::Helios::Core::Registry::Defaults;
typedef Usul::Registry::Database Reg;

// Needed for invoke method.
Q_DECLARE_METATYPE( DocumentProxy );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow ( const std::string &vendor, 
                         const std::string &url, 
                         const std::string &program,
                         const std::string &manual,
                         const std::string &icon,
                         const std::string &output,
                         const std::string& about,
                         bool showSplash ) : BaseClass(),
  _mutex        ( new MainWindow::Mutex ),
  _toolBarActions (),
  _toolBars     (),
  _refCount     ( 0 ),
  _pluginFiles  (),
  _vendor       ( vendor ),
  _url          ( url ),
  _program      ( program ),
  _manual       ( manual ),
  _icon         ( icon ),
  _output       ( output ),
  _about        ( about ),
  _splash       ( 0x0 ),
  _workSpace    ( 0x0 ),
  _textWindow   ( 0x0, StreamQueuePtr ( new StreamQueue() ) ),
  _dockMenu     ( 0x0 ),
  _idleTimer    ( 0x0 ),
  _progressBars ( new ProgressBarDock ),
  _menu         ( 0x0 ),
  _menus        (),
  _recentFiles  (),
  _recentFilesMenu ( 0x0 ),
  _newDocumentMenu ( new MenuKit::Menu ( "New" ) ),
  _timerServer  ( new TimerServer )
{
  USUL_TRACE_SCOPE;

  // Name this thread.
  Usul::Threads::Named::instance().set ( Usul::Threads::Names::GUI );

  // Keep names the same.
  Usul::App::Application::instance().program ( _program );

  // Program-wide settings.
  QCoreApplication::setOrganizationName ( vendor.c_str() );
  QCoreApplication::setOrganizationDomain ( url.c_str() );
  QCoreApplication::setApplicationName ( program.c_str() );

  // Show the splash screen if we should.
  if ( true == showSplash )
    this->showSplashScreen();

  // Set the icon.
  QtTools::Image::icon ( icon, this );

  // Make the work space.
  _workSpace = new QWorkspace ( this );
  this->setCentralWidget ( _workSpace );

  // Enable toolbar docking.
  this->setEnabled ( true );

  // Create the menu for the docking windows.
  _dockMenu = new MenuKit::Menu;
  _dockMenu->text ( "Docking Windows" );

  // Build the text window.
  this->_buildTextWindow();

  // Build the progress bar window.
  this->_buildProgressBarWindow();

  // Set the text window resource.
  Usul::Resources::textWindow ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

  // Make sure we can size the status bar.
  this->statusBar()->setSizeGripEnabled ( true );
  this->statusBar()->showMessage ( tr ( "Ready" ) );

  // Load the settings.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MainWindow::_loadSettings ), "9740089350" );

  // Make the menu and toolbar.
  this->_buildMenuKitMenu();
  this->_buildQtMenu();
  this->_buildToolBar();

  // Set the title.
  this->setWindowTitle ( program.c_str() );

  // Start the idle timer.
  _idleTimer = new QTimer ( this );
  QObject::connect ( _idleTimer, SIGNAL ( timeout() ), SLOT ( _idleProcess() ) );
  _idleTimer->start ( 1000 ); // Once every second.

  this->setAcceptDrops ( true );

  // Add this as an active view listener.
  Usul::Documents::Manager::instance().addActiveViewListener ( this );

  // Add this as an active document listener.
  Usul::Documents::Manager::instance().addActiveDocumentListener ( this );

  // Register DocumentProxy for Usul::Documents::Document.
  qRegisterMetaType<DocumentProxy>();

  // Add our self as a plugin.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  Usul::Components::Manager::instance().addPlugin ( me );
  
  // Add the timer server.
  Usul::Components::Manager::instance().addPlugin ( Usul::Interfaces::IUnknown::QueryPtr ( _timerServer ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MainWindow::_clearDocuments ), "4010634300" );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MainWindow::_saveSettings ),   "1772821423" );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MainWindow::_destroy ),        "1934297230" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_destroy()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2933090027" );

  // Stop the idle timer.
  if ( 0x0 != _idleTimer )
    _idleTimer->stop();

  // Stop all the timers.
  if ( true == _timerServer.valid() )
  {
    _timerServer->clear();
    _timerServer = 0x0;
  }

  // Remove this as an active view listener.
  Usul::Documents::Manager::instance().removeActiveViewListener ( this );

  // Remove this as an active document listener.
  Usul::Documents::Manager::instance().removeActiveDocumentListener ( this );

  // Clear the factory before releasing the plugins (some of them may 
  // populate it but not clean up).
  Usul::Factory::ObjectFactory::instance().clear();
  
  // Clear the map of commands. This has to come before we release the plugins 
  // because the NewDocument command hangs on to a component.
  MenuKit::MenuCommands::instance().clear();

  // Clear the menu bar. Do this before plugins are released.
  this->_clearMenuBar();
  _menu = 0x0;
  _dockMenu = 0x0;
  _recentFilesMenu = 0x0;
  _newDocumentMenu = 0x0;

	// Clear the tool bar.
	this->_clearToolBar();

  // Clear the progress bars without blocking.
  {
    ProgressBarDock::RefPtr bars ( 0x0 );
    {
      Guard guard ( this );
      bars = _progressBars;
      _progressBars = 0x0;
    }
  }

  // Wait for jobs before plugins are released.
  // At least on OS X, using a pointer created in a shared object after it is released causes a crash.
  MainWindow::_waitForJobs();
  
  // Release all the plugins.
  this->releasePlugins();
  
  // Clear all loaded libraries.
  Usul::DLL::LibraryPool::instance().clear ( &std::cout );

  // Wait here until all jobs are done.
  // It is unlikely that a plugin will create a job in its destructor, but leaving this here to make sure.
  MainWindow::_waitForJobs();
  Usul::Jobs::Manager::destroy();

  // Wait here until all threads are done.
  std::cout << "Purging all finished threads..." << std::endl;
  Usul::Threads::Manager::instance().purge();
  std::cout << "Canceling all remaining threads..." << std::endl;
  Usul::Threads::Manager::instance().cancel();
  std::cout << "Waiting for all threads to finish..." << std::endl;
  Usul::Threads::Manager::instance().wait();
  std::cout << "Destroying thread manager..." << std::endl;
  Usul::Threads::Manager::destroy();
  std::cout << "All threads have finished" << std::endl;

  // Unset the text window resource.
  Usul::Resources::textWindow ( 0x0 );

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );

  // Delete remaining members.
  _pluginFiles.clear();
  _vendor.clear();
  _url.clear();
  _program.clear();
  _icon.clear();
  _output.clear();
  _splash = 0x0;
  _workSpace = 0x0;
  _textWindow.first = 0x0;
  _textWindow.second.reset();
  _dockMenu = 0x0;
  _idleTimer = 0x0;

  // Delete the mutex last.
  delete _mutex; _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all jobs.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_waitForJobs()
{
  std::cout << "Waiting for all jobs to finish..." << std::endl;
  Usul::Jobs::Manager::instance().cancel();
  Usul::Jobs::Manager::instance().wait();
  std::cout << "All jobs have finished" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the settings.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_loadSettings()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2863006339" );

  // Read the file and populate the registry. Not a big deal if this fails.
  const std::string settingsFile ( this->settingsFileName() );
  Usul::Functions::safeCallR1R2 ( XmlTree::RegistryIO::read, settingsFile, Usul::Registry::Database::instance(), "1123442106" );

  // Set the window's properties.
  Usul::Registry::Node &mw ( Reg::instance()[Sections::MAIN_WINDOW] );
  this->restoreGeometry ( mw[Keys::GEOMETRY].get<QByteArray> ( QByteArray() ) );

  // Get recent-files list and remove files that do not exist.
  _recentFiles = mw[Keys::RECENT_FILES]["files"].get<StringList> ( StringList() );
  _recentFiles.remove_if ( std::not1 ( Usul::Predicates::FileExists() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the settings.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_saveSettings()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "9722347090" );

  // Set the window's properties.
  Usul::Registry::Node &mw ( Reg::instance()[Sections::MAIN_WINDOW] );
  mw[Keys::GEOMETRY] = this->saveGeometry();

  // Save dock window positions.
  mw[Keys::DOCK_WINDOW_POSITIONS] = this->saveState ( Helios::Core::Constants::VERSION );

  // Save recent files.
  mw[Keys::RECENT_FILES]["files"] = _recentFiles;

  // Write to disk.
  const std::string settingsFile ( this->settingsFileName() );
  Usul::Functions::safeCallR1R2 ( XmlTree::RegistryIO::write, settingsFile, Usul::Registry::Database::instance(), "4136994389" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildMenuKitMenu()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2357339900" );

  // Handle repeated calls.
  _menu = new MenuKit::Menu;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add file menu.
  {  
    MenuKit::Menu::RefPtr menu ( new MenuKit::Menu ("&File" ) );
    menu->append ( _newDocumentMenu );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::OpenDocument ( me ) ) );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::InsertDocument ( me ) ) );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::SaveDocument ( me ) ) );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::SaveAsDocument ( me ) ) );
    menu->addSeparator();
    MenuKit::Menu::RefPtr exportMenu ( new MenuKit::Menu ( "&Export" ) );
    exportMenu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::ExportImage ( me ) ) );
    exportMenu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::ExportModel ( me ) ) );
    exportMenu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::ExportScene ( me ) ) );
    menu->append ( exportMenu );
    menu->addSeparator();

    // Make the recent files menu.
    this->_initRecentFilesMenu();

    // Add the recent file menu.
    menu->append ( _recentFilesMenu );

		menu->addSeparator();
		menu->append ( new MenuKit::ToggleButton ( new CadKit::Helios::Commands::WorkOffline ) );

    // On mac Qt automatically adds a quit item in the application menu.
#ifndef __APPLE__
    menu->addSeparator();
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::ExitApplication ( me ) ) );
#endif
    
    _menu->append ( menu );
  }

  // Add edit menu.
  {
  }

  // Add view menu.
  {
    {
      MenuKit::Menu::RefPtr view ( new MenuKit::Menu ( "&View" ) );
      view->append ( _dockMenu );
      _menu->append ( view );
    }
  }

  // Check the active document.
  {
    Usul::Interfaces::IMenuAdd::QueryPtr ma ( Usul::Documents::Manager::instance().activeDocument() );
    if ( ma.valid() )
    {
      ma->menuAdd ( *_menu, me.get() );
    }
  }

  // Check the active view.
  {
    Usul::Interfaces::IMenuAdd::QueryPtr ma ( Usul::Documents::Manager::instance().activeView() );
    if ( ma.valid() )
    {
      ma->menuAdd ( *_menu, me.get() );
    }
  }

  // Check all plugins.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet Unknowns;

  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IMenuAdd::IID ) );
  for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++ iter )
  {
    // Should be true.
    Usul::Interfaces::IMenuAdd::ValidQueryPtr ma ( (*iter).get() );
    ma->menuAdd ( *_menu, me.get() );
  }

  // Build map of commands because the next thing we do is make the macros.
  // Note: the help menu will not be included.
  {
    // Clear the commands we have.
    MenuKit::MenuCommands::instance().clear();

    // Traverse the menu and build the map of commands
    MenuKit::CommandVisitor::RefPtr commandVisitor ( new MenuKit::CommandVisitor ( "&" ) );
    _menu->accept ( *commandVisitor );
  }

  // Macro menu.
  {
    try
    {
      // Get macro directory.
      const std::string dir ( Usul::User::Directory::vendor ( this->vendor(), true ) + this->programName() + "/macros" );

      // Get all macro files.
      typedef std::list<std::string> Names;
      Names names;
      Usul::File::find ( dir, "macro", names );

      // Make the menu.
      MenuKit::Menu::RefPtr macros ( new MenuKit::Menu ( "&Macros" ) );

      // Loop through the names.
      for ( Names::const_iterator i = names.begin(); i != names.end(); ++i )
      {
        const std::string base ( *i );
        const std::string path ( Usul::Strings::format ( dir, '/', base ) );
        this->_appendMacro ( macros, path );
      }

      // Add the menu if it's not empty.
      if ( false == macros->empty() )
      {
        _menu->append ( macros );
      }
    }
    USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1878677719" );
  }
  
  // Window menu.
  {
    MenuKit::Menu::RefPtr window ( new MenuKit::Menu ( "&Window" ) );
    window->append ( new MenuKit::Button ( USUL_MAKE_COMMAND_ENABLE ( "&Cascade", "", this, &MainWindow::_childWindowsCascade, &MainWindow::_childWindowsHas        ) ) );
    window->append ( new MenuKit::Button ( USUL_MAKE_COMMAND_ENABLE ( "&Tile",    "", this, &MainWindow::_childWindowsTile,    &MainWindow::_childWindowsHas        ) ) );
    window->append ( new MenuKit::Button ( USUL_MAKE_COMMAND_ENABLE ( "Close Active Window", "", this, &MainWindow::_childWindowsCloseActive, &MainWindow::_childWindowsHas ) ) );
    window->append ( new MenuKit::Button ( USUL_MAKE_COMMAND_ENABLE ( "Close All Windows",   "", this, &MainWindow::_childWindowsCloseAll,    &MainWindow::_childWindowsHas ) ) );
    _menu->append ( window );
  }

  // Add help menu.
  {
    MenuKit::Menu::RefPtr help ( new MenuKit::Menu ( "&Help" ) );
    help->append ( new MenuKit::Button ( new CadKit::Helios::Commands::OpenManual ( me, _manual ) ) );
    help->addSeparator();
    help->append ( new MenuKit::Button ( new CadKit::Helios::Commands::About ( this, Usul::Strings::format ( "About ", this->programName() ), this->about() ) ) );
    _menu->append ( help );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the macro.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_appendMacro ( MenuKit::Menu::RefPtr menu, const std::string &file )
{
  USUL_TRACE_SCOPE;

  if ( false == menu.valid() )
    return;

  // Make a command group.
  typedef CadKit::Helios::Commands::Group CommandGroup;
  CommandGroup::RefPtr group ( new CommandGroup ( Usul::Interfaces::IUnknown::QueryPtr ( this ) ) );

  // Read the macro.
  group->readMacro ( file );

  // Add a button for the command-group if it's not empty.
  if ( false == group->empty() )
  {
    menu->append ( new MenuKit::Button ( group ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildQtMenu()
{
  USUL_TRACE_SCOPE;

  // Handle repeated calls.
  this->_clearMenuBar();

  // Handle bad state.
  if ( 0x0 == _menu )
    return;

  // Build the qt menu.
  for ( MenuKit::Menu::iterator iter = _menu->begin(); iter != _menu->end(); ++iter )
  {
    // We only have top level items that are menus.
    if ( MenuKit::Menu *menu = dynamic_cast < MenuKit::Menu* > ( iter->get() ) )
    {
      QtTools::Menu* qtMenu ( new QtTools::Menu ( tr ( menu->text().c_str() ) ) );
      qtMenu->menu ( menu );
      this->menuBar()->addMenu ( qtMenu );
      _menus.push_back ( qtMenu );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_clearMenuBar()
{
  USUL_TRACE_SCOPE;

  this->menuBar()->clear();

  // Delete the menus we created.
  for ( Menus::iterator iter = _menus.begin(); iter != _menus.end(); ++iter )
    delete *iter;

  _menus.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make recent files menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initRecentFilesMenu()
{
  USUL_TRACE_SCOPE;

  // Make a menu if we need to.
  if ( 0x0 == _recentFilesMenu )
  {
    _recentFilesMenu = new MenuKit::Menu;
  }

  // Clear what we have.
  _recentFilesMenu->clear();

  // Set the name.
  _recentFilesMenu->text ( "&Recent Files" );

  // For convienence.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add to the recent file menu if the file exists.
  unsigned int count ( 0 );
  for ( StringList::const_iterator iter = _recentFiles.begin(); iter != _recentFiles.end(); ++iter )
  {
    const std::string file ( *iter );
    if ( true == Usul::Predicates::FileExists::test ( file ) )
    {
      CadKit::Helios::Commands::OpenDocument::RefPtr openDocument ( new CadKit::Helios::Commands::OpenDocument ( me ) );
      openDocument->filename ( file );
      std::string label ( Usul::Convert::Type<unsigned int,std::string>::convert ( ++count ) );
      label.insert ( ( 1 == label.size() ) ? 0 : label.size() - 1, "&" );
      openDocument->text ( Usul::Strings::format ( label, ' ', file ) );
      _recentFilesMenu->append ( new MenuKit::Button ( openDocument ) );
    }
  }

  _recentFilesMenu->addSeparator();

  // Useful typedefs.
  typedef void (MainWindow::*Function)();
  typedef Usul::Adaptors::MemberFunction < void, MainWindow*, Function > MemFun;
  typedef Usul::Commands::GenericCommand < MemFun > GenericCommand;

  // Add a clear button.
  _recentFilesMenu->append ( new MenuKit::Button ( new GenericCommand ( "Clear", MemFun ( this, &MainWindow::_clearRecentFiles ) ) ) );

  // Enable the menu only if there are recent files.
  _recentFilesMenu->enabled ( false == _recentFiles.empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear recent files.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_clearRecentFiles()
{
  USUL_TRACE_SCOPE;

  _recentFiles.clear();
  this->_initRecentFilesMenu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tool bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildToolBar()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "6391763130" );

  // Handle repeated calls.
  this->_clearToolBar();
  
  // Make the standard toolbar.
  const char *standardToolBarName ( "standard_tool_bar" );
  QToolBar *toolBar ( this->addToolBar ( standardToolBarName ) );
  _toolBars.insert ( ToolBars::value_type ( standardToolBarName, toolBar ) );

  // Set standard size.
  toolBar->setIconSize ( QSize ( 16, 16 ) );
  toolBar->setObjectName ( standardToolBarName );

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add buttons.
  {
    Action::RefPtr action ( new Action ( new CadKit::Helios::Commands::OpenDocument ( me ) ) );
    _toolBarActions.insert ( action );
    toolBar->addAction ( action.get() );
  }
  {
    Action::RefPtr action ( new Action ( new CadKit::Helios::Commands::SaveDocument ( me ) ) );
    _toolBarActions.insert ( action );
    toolBar->addAction ( action.get() );
  }
  
  {
    MenuKit::Menu::RefPtr toolBar ( new MenuKit::Menu );
    
    // Check the active document.
    {
      Usul::Interfaces::IToolBarAdd::QueryPtr ta ( Usul::Documents::Manager::instance().activeDocument() );
      if ( ta.valid() )
      {
        ta->toolBarAdd ( *toolBar, me.get() );
      }
    }
    
    // Check the active view.
    {
      Usul::Interfaces::IToolBarAdd::QueryPtr ta ( Usul::Documents::Manager::instance().activeView() );
      if ( ta.valid() )
      {
        ta->toolBarAdd ( *toolBar, me.get() );
      }
    }
    
    // Check all plugins.
    typedef Usul::Components::Manager PluginManager;
    typedef PluginManager::UnknownSet Unknowns;
    
    Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IToolBarAdd::IID ) );
    for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++ iter )
    {
      // Should be true.
      Usul::Interfaces::IToolBarAdd::ValidQueryPtr ta ( (*iter).get() );
      ta->toolBarAdd ( *toolBar, me.get() );
    }
    
    // Make the Qt tool bars.
    this->_buildQtToolBar ( *toolBar );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the Qt tool bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildQtToolBar( const MenuKit::Menu& toolBar )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "4595687620" );
  
  // Make the toolbars.
  for ( MenuKit::Menu::const_iterator iter = toolBar.begin(); iter != toolBar.end(); ++iter )
  {
    // We only have top level items that are menus.
    if ( MenuKit::Menu *menu = dynamic_cast < MenuKit::Menu* > ( iter->get() ) )
    {
      QToolBar *tb ( this->addToolBar ( menu->text().c_str() ) );
      _toolBars.insert ( ToolBars::value_type ( menu->text().c_str(), tb ) );
      
      // Set standard size.
      tb->setIconSize ( QSize ( 16, 16 ) );
      tb->setObjectName ( menu->text().c_str() );
      
      for ( MenuKit::Menu::const_iterator item = menu->begin(); item != menu->end(); ++item )
      {
        if ( MenuKit::Button* button = dynamic_cast<MenuKit::Button*> ( (*item).get() ) )
        {
          Action::RefPtr action ( new Action ( button->command() ) );
          _toolBarActions.insert ( action );
          tb->addAction ( action.get() );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the tool bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_clearToolBar()
{
	USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1988270626" );

	// This will delete the actions and set their callers to null.
  _toolBarActions.clear();

	// Remove the tool bars.
	for ( ToolBars::const_iterator iter = _toolBars.begin(); iter != _toolBars.end(); ++iter )
    this->removeToolBar ( iter->second );
  _toolBars.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the settings.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildTextWindow()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3978847086" );

  // Handle repeated calls.
  if ( 0x0 != _textWindow.first )
    return;

  // Build the docking window.
  QDockWidget *dock = new QDockWidget ( tr ( "Text Output" ), this );
  dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

  // Make the text window.
  _textWindow.first = new QTextEdit ( dock );

  // Set properties.
  _textWindow.first->setReadOnly ( true );

  // Keep text window from growing too big.
  if ( 0x0 != _textWindow.first->document() )
  {
    // Set maximum block count.
    const int maxBlocks ( Reg::instance()[Sections::TEXT_WINDOW][Keys::MAXIMUM_BLOCK_COUNT].get<int> ( 1000 ) );
    _textWindow.first->document()->setMaximumBlockCount ( maxBlocks );

    // Write it back so that we can at least modify the registry 
    // file to alter (until we have a GUI).
    Reg::instance()[Sections::TEXT_WINDOW][Keys::MAXIMUM_BLOCK_COUNT] = maxBlocks;
  }

  // Dock it.
  dock->setWidget ( _textWindow.first );
  this->addDockWidget ( Qt::BottomDockWidgetArea, dock );

  // Set the object name.
  dock->setObjectName ( "TextWindow" );

  // Add toggle to the menu.
  this->addDockWidgetMenu ( dock );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the progress bar window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildProgressBarWindow()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1230652240" );

  // Is there still a progress bar window?
  ProgressBarDock::RefPtr pb ( Usul::Threads::Safe::get ( this->mutex(), _progressBars ) );
  if ( false == pb.valid() )
    return;

  // Build the docking window.
  QDockWidget *dock = new QDockWidget ( tr ( "Progress Bars" ), this );
  dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

  // Dock it.
  dock->setWidget ( ( *pb ) ( dock ) );
  this->addDockWidget ( Qt::BottomDockWidgetArea, dock );

  // Set the object name.
  dock->setObjectName ( "ProgressBarsWindow" );

  // Add toggle to the menu.
  this->addDockWidgetMenu ( dock );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::ref()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrement the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::unref ( bool )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( 0 == _refCount )
  {
    USUL_ASSERT ( 0 );
    throw Usul::Exceptions::Exception ( "Error 4107780854: Reference count is already 0" );
  }

  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MainWindow::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  // No need to guard, should be re-entrant.

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ILoadFileDialog::IID:
    return static_cast<Usul::Interfaces::ILoadFileDialog*> ( this );
  case Usul::Interfaces::ISaveFileDialog::IID:
    return static_cast < Usul::Interfaces::ISaveFileDialog * > ( this );
  case Usul::Interfaces::IUpdateTextWindow::IID:
    return static_cast<Usul::Interfaces::IUpdateTextWindow*> ( this );
  case Usul::Interfaces::Qt::IMainWindow::IID:
    return static_cast < Usul::Interfaces::Qt::IMainWindow* > ( this );
  case Usul::Interfaces::Qt::IWorkspace::IID:
    return static_cast < Usul::Interfaces::Qt::IWorkspace* > ( this );
  case Usul::Interfaces::IGUIDelegateNotify::IID:
    return static_cast < Usul::Interfaces::IGUIDelegateNotify* > ( this );
  case Usul::Interfaces::IStreamListenerChar::IID:
    return static_cast < Usul::Interfaces::IStreamListenerChar* > ( this );
  case Usul::Interfaces::IProgressBarFactory::IID:
    return static_cast < Usul::Interfaces::IProgressBarFactory * > ( this );
  case Usul::Interfaces::IQtDockWidgetMenu::IID:
    return static_cast < Usul::Interfaces::IQtDockWidgetMenu * > ( this );
  case Usul::Interfaces::IActiveDocumentListener::IID:
    return static_cast < Usul::Interfaces::IActiveDocumentListener * > ( this );
  case Usul::Interfaces::IActiveViewListener::IID:
    return static_cast < Usul::Interfaces::IActiveViewListener * > ( this );
  case Usul::Interfaces::IQuestion::IID:
    return static_cast < Usul::Interfaces::IQuestion * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get a file name.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::FileResult MainWindow::getLoadFileName ( const std::string &title, const Filters &filters )
{
  USUL_TRACE_SCOPE;
  return QtTools::FileDialog::getLoadFileName ( this, title, filters );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get many file names.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::FilesResult MainWindow::getLoadFileNames ( const std::string &title, const Filters &filters )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "4159638088" );
  return QtTools::FileDialog::getLoadFileNames ( this, title, filters );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get a file name.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::FileResult MainWindow::getSaveFileName  ( const std::string &title, const Filters &filters )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1215213562" );
  return QtTools::FileDialog::getSaveFileName ( this, title, filters );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory where this binary lives.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::directory() const
{
  USUL_TRACE_SCOPE;
  // No guard needed.
  return Usul::CommandLine::Arguments::instance().directory();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::icon() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return std::string ( _icon.begin(), _icon.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the program file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::programFile() const
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );

  const std::string path ( Usul::CommandLine::Arguments::instance().program() );
  std::ostringstream file;
  file << Usul::File::base ( path ) << '.' << Usul::File::extension ( path );
  return file.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the program name.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::programName() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return std::string ( _program.begin(), _program.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vendor name.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::vendor() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return std::string ( _vendor.begin(), _vendor.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the url.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::url() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return std::string ( _url.begin(), _url.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the about.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::about() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return std::string ( _about.begin(), _about.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear plugin file names.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::clearPluginFiles()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _pluginFiles.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a plugin file name.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::addPluginFile ( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // If it's not empty...
  if ( false == name.empty() )
  {
    // If it's not already in the list...
    if ( _pluginFiles.end() == std::find ( _pluginFiles.begin(), _pluginFiles.end(), name ) )
    {
      // Append the name.
      _pluginFiles.push_back ( name );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the list of plugin file names.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::PluginFiles MainWindow::pluginFiles() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return PluginFiles ( _pluginFiles.begin(), _pluginFiles.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the default plugin file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::defautPluginFile() const
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );

  // First check the environment.
  std::string file ( Usul::System::Environment::get (  Usul::Strings::format ( Usul::Strings::upperCase ( this->programName() ), "_PLUGIN_FILE" ) ) );
  if ( false == file.empty() )
  {
    std::replace ( file.begin(), file.end(), '\\', '/' );
    return file;
  }

  // If we get to here then figure out the file.
  #ifdef __APPLE__
  // Relative path for an application bundle on OS X.
  std::string relativePath ( "/../Plugins/" );
  #else
  std::string relativePath ( "/../configs/" );
  #endif

  file = Usul::Strings::format ( this->directory(), relativePath, this->programName(), "Plugins.xml" );
  std::replace ( file.begin(), file.end(), '\\', '/' );

  return file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the loaded plugin names.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::printPlugins() const
{
  USUL_TRACE_SCOPE;
  Usul::Components::Manager::instance().print ( std::cout );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::loadPlugins()
{
  USUL_TRACE_SCOPE;
  const PluginFiles configs ( this->pluginFiles() );
  for ( PluginFiles::const_iterator i = configs.begin(); i != configs.end(); ++i )
  {
    const std::string config ( *i );
    this->loadPlugins ( config );
  }
}


///////////////////////////////////////////////////////////////////////////////ƒ
//
//  Load the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::loadPlugins ( const std::string &config )
{
	USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "4106161463" );

  if ( false == Usul::Predicates::FileExists::test ( config ) )
  {
    std::cout << "Warning 2088092978: Plugin file '" << config << "' does not exist" << std::endl;
    return;
  }

  Usul::Interfaces::IUnknown::QueryPtr unknown ( this );
  Usul::Interfaces::IUnknown::QueryPtr splash ( _splash );

  typedef Usul::Components::Loader < XmlTree::Document > Loader;
  Loader loader;

  // Set the directory on Apple.  Allow user to over ride by calling this before parse.
#ifdef __APPLE__
  const std::string directory ( Usul::CommandLine::Arguments::instance().directory() + "/../Plugins/" );
  loader.directory ( directory );
#endif
  
  std::cout << Usul::Strings::format ( "Loading plugin file: ", config ) << std::endl;

	loader.parse ( config );
  loader.load ( ( splash.valid() ) ? splash : unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the single plugin.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::loadPlugin ( const std::string &plugin )
{
	USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2338588069" );

  if ( false == Usul::Predicates::FileExists::test ( plugin ) )
  {
    std::cout << "Warning 3552654765: Plugin file '" << plugin << "' does not exist" << std::endl;
    return;
  }

  std::cout << "Processing plugin file: " << plugin << std::endl;

  Usul::Interfaces::IUnknown::QueryPtr unknown ( this );
  Usul::Interfaces::IUnknown::QueryPtr splash ( _splash );

  typedef Usul::Components::Loader < XmlTree::Document > Loader;
  Loader loader;
  loader.addPlugin ( plugin );
  loader.load ( ( splash.valid() ) ? splash : unknown );

  std::cout << "Done processing plugin file: " << plugin << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release all the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::releasePlugins()
{
  Usul::Components::Manager::instance().clear ( &std::cout );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the splash screen.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::showSplashScreen()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3041826876" );

  // Make the splash screen if we have to.
  if ( false == _splash.valid() )
  {
    const std::string iconDir ( Usul::App::Application::instance().iconDirectory() );
    const std::string splashImage ( Usul::App::Application::instance().splashImage() );
    _splash = new SplashScreen ( iconDir + splashImage );
  }

  if ( true == _splash.valid() )
  {
    _splash->show();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the splash screen.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::hideSplashScreen()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3041826876" );
  if ( true == _splash.valid() )
  {
    _splash->hide();
    _splash = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::updateTextWindow ( bool force )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  
  // Return now if queue is empty.
  if ( true == _textWindow.second->empty() )
    return;

  // Handle no text window.
  if ( 0x0 == _textWindow.first )
    return;
  
  // Return now if the window is hidden.
  // With Minerva printing to a log file, I don't think this is needed anymore.
  // Commenting out to see if text window still affects preformance.
#if 0
  if ( false == _textWindow.first->isVisible() )
  {
    _textWindow.second->clear();
    return;
  }
#endif

  // Don't allow this to throw because it may create an infinite loop.
  try
  {
    bool changed ( false );
    
    // Maximum number of iterations.
    const unsigned int maxIterations ( Reg::instance()[Sections::TEXT_WINDOW][Keys::MAXIMUM_ITERATIONS].get<unsigned int> ( 50, true ) );
    unsigned int iterations ( 0 );

    // Loop over all strings in the queue. The queue has an internal mutex.
    while ( false == _textWindow.second->empty() && iterations < maxIterations )
    {
      // Append the new string.
      const std::string s ( _textWindow.second->next() );
      _textWindow.first->append ( s.c_str() );
      changed = true;
      ++iterations;
    }

    // Force a GUI update now if we should.
    if ( true == force && true == changed )
    {
      _textWindow.first->update();
    }
  }

  // Catch all exceptions.
  catch ( ... )
  {
    try
    {
      _textWindow.first->append ( "Error 1536020510: failed to append new text output" );
    }
    catch ( ... )
    {
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use when profiling.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef USUL_USING_PROFILER

namespace Helper
{
  void autoQuit ( unsigned int maxCount )
  {
    static unsigned int count ( 0 );
    bool print ( false );
    Usul::Interfaces::IBusyState::QueryPtr busy ( Usul::Documents::Manager::instance().activeDocument() );
    if ( true == busy.valid() )
    {
      if ( false == busy->busyStateGet() )
      {
        ++count;
        print = true;
      }
      else
      {
        if ( count > 0 )
        {
          count = 0;
          print = true;
        }
      }
    }
    if ( count >= maxCount )
    {
      QApplication::quit();
    }
    if ( true == print )
    {
      std::cout << Usul::Strings::format ( "Idle count: ", count, " of ", maxCount, '\n' ) << std::flush;
    }
  }
}

#endif // USUL_USING_PROFILER


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the idle timer.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_idleProcess()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( &(Usul::Threads::Manager::instance()), &Usul::Threads::Manager::purge ) );

  // Tell window to refresh.
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &MainWindow::updateTextWindow ), true );

  // Update the timer server.
  if ( true == _timerServer.valid() )
  {
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( _timerServer.get(), &TimerServer::purge ), "2332384530" );
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( _timerServer.get(), &TimerServer::addPendingTimers ), "1116658650" );
  }

  #ifdef USUL_USING_PROFILER
  Helper::autoQuit ( 10 );
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all the documents.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_clearDocuments()
{
  USUL_TRACE_SCOPE;

  // Typedefs.
  typedef Usul::Documents::Manager::Documents  Documents;
  typedef Usul::Documents::Document            Document;

  {
    // Make a copy because as the documents are closing, they remove themselves from the document manager.
    Documents copy ( Usul::Documents::Manager::instance().documents() );

    // For convienence.
    Usul::Interfaces::IUnknown::QueryPtr me ( this );

    // Tell the remaining open documents that the application is about to close.
    // This allows the document to clean up any circular references.
    for ( Documents::iterator i = copy.begin(); i != copy.end(); ++i )
    {
      // Grab the document in a smart pointer so it doesn't get deleted out from under us.
      Document::RefPtr doc ( *i );
      doc->applicationClosing( me.get() );
    }
  }

  // Clear documents.
  Usul::Documents::Manager::instance().documents().clear();

  // Reset the document manager.
  Usul::Documents::Manager::reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the MainWindow.
//
///////////////////////////////////////////////////////////////////////////////

QMainWindow * MainWindow::mainWindow()
{
  USUL_TRACE_SCOPE;
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the MainWindow.
//
///////////////////////////////////////////////////////////////////////////////

const QMainWindow* MainWindow::mainWindow() const
{
  USUL_TRACE_SCOPE;
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the Workspace.
//
///////////////////////////////////////////////////////////////////////////////

QWorkspace * MainWindow::workspace()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _workSpace;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the Workspace.
//
///////////////////////////////////////////////////////////////////////////////

const QWorkspace* MainWindow::workspace() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _workSpace;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The document has finished loading.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::notifyDocumentFinishedLoading ( Usul::Documents::Document *document )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != document )
  {
    // Reference the document.  Some care will have to be taken since the proxy has a raw pointer.
    // The proxies aren't being deleted, which is why a raw pointer needs to be used.
    //document->ref(); I don't think the above comment is true anymore. -- Perry, 30-Mar-2009

    DocumentProxy proxy ( document );

    // This will add an event to the applications event loop.  It will be executed in the proper thread.
    QMetaObject::invokeMethod ( this, "_notifyDocumentFinishedLoading", Qt::QueuedConnection, 
                                Q_ARG ( DocumentProxy, proxy ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The document has finished loading. This function cannot throw!
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_notifyDocumentFinishedLoading ( DocumentProxy proxy  )
{
  USUL_TRACE_SCOPE;

  // If we get this far it should be the gui thread.
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Safely call the notification function.
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &MainWindow::_notifyFinishedLoading ), proxy.getDocument().get(), "3741587952" );

  // Safely set the proxy's document to null.
  typedef Usul::Documents::Document Document;
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( &proxy, &DocumentProxy::setDocument ), Document::RefPtr ( 0x0 ), "2091388641" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The document has finished loading.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_notifyFinishedLoading ( Usul::Documents::Document *document )
{
  USUL_TRACE_SCOPE;

  // If we get this far it should be the gui thread.
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Handle no document.
  if ( 0x0 == document )
    return;

  // The filename.
  std::string name ( document->fileName() );

  // Replace back-slashes. Otherwise, on Windows we can get the same file twice.
  std::replace ( name.begin(), name.end(), '\\', '/' );

  // Add the document to the recent file list if it isn't already in there.
  if ( _recentFiles.end() == std::find ( _recentFiles.begin(), _recentFiles.end(), name ) )
    _recentFiles.push_front ( name );

  // If the list is too long then remove the oldest (the front).
  const unsigned int maxRecentFiles ( Reg::instance()[Sections::MAIN_WINDOW][Keys::RECENT_FILES]["max_num"].get<unsigned int> ( 15 ) );
  while ( _recentFiles.size() > maxRecentFiles )
    _recentFiles.pop_back();

  // Typedefs.
  typedef Usul::Documents::Document Document;
  typedef Document::Delegate        Delegate;

  // If there is a valid delegate...
  Delegate::QueryPtr delegate ( document->delegate() );
  if ( delegate.valid() )
  {
    // Create the GUI.
    delegate->createDefaultGUI ( document, this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  }

  // Unreference.
  // The reason for this is no longer true. -- Perry, 30-Mar-2009.
  //document->unref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::notify ( Usul::Interfaces::IUnknown *caller, const char *values, unsigned int numValues )
{
  USUL_TRACE_SCOPE;

  if ( ( 0x0 != values ) && ( numValues > 0 ) )
  {
    // Remove all carriage returns and null characters.
    std::string s ( values, values + numValues );
    s.erase ( std::remove ( s.begin(), s.end(), '\r' ), s.end() );
    s.erase ( std::remove ( s.begin(), s.end(), '\0' ), s.end() );

    // Chop any trailing newline characters. The text window appends 
    // "paragraphs", which automatically start at the next line.
    if ( false == s.empty() && '\n' == s.at ( s.size() - 1 ) )
      s.resize ( s.size() - 1 );

    // Append text.
    this->textWindowAppend ( s );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The document has finished loading.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::textWindowAppend ( const std::string &s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Need to guard this check.
  if ( 0x0 != _textWindow.second.get() )
  {
    _textWindow.second->add ( s );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize all the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::initPlugins()
{
  USUL_TRACE_SCOPE;

  // Typedefs to shorten lines below...
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet Unknowns;
  typedef Usul::Interfaces::IPluginInitialize  IPluginInitialize;

  // Query point to this.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Look for plugins that create documents.
  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IDocumentCreate::IID ) );

  for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    // Should be true.
    Usul::Interfaces::IDocumentCreate::ValidQueryPtr dc ( (*iter).get() );

    // Create a document.
    Usul::Documents::Document::RefPtr document ( dc->createDocument( me ) );

    // Get the name of the document.
    std::string name ( document.valid() ? document->typeName() : "" );

    CadKit::Helios::Commands::NewDocument::RefPtr command ( new CadKit::Helios::Commands::NewDocument ( me, (*iter).get(), name ) );
    _newDocumentMenu->append ( new MenuKit::Button ( command.get() ) );
  }

  // Initialize plugins that need to.
  unknowns = PluginManager::instance().getInterfaces ( IPluginInitialize::IID );
  for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    try
    {
      IPluginInitialize::ValidQueryPtr pluginInit ( (*iter).get() );
      pluginInit->initializePlugin ( me.get() );
    }
    catch ( const std::exception &e )
    {
      std::cout << "Error 4126728500: Standard exception caught while trying to initialize plugin: " << e.what() << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Error 1505504842: Unknown exception caught while trying to initialize plugin.";
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  A close event has been recieved.  Close any windows that the workspace 
//  has open. Calling in the destructor is too late because the event loop 
//  has exited.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent ( QCloseEvent* event )
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &MainWindow::_closeEvent ), event, "1179334380" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A close event has been recieved.  Close any windows that the workspace 
//  has open. Calling in the destructor is too late because the event loop 
//  has exited.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_closeEvent ( QCloseEvent* event )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  typedef Usul::Documents::Manager::Documents Documents;
  typedef Usul::Documents::Document Document;
  
  // Get any remaining documents.
  Documents documents ( Usul::Documents::Manager::instance().documents() );

  // See if any documents need to be saved.
  for ( Documents::iterator i = documents.begin(); i != documents.end(); ++i )
  {
    if ( false == (*i)->canClose ( Usul::Interfaces::IUnknown::QueryPtr ( this ) ) )
    {
      event->ignore();
      return;
    }
  }

  // Tell the remaining open documents that the application is about to close.
  // This allows the document to clean up any circular references.
  for ( Documents::iterator i = documents.begin(); i != documents.end(); ++i )
  {
    // Grab the document in a smart pointer so it doesn't get deleted out from under us.
    Document::RefPtr doc ( *i );
    doc->applicationClosing( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }

  // Clear our copy.
  documents.clear();

  // Close child windows first.
  _workSpace->closeAllWindows();

  // Pass along to the base class.
  BaseClass::closeEvent ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr MainWindow::createProgressBar ( bool waitIfNotGuiThread )
{
  USUL_TRACE_SCOPE;
  ProgressBarDock::RefPtr pb ( Usul::Threads::Safe::get ( this->mutex(), _progressBars ) );
  return ( ( true == pb.valid() ) ? ( pb->createProgressBar ( waitIfNotGuiThread ) ) : IUnknown::RefPtr ( 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore dock window positions.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::restoreDockWindows()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  QByteArray defaults ( this->saveState ( Helios::Core::Constants::VERSION ) );
  QByteArray positions ( Reg::instance()[Sections::MAIN_WINDOW][Keys::DOCK_WINDOW_POSITIONS].get<QByteArray> ( defaults ) );
  this->restoreState ( positions, Helios::Core::Constants::VERSION );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add dock widget to menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::addDockWidgetMenu ( QDockWidget * dock )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  if ( _dockMenu.valid() )
  {
    MenuKit::Button::RefPtr button ( new MenuKit::ToggleButton ( new CadKit::Helios::Commands::ToggleView ( dock ) ) );
    button->toggle ( true );
    _dockMenu->append ( button.get() );
  }
  this->_buildQtMenu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dragging has entering window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::dragEnterEvent ( QDragEnterEvent *event )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  typedef QList < QUrl > Urls;
  typedef Urls::const_iterator ConstIterator;

  Urls urls ( event->mimeData()->urls() );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile().toStdString() );

    if ( Usul::Documents::Manager::instance().canOpen ( file ) )
      event->acceptProposedAction();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Files have been dropped.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::dropEvent ( QDropEvent *event )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  typedef QList < QUrl > Urls;
  typedef Urls::const_iterator ConstIterator;

  Urls urls ( event->mimeData()->urls() );

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile().toStdString() );

    CadKit::Helios::Commands::OpenDocument::RefPtr command ( new CadKit::Helios::Commands::OpenDocument ( me ) );
    command->filename ( file );
    command->execute ( 0x0 );
  }

  event->acceptProposedAction();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc )
{
  USUL_TRACE_SCOPE;
  //this->_buildMenuKitMenu();
  //this->_buildQtMenu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active view has changed.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView )
{
  USUL_TRACE_SCOPE;
  this->_buildToolBar();
  this->_buildMenuKitMenu();
  this->_buildQtMenu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::parseCommandLine ( int argc, char **argv )
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCallV1V2 ( Usul::Adaptors::memberFunction ( this, &MainWindow::_parseCommandLine ), argc, argv, "4427951490" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_parseCommandLine ( int argc, char **argv )
{
  USUL_TRACE_SCOPE;

  for ( int i = 1; i < argc; ++i )
  {
    this->loadFile ( argv[i] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the file.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::loadFile ( const std::string &file )
{
  USUL_TRACE_SCOPE;

  if ( true == Usul::Predicates::FileExists::test ( file ) )
  {
    Usul::Interfaces::IUnknown::QueryPtr me ( this );
    CadKit::Helios::Commands::OpenDocument::RefPtr command ( new CadKit::Helios::Commands::OpenDocument ( me ) );
    command->filename ( file );
    command->execute ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return name of registry file.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::settingsFileName() const
{
  USUL_TRACE_SCOPE;

  // Get persistant directory.
  const std::string dir ( Usul::User::Directory::vendor ( this->vendor(), true ) + this->programName() + "/" );

  // Make sure it exists.
  Usul::File::make ( dir );

  // Make the file name.
  std::string name ( dir + this->programName() + ".settings" );

  // Return file name.
  return name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt the user (IQuestion).
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::question ( const std::string &buttons, const std::string &title, const std::string &text )
{
  return QtTools::question ( this, buttons, title, text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cascade all child windows.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_childWindowsCascade()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  if ( 0x0 != _workSpace )
  {
    _workSpace->cascade();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tile all child windows.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_childWindowsTile()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  
  if ( 0x0 != _workSpace )
    _workSpace->tile();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close active window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_childWindowsCloseActive()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  
  if ( 0x0 != _workSpace )
    _workSpace->closeActiveWindow();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close all child windows.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_childWindowsCloseAll()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  
  if ( 0x0 != _workSpace )
    _workSpace->closeAllWindows();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does we have child windows.
//
///////////////////////////////////////////////////////////////////////////////

bool MainWindow::_childWindowsHas() const
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return false );
  
  return ( 0x0 != _workSpace ? !_workSpace->windowList().empty() : false );
}
