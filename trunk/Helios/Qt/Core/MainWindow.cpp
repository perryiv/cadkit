
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/MainWindow.h"
#include "Helios/Qt/Core/Constants.h"
#include "Helios/Qt/Core/Icon.h"
#include "Helios/Qt/Core/SettingsGroupScope.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QApplication"
#include "QtGui/QMenuBar"
#include "QtGui/QImageReader"
#include "QtGui/QStatusBar"
#include "QtGui/QToolBar"

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Macros used below.
//
///////////////////////////////////////////////////////////////////////////////

#define ENSURE_GUI_THREAD\
  if ( false == this->isGuiThread() )\
    return


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow ( const std::string &vendor, 
                         const std::string &url, 
                         const std::string &program ) : BaseClass(),
  _mutex     ( new MainWindow::Mutex ),
  _settings  ( QSettings::IniFormat, QSettings::UserScope, vendor.c_str(), program.c_str() ),
  _guiThread ( Usul::Threads::currentThreadId() ),
  _actions   (),
  _toolBars  ()
{
  USUL_TRACE_SCOPE;

  // Program-wide settings.
  QCoreApplication::setOrganizationName ( vendor.c_str() );
  QCoreApplication::setOrganizationDomain ( url.c_str() );
  QCoreApplication::setApplicationName ( program.c_str() );

  // Set the icon.
  CadKit::Helios::Core::Icon::set ( "helios_sun.png", this );

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
  ENSURE_GUI_THREAD;
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
  ENSURE_GUI_THREAD;

  // Set the window's properties.
  {
    SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::MAIN_WINDOW, _settings );
    const QRect rect ( _settings.value ( CadKit::Helios::Core::Registry::Keys::GEOMETRY.c_str(), CadKit::Helios::Core::Registry::Defaults::GEOMETRY ).value<QRect>() );
    this->setGeometry ( rect );
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
  ENSURE_GUI_THREAD;

  // Set the window's properties.
  {
    SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::MAIN_WINDOW, _settings );
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
  ENSURE_GUI_THREAD;

  // Make sure actions are built.
  this->_makeActions();

  // Handle repeated calls.
  this->menuBar()->clear();

  // Add menus.
  QMenu *menu = this->menuBar()->addMenu ( tr ( "&File" ) );
  menu->addAction ( _actions["open_document"] );
  menu->addSeparator();
  menu->addAction ( _actions["exit"] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tool bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_buildToolBar()
{
  USUL_TRACE_SCOPE;
  ENSURE_GUI_THREAD;

  // Make sure actions are built.
  this->_makeActions();

  // Handle repeated calls.
  const char *standardToolBarName ( "standard_tool_bar" );
  this->removeToolBar ( _toolBars[standardToolBarName] );
  QToolBar *toolBar ( this->addToolBar ( standardToolBarName ) );
  _toolBars.insert ( ToolBars::value_type ( standardToolBarName, toolBar ) );

  // Set standard size.
  toolBar->setIconSize ( QSize ( 16, 16 ) );

  // Add buttons.
  toolBar->addAction ( _actions["open_document"] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the actions.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_makeActions()
{
  USUL_TRACE_SCOPE;
  ENSURE_GUI_THREAD;

  // Only build the actions once.
  if ( false == _actions.empty() )
    return;

  {
    QAction *action ( new QAction ( tr ( "&Open" ), this ) );
    action->setShortcut ( tr ( "Ctrl+O" ) );
    action->setStatusTip ( "Open existing document" );
    CadKit::Helios::Core::Icon::set ( "openDocument.png", action );
    this->connect ( action, SIGNAL ( triggered() ), this, SLOT ( _open() ) );
    _actions.insert ( Actions::value_type ( "open_document", action ) );
  }

  {
    QAction *action ( new QAction ( tr ( "E&xit" ), this ) );
    action->setStatusTip ( "Exit the application" );
    this->connect ( action, SIGNAL ( triggered() ), this, SLOT ( _quit() ) );
    _actions.insert ( Actions::value_type ( "exit", action ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open files.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_open()
{
  USUL_TRACE_SCOPE;
  ENSURE_GUI_THREAD;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the program.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_quit()
{
  USUL_TRACE_SCOPE;
  ENSURE_GUI_THREAD;

  USUL_ERROR_STACK_CATCH_EXCEPTIONS_BEGIN;

  this->close();

  USUL_ERROR_STACK_CATCH_EXCEPTIONS_END ( 2624682894 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the calling thread is the GUI thread.
//
///////////////////////////////////////////////////////////////////////////////

bool MainWindow::isGuiThread() const
{
  USUL_TRACE_SCOPE;
  const unsigned long thread ( Usul::Threads::currentThreadId() );
  return ( thread == _guiThread );
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
