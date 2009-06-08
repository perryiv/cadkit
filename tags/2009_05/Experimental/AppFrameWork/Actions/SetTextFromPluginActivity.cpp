
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the window's text from plugin activity.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Actions/SetTextFromPluginActivity.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Windows/Window.h"

using namespace AFW::Actions;

USUL_IMPLEMENT_TYPE_ID ( SetTextFromPluginActivity );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SetTextFromPluginActivity::SetTextFromPluginActivity() : BaseClass(),
  _size ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SetTextFromPluginActivity::~SetTextFromPluginActivity()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform the action.
//
///////////////////////////////////////////////////////////////////////////////

void SetTextFromPluginActivity::execute ( AFW::Core::Object *object )
{
  Guard guard ( this->mutex() );

  // Get the window.
  AFW::Windows::Window::RefPtr window ( dynamic_cast < AFW::Windows::Window * > ( object ) );
  if ( false == window.valid() )
    return;

  // Lock the program because we access one of its internal pointers.
  AFW_GUARD_PROGRAM;

  // Get the listener.
  AFW::Core::PluginListener::RefPtr listener ( AFW::Core::Program::instance().pluginListener() );
  if ( false == listener.valid() )
    return;

  // Set this window's text if different.
  const std::string &text ( listener->text() );
  if ( _size != text.size() )
  {
    _size = text.size();
    window->textSet ( text );
    window->scrollToEnd();
  }
}
