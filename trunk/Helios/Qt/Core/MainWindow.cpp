
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
#include "Helios/Qt/Tools/Icon.h"
#include "Helios/Qt/Tools/Move.h"
#include "Helios/Qt/Tools/SettingsGroupScope.h"
#include "Helios/Plugins/Manager/Loader.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Predicates/FileExists.h"
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
#include "QtGui/QSplashScreen"
#include "QtGui/QStatusBar"
#include "QtGui/QToolBar"
#include "QtGui/QVBoxLayout"

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow ( const std::string &vendor, 
                         const std::string &url, 
                         const std::string &program ) : BaseClass(),
  _mutex      ( new MainWindow::Mutex ),
  _settings   ( QSettings::IniFormat, QSettings::UserScope, vendor.c_str(), program.c_str() ),
  _actions    (),
  _toolBars   (),
  _threadPool ( 0x0 ),
  _refCount   ( 0 )
{
  USUL_TRACE_SCOPE;

  // Name this thread.
  Usul::Threads::Named::instance().set ( Usul::Threads::Names::GUI );

  // Make the splash screen.
  SplashScreen splashScreen ( this );
  CadKit::Helios::Tools::Move::center ( &splashScreen );
  splashScreen.show();

  // Program-wide settings.
  QCoreApplication::setOrganizationName ( vendor.c_str() );
  QCoreApplication::setOrganizationDomain ( url.c_str() );
  QCoreApplication::setApplicationName ( program.c_str() );

  // Set the icon.
  CadKit::Helios::Tools::Icon::set ( "helios_sun.png", this );

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
  this->setWindowTitle ( QCoreApplication::applicationName() );

  // Load plugins.
  this->_loadPlugins ( splashScreen.queryInterface ( Usul::Interfaces::IUnknown::IID ) );

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

  delete _mutex;
  _mutex = 0x0;
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
//  Load the plugins.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_loadPlugins ( Usul::Interfaces::IUnknown *caller )
{
	USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "4106161463" );

	std::string pluginFile ( Usul::CommandLine::Arguments::instance().directory() + "/../config/HeliosQt.xml" );
	USUL_TRACE_2 ( "Plugin file: ", pluginFile );
	
	if ( Usul::Predicates::FileExists::test ( pluginFile ) )
	{
		CadKit::Helios::Plugins::Manager::Loader loader;
    loader.filename ( pluginFile );
		loader.parse();
		loader.load ( caller );
	}
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
