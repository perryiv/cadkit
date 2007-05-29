
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
#include "Helios/Qt/Core/SplashScreen.h"
#include "Helios/Qt/Commands/Action.h"
#include "Helios/Qt/Commands/OpenDocument.h"
#include "Helios/Qt/Commands/ExitApplication.h"
#include "Helios/Qt/Tools/Image.h"
#include "Helios/Qt/Tools/SettingsGroupScope.h"
#include "Helios/Plugins/Manager/Loader.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Errors/Stack.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Qt.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QApplication"
#include "QtGui/QDockWidget"
#include "QtGui/QImageReader"
#include "QtGui/QLabel"
#include "QtGui/QMenuBar"
#include "QtGui/QProgressDialog"
#include "QtGui/QPushButton"
#include "QtGui/QStatusBar"
#include "QtGui/QToolBar"
#include "QtGui/QVBoxLayout"

#include <algorithm>

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow ( const std::string &vendor, 
                         const std::string &url, 
                         const std::string &program,
                         const std::string &icon,
                         bool showSplash ) : BaseClass(),
  _mutex       ( new MainWindow::Mutex ),
  _settings    ( QSettings::IniFormat, QSettings::UserScope, vendor.c_str(), program.c_str() ),
  _actions     (),
  _toolBars    (),
  _threadPool  ( 0x0 ),
  _refCount    ( 0 ),
  _pluginFiles (),
  _vendor      ( vendor ),
  _url         ( url ),
  _program     ( program ),
  _icon        ( icon ),
  _splash      ( 0x0 )
{
  USUL_TRACE_SCOPE;

  // Name this thread.
  Usul::Threads::Named::instance().set ( Usul::Threads::Names::GUI );

  // Make the splash screen.
  const std::string iconDir ( Usul::App::Application::instance().iconDirectory() );
  const std::string splashImage ( Usul::App::Application::instance().splashImage() );
  _splash = new SplashScreen ( iconDir + splashImage );

  // Show the splash screen if we should.
  if ( true == showSplash )
    this->showSplashScreen();

  // Program-wide settings.
  QCoreApplication::setOrganizationName ( vendor.c_str() );
  QCoreApplication::setOrganizationDomain ( url.c_str() );
  QCoreApplication::setApplicationName ( program.c_str() );

  // Set the icon.
  CadKit::Helios::Tools::Image::icon ( icon, this );

  // Enable toolbar docking.
  this->setEnabled ( true );

  // Make the menu.
  this->_buildMenu();
  this->_buildToolBar();

  // Make sure we can size the status bar.
  this->statusBar()->setSizeGripEnabled ( true );
  this->statusBar()->showMessage ( tr ( "Ready" ) );

  // Load the settings.
  this->_loadSettings();

  // Set the title.
  this->setWindowTitle ( program.c_str() );

  // Start the thread pool.
  _threadPool = new ThreadPool;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MainWindow::_saveSettings ), "1772821423" );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MainWindow::_destroy ),      "1934297230" );
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

  // Wait here until all threads in the pool are done.
  _threadPool->wait();
  _threadPool = 0x0;

  // Wait here until all threads are done.
  Usul::Threads::Manager::instance().wait();

  // This will delete the actions and set their callers to null.
  _actions.clear();

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );

  // Delete remaining members.
  _pluginFiles.clear();
  _vendor.clear();
  _url.clear();
  _program.clear();
  _icon.clear();
  _splash = 0x0;

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
  }

  // Write to disk.
  _settings.sync();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildMenu()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "2357339900" );

  // Handle repeated calls.
  this->menuBar()->clear();

  // Add menus.
  QMenu *menu = this->menuBar()->addMenu ( tr ( "&File" ) );

  {
    CadKit::Helios::Commands::BaseAction::RefPtr action ( new CadKit::Helios::Commands::Action<CadKit::Helios::Commands::OpenDocument> ( this ) );
    _actions.insert ( action );
    menu->addAction ( action.get() );
  }
  menu->addSeparator();
  {
    CadKit::Helios::Commands::BaseAction::RefPtr action ( new CadKit::Helios::Commands::Action<CadKit::Helios::Commands::ExitApplication> ( this ) );
    _actions.insert ( action );
    menu->addAction ( action.get() );
  }
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

  // Add buttons.
  {
    CadKit::Helios::Commands::BaseAction::RefPtr action ( new CadKit::Helios::Commands::Action<CadKit::Helios::Commands::OpenDocument> ( this ) );
    _actions.insert ( action );
    toolBar->addAction ( action.get() );
  }
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
  case Usul::Interfaces::ILoadFileDialog::IID:
    return static_cast<Usul::Interfaces::ILoadFileDialog*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<Usul::Interfaces::ILoadFileDialog*>(this));
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
  Guard guard ( this->mutex() );
  FilesResult result ( this->getLoadFileNames ( title, filters ) );
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
  Guard guard ( this->mutex() );
  FilesResult result;
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory where this binary lives.
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::directory() const
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
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

  std::string file ( this->directory() + "/../config/" + this->programFile() );
  std::replace ( file.begin(), file.end(), '\\', '/' );
  return file;
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

  std::cout << "Processing plugin config file: " << config << std::endl;

  Usul::Interfaces::IUnknown::QueryPtr unknown ( this );
  Usul::Interfaces::IUnknown::QueryPtr splash ( _splash );

  CadKit::Helios::Plugins::Manager::Loader loader;
  loader.filename ( config );
	loader.parse();
  loader.load ( ( splash.valid() ) ? splash : unknown );

  std::cout << "Done processing plugin config file: " << config << std::endl;
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

  CadKit::Helios::Plugins::Manager::Loader loader;
  loader.addPlugin ( plugin );
  loader.load ( ( splash.valid() ) ? splash : unknown );

  std::cout << "Done processing plugin file: " << plugin << std::endl;
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
