
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
#include "AppFrameWork/Windows/Window.h"

using namespace AFW::Conditions;

USUL_IMPLEMENT_TYPE_ID ( IsEnabled );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

IsEnabled::IsEnabled ( bool want ) : BaseClass ( want )
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

bool IsEnabled::evaluate ( AFW::Core::Object *object )
{
  Guard guard ( this->mutex() );
  AFW::Windows::Window::RefPtr window ( dynamic_cast < AFW::Windows::Window * > ( object ) );
  return ( ( window.valid() ) ? ( window->enabled() == _want ) : false );
}