
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the window's text from the contents of the given file.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Actions/SetTextFromStdout.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Windows/Window.h"

using namespace AFW::Actions;

USUL_IMPLEMENT_TYPE_ID ( SetTextFromStdout );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SetTextFromStdout::SetTextFromStdout() : BaseClass(),
  _start ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SetTextFromStdout::~SetTextFromStdout()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform the action.
//
///////////////////////////////////////////////////////////////////////////////

void SetTextFromStdout::execute ( AFW::Core::Object *object )
{
  Guard guard ( this->mutex() );

  // Get the window.
  AFW::Windows::Window::RefPtr window ( dynamic_cast < AFW::Windows::Window * > ( object ) );
  if ( false == window.valid() )
    return;

  // Get recent text.
  std::string buffer;
  const unsigned int start ( AFW::Core::Program::instance().standardOutGet ( _start, buffer ) );

  // Handle nothing new.
  if ( start == _start || buffer.empty() )
    return;

  // Update starting point.
  _start = start;

  // Append to this window's text.
  window->textAppend ( buffer );

  // Scroll to the end.
  window->scrollToEnd();
}
