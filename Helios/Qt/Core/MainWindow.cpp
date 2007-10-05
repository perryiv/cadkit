
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
#include "Helios/Qt/Core/Menu.h"
#include "Helios/Qt/Commands/Action.h"
#include "Helios/Qt/Commands/OpenDocument.h"
#include "Helios/Qt/Commands/InsertDocument.h"
#include "Helios/Qt/Commands/SaveDocument.h"
#include "Helios/Qt/Commands/SaveAsDocument.h"
#include "Helios/Qt/Commands/ExportImage.h"
#include "Helios/Qt/Commands/ExitApplication.h"
#include "Helios/Qt/Commands/ToggleView.h"
#include "Helios/Qt/Tools/Image.h"
#include "Helios/Qt/Tools/SettingsGroupScope.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Components/Loader.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/GUI/IAddDockWindow.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Strings/Qt.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include "XmlTree/Document.h"

#include "MenuKit/Button.h"

#include "QtCore/QStringList"
#include "QtCore/QTimer"
#include "QtGui/QApplication"
#include "QtGui/QDockWidget"
#include "QtGui/QFileDialog"
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
                         const std::string &icon,
                         const std::string &output,
                         bool showSplash ) : BaseClass(),
  _mutex        ( new MainWindow::Mutex ),
  _settings     ( QSettings::IniFormat, QSettings::UserScope, vendor.c_str(), program.c_str() ),
  _actions      (),
  _toolBars     (),
  _refCount     ( 0 ),
  _pluginFiles  (),
  _vendor       ( vendor ),
  _url          ( url ),
  _program      ( program ),
  _icon         ( icon ),
  _output       ( output ),
  _splash       ( 0x0 ),
  _workSpace    ( 0x0 ),
  _textWindow   ( 0x0, StreamQueuePtr ( new StreamQueue() ) ),
  _dockMenu     ( 0x0 ),
  _idleTimer    ( 0x0 ),
  _progressBars ( new ProgressBarDock ),
  _menu         ( 0x0 ),
  _menus        (),
  _recentFiles  (),
  _recentFilesMenu ( 0x0 )
{
  USUL_TRACE_SCOPE;

  // Name this thread.
  Usul::Threads::Named::instance().set ( Usul::Threads::Names::GUI );

  // Program-wide settings.
  QCoreApplication::setOrganizationName ( vendor.c_str() );
  QCoreApplication::setOrganizationDomain ( url.c_str() );
  QCoreApplication::setApplicationName ( program.c_str() );

  // Make the splash screen.
  const std::string iconDir ( Usul::App::Application::instance().iconDirectory() );
  const std::string splashImage ( Usul::App::Application::instance().splashImage() );
  _splash = new SplashScreen ( iconDir + splashImage );

  // Show the splash screen if we should.
  if ( true == showSplash )
    this->showSplashScreen();

  // Set the icon.
  CadKit::Helios::Tools::Image::icon ( icon, this );

  // Make the work space.
  _workSpace = new QWorkspace ( this );
  this->setCentralWidget ( _workSpace );

  // Enable toolbar docking.
  this->setEnabled ( true );

  // Create the menu for the docking windows.
  _dockMenu = new MenuKit::Menu;
  _dockMenu->text ( "Docking Windows" );

  // Make the menu.
  this->_initMenu();
  this->_buildToolBar();

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
  this->_loadSettings();

  // Make the menu.
  this->_initMenu();

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
  qRegisterMetaType < DocumentProxy > ( );
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

  // Remove this as an active view listener.
  Usul::Documents::Manager::instance().removeActiveViewListener ( this );

  // Remove this as an active document listener.
  Usul::Documents::Manager::instance().removeActiveDocumentListener ( this );

  // Clear the factory before releasing the plugins (some of them may 
  // populate it but not clean up).
  Usul::Factory::ObjectFactory::instance().clear();

  // Clear the menu bar.  To this before plugins are released.
  this->_clearMenuBar ();
  _menu = 0x0;
  _dockMenu = 0x0;
  _recentFilesMenu = 0x0;

  // Release all the plugins.
  this->releasePlugins();

  // Wait here until all jobs are done.
  std::cout << "Waiting for all jobs to finish..." << std::endl;
  Usul::Jobs::Manager::instance().purge();
  Usul::Jobs::Manager::instance().cancel();
  Usul::Jobs::Manager::instance().wait();
  Usul::Jobs::Manager::destroy();
  std::cout << "All jobs have finished" << std::endl;

  // Wait here until all threads are done.
  std::cout << "Waiting for all threads to finish..." << std::endl;
  Usul::Threads::Manager::instance().purge();
  Usul::Threads::Manager::instance().cancel();
  Usul::Threads::Manager::instance().wait();
  Usul::Threads::Manager::destroy();
  std::cout << "All threads have finished" << std::endl;

  // This will delete the actions and set their callers to null.
  _actions.clear();

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
//  Load the settings.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_loadSettings()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2863006339" );

  // Set the window's properties.
  {
    CadKit::Helios::Tools::SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::MAIN_WINDOW, _settings );
    const QRect rect ( _settings.value ( CadKit::Helios::Core::Registry::Keys::GEOMETRY.c_str(), CadKit::Helios::Core::Registry::Defaults::GEOMETRY ).value<QRect>() );
    this->setGeometry ( rect );

    _recentFiles = _settings.value  ( CadKit::Helios::Core::Registry::Keys::RECENT_FILES.c_str () ).toStringList ();

    // This is what we want...
    //this->setGeometry ( Usul::Registry::Database::instance().get<QRect> 
    //  ( CadKit::Helios::Core::Registry::Keys::GEOMETRY, CadKit::Helios::Core::Registry::Defaults::GEOMETRY ) );

    // also add to usul preferences...
  }
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
  {
    CadKit::Helios::Tools::SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::MAIN_WINDOW, _settings );
    const QRect rect ( this->geometry() );
    _settings.setValue ( CadKit::Helios::Core::Registry::Keys::GEOMETRY.c_str(), rect );

    // Save dock window positions.
    QByteArray positions ( this->saveState ( Helios::Core::Constants::VERSION ) );
    _settings.setValue ( CadKit::Helios::Core::Registry::Keys::DOCK_WINDOW_POSITIONS.c_str(), positions );

    // Save recent files.
    _settings.setValue ( CadKit::Helios::Core::Registry::Keys::RECENT_FILES.c_str (), _recentFiles );
  }

  // Write to disk.
  _settings.sync();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initMenu()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2357339900" );

  // Handle repeated calls.
  this->_clearMenuBar ();
  _menu = new MenuKit::Menu;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add file menu.
  {  
    MenuKit::Menu::RefPtr menu ( new MenuKit::Menu );
    menu->text ( "&File" );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::OpenDocument ( me ) ) );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::InsertDocument ( me ) ) );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::SaveDocument ( me ) ) );
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::SaveAsDocument ( me ) ) );
    menu->addSeparator();
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::ExportImage ( me ) ) );
    menu->addSeparator();

    // Make the recent files menu.
    this->_initRecentFilesMenu ();

    // Add the recent file menu.
    menu->append ( _recentFilesMenu );

    menu->addSeparator();
    menu->append ( new MenuKit::Button ( new CadKit::Helios::Commands::ExitApplication ( me ) ) );

    _menu->append ( menu );
  }

  // Add edit menu.
  {
  }

  // Add view menu.
  {
    {
      MenuKit::Menu::RefPtr view ( new MenuKit::Menu );
      view->text ( "&View" );
      view->append ( _dockMenu );
      _menu->append ( view );
    }
  }

  // Check the active document.
  {
    Usul::Interfaces::IMenuAdd::QueryPtr ma ( Usul::Documents::Manager::instance().activeDocument () );
    if ( ma.valid () )
    {
      ma->menuAdd ( *_menu );
    }
  }

  // Check the active view.
  {
    Usul::Interfaces::IMenuAdd::QueryPtr ma ( Usul::Documents::Manager::instance().activeView () );
    if ( ma.valid () )
    {
      ma->menuAdd ( *_menu );
    }
  }

  // Check all plugins.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet Unknowns;

  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IMenuAdd::IID ) );
  for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++ iter )
  {
    // Should be true.
    Usul::Interfaces::IMenuAdd::ValidQueryPtr ma ( *iter );
    ma->menuAdd ( *_menu );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildMenu ()
{
  // Handle repeated calls.
  this->_clearMenuBar ();

  // Build the qt menu.
  for ( MenuKit::Menu::iterator iter = _menu->begin(); iter != _menu->end (); ++iter )
  {
    // We only have top level items that are menus.
    if ( MenuKit::Menu *menu = dynamic_cast < MenuKit::Menu* > ( iter->get() ) )
    {
      CadKit::Helios::Core::Menu* qtMenu ( new CadKit::Helios::Core::Menu ( tr ( menu->text ().c_str() ) ) );
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

void MainWindow::_clearMenuBar ()
{
  this->menuBar()->clear();

  // Delete the menus we created.
  for ( Menus::iterator iter = _menus.begin(); iter != _menus.end(); ++iter )
    delete *iter;

  _menus.clear ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make recent files menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initRecentFilesMenu()
{
  // Make a menu if we need to.
  if ( 0x0 == _recentFilesMenu )
  {
    _recentFilesMenu = new MenuKit::Menu;
  }

  // Clear what we have.
  _recentFilesMenu->clear ();

  // Set the name.
  _recentFilesMenu->text ( "Recent Files" );

  // For convienence.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add to the recent file menu.
  for ( QStringList::iterator iter = _recentFiles.begin(); iter != _recentFiles.end(); ++iter )
  {
    CadKit::Helios::Commands::OpenDocument::RefPtr openDocument ( new CadKit::Helios::Commands::OpenDocument ( me ) );
    openDocument->filename ( iter->toStdString () );
    _recentFilesMenu->append ( new MenuKit::Button ( openDocument ) );
  }

  _recentFilesMenu->addSeparator ();

  // Useful typedefs.
  typedef void (MainWindow::*Function) ();
  typedef Usul::Adaptors::MemberFunction < void, MainWindow*, Function > MemFun;
  typedef Usul::Commands::GenericCommand < MemFun > GenericCommand;

  // Add a clear button.
  _recentFilesMenu->append ( new MenuKit::Button ( new GenericCommand ( "Clear", MemFun ( this, &MainWindow::_clearRecentFiles ) ) ) );

  // Enable the menu only if there are recent files.
  _recentFilesMenu->enabled ( !_recentFiles.empty () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear recent files.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_clearRecentFiles ()
{
  _recentFiles.clear ();
  this->_initRecentFilesMenu ();
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
  const char *standardToolBarName ( "standard_tool_bar" );
  this->removeToolBar ( _toolBars[standardToolBarName] );
  QToolBar *toolBar ( this->addToolBar ( standardToolBarName ) );
  _toolBars.insert ( ToolBars::value_type ( standardToolBarName, toolBar ) );

  // Set standard size.
  toolBar->setIconSize ( QSize ( 16, 16 ) );
  toolBar->setObjectName ( standardToolBarName );

  typedef CadKit::Helios::Commands::Action Action;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add buttons.
  {
    CadKit::Helios::Commands::BaseAction::RefPtr action ( new Action ( new CadKit::Helios::Commands::OpenDocument ( me ) ) );
    _actions.insert ( action );
    toolBar->addAction ( action.get() );
  }
  {
    CadKit::Helios::Commands::BaseAction::RefPtr action ( new Action ( new CadKit::Helios::Commands::SaveDocument ( me ) ) );
    _actions.insert ( action );
    toolBar->addAction ( action.get() );
  }
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
  _textWindow.first->setReadOnly ( true );

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

  // Build the docking window.
  QDockWidget *dock = new QDockWidget ( tr ( "Progress Bars" ), this );
  dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

  // Dock it.
  dock->setWidget ( ( *_progressBars ) ( dock ) );
  this->addDockWidget ( Qt::BottomDockWidgetArea, dock );

  // Set the object name.
  dock->setObjectName ( "ProgressBarsWindow" );

  // Add toggle to the menu.
  this->addDockWidgetMenu ( dock );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the settings.
//
///////////////////////////////////////////////////////////////////////////////

const QSettings &MainWindow::settings() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _settings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the settings.
//
///////////////////////////////////////////////////////////////////////////////

QSettings &MainWindow::settings()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _settings;
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
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 4107780854: Reference count is already 0" );
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
    return static_cast<Usul::Interfaces::ILoadFileDialog*>(this);
  case Usul::Interfaces::ISaveFileDialog::IID:
    return static_cast < Usul::Interfaces::ISaveFileDialog * > ( this );
  case Usul::Interfaces::IUpdateTextWindow::IID:
    return static_cast<Usul::Interfaces::IUpdateTextWindow*>(this);
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
  const FilesResult result ( this->getLoadFileNames ( title, filters ) );
  return ( ( result.first.empty() ) ? FileResult() : FileResult ( result.first.front(), result.second ) );
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

  // Initialize the answer.
  FilesResult files;

  // Set the filter string.
  const std::string allFilters ( this->_formatFilters ( filters ) );

  // Get the current filter.
  const std::string filter ( this->_lastFileDialogFilter ( title ) );

  // Get the directory.
  const std::string dir ( this->_lastFileDialogDir ( title ) );

  // Need to use this static function to get native file dialog.
  QStringList answer ( QFileDialog::getOpenFileNames ( this, title.c_str(), dir.c_str(), allFilters.c_str(), 0x0 ) );

  // If we have at least one file.
  if( false == answer.empty() )
  {
    std::string filename ( answer.first().toStdString() );
    std::string directory ( Usul::File::directory ( filename, false ) );
    std::string filter ( "" );

    // Save the directory and filter.
    this->_lastFileDialogDir    ( title, directory );
    this->_lastFileDialogFilter ( title, filter    );

    files.second.first = filter;

    // Fox and WinForms both use a string for the filter "label" and another 
    // one for the actual file extensions. Qt does not, so the fact that 
    // files.second is a pair is legacy.
    files.second.second = files.second.first;
  }

  // Convert the answer.
  for( QStringList::const_iterator iter = answer.begin(); iter != answer.end(); ++iter )
  {
    files.first.push_back ( iter->toStdString() );
  }

  // Return the result, which may be an empty list.
  return files;
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

  // Initialize the answer.
  FileResult file;

  // Set the filter string.
  const std::string allFilters ( this->_formatFilters ( filters ) );

  // Get the current filter.
  const std::string filter ( this->_lastFileDialogFilter ( title ) );

  // Get the directory.
  const std::string dir ( this->_lastFileDialogDir ( title ) );

  // Need to use this static function to get native file dialog.
  QString answer ( QFileDialog::getSaveFileName ( this, title.c_str(), dir.c_str(), allFilters.c_str(), 0x0 ) );

  if( 0 != answer.size () )
  {
    std::string filename ( answer.toStdString() );
    std::string directory ( Usul::File::directory ( filename, false ) );
    std::string filter ( "" );

    // Save the directory and filter.
    this->_lastFileDialogDir    ( title, directory );
    this->_lastFileDialogFilter ( title, filter    );

    file.first = filename;
  }

  return file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the given filters into a single string.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::_formatFilters ( const Filters &filters ) const
{
  USUL_TRACE_SCOPE;

  // Initialize the answer.
  QStringList allFilters;

  // Loop through and append each filter.
  for ( Filters::const_iterator i = filters.begin(); i != filters.end(); ++i )
  {
    const std::string filter ( i->first );
    allFilters.push_back ( filter.c_str() );
  }

  // Sort the filters.
  allFilters.sort();

  // Put a semi-colon between each filter.
  QString answer ( allFilters.join ( ";;" ) );

  // Return the answer.
  return answer.toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::_lastFileDialogDir ( const std::string &title ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  const std::string key ( "helios_qt_file_dialog_" + title );
  QVariant dir ( _settings.value ( key.c_str(), QString ( Usul::CommandLine::Arguments::instance().directory().c_str() ) ) );
  return dir.toString().toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last directory.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_lastFileDialogDir ( const std::string &title, const std::string &dir )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  const std::string key ( "helios_qt_file_dialog_" + title );
  _settings.setValue ( key.c_str(), QVariant ( dir.c_str() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last file dialog filter.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::_lastFileDialogFilter ( const std::string &title ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  std::string filter;
  return filter;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last file dialog filter.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_lastFileDialogFilter ( const std::string &title, const std::string &filter ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
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

  std::string file ( this->directory() + "/../configs/" + this->programName() + "Plugins.xml" );
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


///////////////////////////////////////////////////////////////////////////////
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
	loader.parse( config );
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
  if ( true == _splash.valid() )
    _splash->show();
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
    _splash->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_updateTextWindow()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  this->updateTextWindow ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::updateTextWindow ( bool force )
{
  USUL_TRACE_SCOPE;

  // If this is not the GUI thread then re-direct.
  if ( false == Usul::Threads::Named::instance().is ( Usul::Threads::Names::GUI ) )
  {
    QMetaObject::invokeMethod ( this, "_updateTextWindow", Qt::QueuedConnection );
    return;
  }

  // If we get this far it should be the gui thread.
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Handle no text window.
  if ( 0x0 == _textWindow.first )
    return;

  // Don't allow this to throw because it may create an infinite loop.
  try
  {
    bool changed ( false );

    // Loop over all strings in the queue. The queue has an internal mutex.
    while ( false == _textWindow.second->empty() )
    {
      // Append the new string.
      const std::string s ( _textWindow.second->next() );
      _textWindow.first->append ( s.c_str() );
      changed = true;
    }

    // Force a GUI update now if we should.
    if ( true == force && true == changed )
      _textWindow.first->update();
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
//  Called by the idle timer.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_idleProcess()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( &(Usul::Jobs::Manager::instance()),    &Usul::Jobs::Manager::purge    ) );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( &(Usul::Threads::Manager::instance()), &Usul::Threads::Manager::purge ) );
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

void MainWindow::notifyDocumentFinishedLoading ( Usul::Documents::Document* document )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != document )
  {
    // Reference the document.  Some care will have to be taken since the proxy has a raw pointer.
    // The proxies aren't being deleted, which is why a raw pointer needs to be used.
    document->ref();

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
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &MainWindow::_notifyFinishedLoading ), proxy.getDocument(), "3741587952" );

  // Safely set the proxy's document to null.
  typedef Usul::Documents::Document Document;
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( &proxy, &DocumentProxy::setDocument ), static_cast<Document *> ( 0x0 ), "2091388641" );
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
  std::string filename ( document->fileName () );

  // Add the document to the recent file list.
  if ( false == _recentFiles.contains ( filename.c_str () ) )
    _recentFiles.append ( filename.c_str () );

  // If the recent files are too big...
  if ( _recentFiles.count () > 10 )
    _recentFiles.erase ( _recentFiles.begin () );

  // Typedefs.
  typedef Usul::Documents::Document Document;
  typedef Document::Delegate        Delegate;

  // If there is a valid delegate...
  Delegate::QueryPtr delegate ( document->delegate() );
  if ( delegate.valid() );
  {
    // Create the GUI.
    delegate->createDefaultGUI ( document, this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  }

  // Unreference.
  document->unref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The document has finished loading.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::notify ( Usul::Interfaces::IUnknown *caller, const char *values, unsigned int numValues )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != values && numValues > 0 )
  {
    // Remove all carriage returns and null characters.
    std::string s ( values, values + numValues );
    s.erase ( std::remove ( s.begin(), s.end(), '\r' ), s.end() );
    s.erase ( std::remove ( s.begin(), s.end(), '\0' ), s.end() );

    // Chop any trailing newline characters. The text window appends 
    // "paragraphs", which automatically start at the next line.
    if ( false == s.empty() && '\n' == s.at ( s.size() - 1 ) )
      s.resize ( s.size() - 1 );

    // Do not guard, the queue has an internal mutex.
    _textWindow.second->add ( s );

    // Tell window to refresh.
    this->updateTextWindow ( true );
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

  // Have the plugins build any dock widgets.
  this->_buildPluginDockWidgets();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Have the plugins build any dock widgets.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildPluginDockWidgets()
{
  USUL_TRACE_SCOPE;

  typedef Usul::Components::Manager Manager;
  typedef Manager::UnknownSet       Unknowns;

  // Get all the plugins that implement Usul::Interfaces::IAddDockWindow.
  Unknowns unknowns ( Manager::instance().getInterfaces ( Usul::Interfaces::IAddDockWindow::IID ) );

  // For convienence.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Go through the plugins.
  for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::IAddDockWindow::ValidQueryPtr plugin ( (*iter).get() );
    plugin->addDockWindow ( me );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  A close event has been recieved.  Close any windows that the workspace 
//  has open. Calling in the destructor is too late because the event loop 
//  has exited.
//
//  TODO: Ask the documents if they need to save before calling the 
//  BaseClass' function.
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
//  TODO: Ask the documents if they need to save before calling the 
//  BaseClass' function.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_closeEvent ( QCloseEvent* event )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

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

Usul::Interfaces::IUnknown* MainWindow::createProgressBar()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return 0x0 );
  return _progressBars->createProgressBar ();
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

  // Restore dock window positions.
  CadKit::Helios::Tools::SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::MAIN_WINDOW, _settings );  
  QByteArray positions ( _settings.value ( CadKit::Helios::Core::Registry::Keys::DOCK_WINDOW_POSITIONS.c_str() ).toByteArray() );
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

  if ( _dockMenu.valid () )
  {
    MenuKit::Button::RefPtr button ( new MenuKit::Button ( new CadKit::Helios::Commands::ToggleView ( dock ) ) );
    button->toggle ( true );
    _dockMenu->append ( button.get() );
  }
  this->_buildMenu ();
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

  Urls urls ( event->mimeData()->urls () );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile ().toStdString () );

    if ( Usul::Documents::Manager::instance ().canOpen ( file ) )
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

  Urls urls ( event->mimeData()->urls () );

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile ().toStdString () );

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
  this->_initMenu ();
  this->_buildMenu ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active view has changed.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView )
{
  USUL_TRACE_SCOPE;
  this->_initMenu ();
  this->_buildMenu ();
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
