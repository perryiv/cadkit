
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the window is visible.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Conditions/IsVisible.h"
#include "AppFrameWork/Core/Window.h"

using namespace AFW::Conditions;

USUL_IMPLEMENT_TYPE_ID ( IsVisible );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

IsVisible::IsVisible ( bool want ) : BaseClass ( want )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

IsVisible::~IsVisible()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the window is visible.
//
///////////////////////////////////////////////////////////////////////////////

bool IsVisible::evaluate ( AFW::Core::Object *object )
{
  Guard guard ( this->mutex() );
  AFW::Core::Window::RefPtr window ( dynamic_cast < AFW::Core::Window * > ( object ) );
  return ( ( window.valid() ) ? ( window->visible() == _want ) : false );
}
