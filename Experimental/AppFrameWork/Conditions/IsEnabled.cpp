
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the window is enabled.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Conditions/IsEnabled.h"
#include "AppFrameWork/Core/Window.h"
#include "AppFrameWork/Core/Application.h"

using namespace AFW::Conditions;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

IsEnabled::IsEnabled() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

IsEnabled::~IsEnabled()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the window is enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool IsEnabled::evaluate ( AFW::Core::Object *object, Usul::Base::Referenced *data )
{
  AFW::Core::Window::RefPtr window ( dynamic_cast < AFW::Core::Window * > ( object ) );
  return ( ( window.valid() ) ? AFW::Core::Application::instance().guiServer()->isWindowEnabled ( window.get(), data ) : false );
}
