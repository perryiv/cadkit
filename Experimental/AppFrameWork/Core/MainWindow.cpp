
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/MainWindow.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/TextWindow.h"
#include "AppFrameWork/Core/BaseVisitor.h"
#include "AppFrameWork/Core/Define.h"

#include "AppFrameWork/Conditions/Always.h"
#include "AppFrameWork/Actions/SetTextFromStdout.h"
#include "AppFrameWork/Actions/SetTextFromPluginActivity.h"

#include <iostream>
#include <stdexcept>

using namespace AFW::Core;

AFW_IMPLEMENT_OBJECT ( MainWindow );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : BaseClass(),
  _menuBar    ( 0x0 ),
  _statusBar  ( 0x0 ),
  _editors    ( Program::instance().newObject<Frame>() )
{
  if ( _editors.valid() )
  {
    _editors->layout ( Frame::HORIZONTAL );
    this->append ( _editors.get() );
  }

  this->persistentName ( "main_window" );
  this->icon ( Icon ( "sun" ) );
  this->title ( Program::instance().app()->name() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
  // Safely...
  try
  {
    _menuBar = 0x0;
    _statusBar = 0x0;
    _editors = 0x0;
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( "7091126040", "6330931110" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a default GUI.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::init()
{
  Guard guard ( this->mutex() );

  // Add a menu bar.
  this->menuBar ( Program::instance().newObject<AFW::Menus::MenuBar>() );
  if ( this->menuBar() )
    this->menuBar()->init();

  // Add a status bar.
  this->statusBar ( Program::instance().newObject<StatusBar>() );

  // Update functors.
  AFW::Conditions::Always::RefPtr always ( new AFW::Conditions::Always );

  // Add a text output window.
  {
    TextWindow::ValidRefPtr text ( Program::instance().newObject<TextWindow>() );
    if ( text.valid() )
    {
      text->dockState ( DockState ( AFW::Core::DockSite::BOTTOM, 1 ) );
      text->persistentName ( "standard_output_window" );
      text->append ( always.get(), new AFW::Actions::SetTextFromStdout );
      this->append ( text.get() );
    }
  }

  // Add a window that displays plugin information.
  {  
    TextWindow::ValidRefPtr text ( Program::instance().newObject<TextWindow>() );
    if ( text.valid() )
    {
      text->dockState ( DockState ( AFW::Core::DockSite::BOTTOM, 1 ) );
      text->persistentName ( "plugin_activity_window" );
      text->append ( always.get(), new AFW::Actions::SetTextFromPluginActivity );
      text->title ( "Plugins" );
      text->icon ( Icon ( "plugins" ) );
      this->append ( text.get() );
    }
  }

  // Add a scene-view.
  {
    Frame::ValidRefPtr sceneTree ( Program::instance().newObject<Frame>() );
    if ( sceneTree.valid() )
    {
      sceneTree->dockState ( DockState ( AFW::Core::DockSite::LEFT, 0 ) );
      sceneTree->title ( "Scene Tree" );
      sceneTree->icon ( Icon ( "open" ) );
      this->append ( sceneTree.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::dirty ( bool state )
{
  Guard guard ( this->mutex() );
  BaseClass::dirty ( state );
  if ( state )
    AFW::Core::Program::instance().app()->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Just helps the compiler...
//
///////////////////////////////////////////////////////////////////////////////

bool MainWindow::dirty() const
{
  Guard guard ( this->mutex() );
  return BaseClass::dirty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::menuBar ( AFW::Menus::MenuBar *m )
{
  Guard guard ( this->mutex() );
  _menuBar = m;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Menus::MenuBar *MainWindow::menuBar()
{
  Guard guard ( this->mutex() );
  return _menuBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

const AFW::Menus::MenuBar *MainWindow::menuBar() const
{
  Guard guard ( this->mutex() );
  return _menuBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::statusBar ( StatusBar *m )
{
  Guard guard ( this->mutex() );
  _statusBar = m;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status bar.
//
///////////////////////////////////////////////////////////////////////////////

StatusBar *MainWindow::statusBar()
{
  Guard guard ( this->mutex() );
  return _statusBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status bar.
//
///////////////////////////////////////////////////////////////////////////////

const StatusBar *MainWindow::statusBar() const
{
  Guard guard ( this->mutex() );
  return _statusBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::accept ( AFW::Core::BaseVisitor *v )
{
  Guard guard ( this->mutex() );
  if ( v )
    v->visit ( this );
}
