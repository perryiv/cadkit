
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Check the control.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Actions/Check.h"

#include "AppFrameWork/Menus/Button.h"

using namespace AFW::Actions;

USUL_IMPLEMENT_TYPE_ID ( Check );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Check::Check ( AFW::Core::Object *obj, bool state ) : BaseClass(),
  _state ( state ),
  _object ( obj )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Check::~Check()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform the action.
//
///////////////////////////////////////////////////////////////////////////////

void Check::execute ( AFW::Core::Object *object )
{
  Guard guard ( this->mutex() );

  // First try member object.
  if ( _object.valid() )
  {
    AFW::Menus::Button::RefPtr button ( dynamic_cast < AFW::Menus::Button * > ( _object.get() ) );
    if ( button.valid() )
    {
      button->check ( _state );
      return;
    }
  }

  // Set check for given object.
  if ( object )
  {
    AFW::Menus::Button::RefPtr button ( dynamic_cast < AFW::Menus::Button * > ( object ) );
    if ( button.valid() )
      button->check ( _state );
  }
}
