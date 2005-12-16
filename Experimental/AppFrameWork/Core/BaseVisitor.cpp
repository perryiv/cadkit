
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all visitors.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/BaseVisitor.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/MainWindow.h"
#include "AppFrameWork/Core/Group.h"
#include "AppFrameWork/Core/TextWindow.h"
#include "AppFrameWork/Core/StatusBar.h"
#include "AppFrameWork/Menus/Button.h"
#include "AppFrameWork/Menus/MenuGroup.h"
#include "AppFrameWork/Menus/MenuBar.h"
#include "AppFrameWork/Dialogs/Dialog.h"

using namespace AFW::Core;

USUL_IMPLEMENT_TYPE_ID ( BaseVisitor );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseVisitor::BaseVisitor() : BaseClass(),
  _mutex()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseVisitor::~BaseVisitor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the mutex.
//
///////////////////////////////////////////////////////////////////////////////

BaseVisitor::Mutex &BaseVisitor::mutex() const
{
  return _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the application.
//
///////////////////////////////////////////////////////////////////////////////

void BaseVisitor::visit ( AFW::Core::Application *app )
{
  Guard guard ( _mutex );

  if ( 0x0 == app )
    return;

  AFW::Core::MainWindow::RefPtr mw ( app->mainWindow() );
  if ( true == mw.valid() )
  {
    mw->accept ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the window.
//
///////////////////////////////////////////////////////////////////////////////

void BaseVisitor::visit ( AFW::Core::Window *window )
{
  Guard guard ( _mutex );
  if ( 0x0 != window )
    window->_traverse ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define default visit functions.
//
///////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_VISIT_FUNCTION(object_type)\
void BaseVisitor::visit ( object_type *object )\
{\
  Guard guard ( _mutex );\
  typedef object_type::BaseClass BaseObjectType;\
  this->visit ( static_cast < BaseObjectType * > ( object ) );\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Define default visitor functions.
//
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_VISIT_FUNCTION ( AFW::Core::Group );
IMPLEMENT_VISIT_FUNCTION ( AFW::Core::Frame );
IMPLEMENT_VISIT_FUNCTION ( AFW::Core::MainWindow );
IMPLEMENT_VISIT_FUNCTION ( AFW::Core::TextWindow );
IMPLEMENT_VISIT_FUNCTION ( AFW::Core::StatusBar );
IMPLEMENT_VISIT_FUNCTION ( AFW::Menus::Button );
IMPLEMENT_VISIT_FUNCTION ( AFW::Menus::MenuBar );
IMPLEMENT_VISIT_FUNCTION ( AFW::Menus::MenuGroup );
IMPLEMENT_VISIT_FUNCTION ( AFW::Dialogs::Dialog );
