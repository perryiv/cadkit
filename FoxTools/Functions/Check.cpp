
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to set the window's check.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Functions/Check.h"
#include "FoxTools/Headers/MenuCheck.h"
#include "FoxTools/Headers/MenuRadio.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/RadioButton.h"

#include "Usul/Cast/Cast.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Set the check.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > inline void _check ( bool state, T *window )
  {
    if ( window )
    {
      window->setCheck ( state );
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > inline void _state ( bool state, T *window )
  {
    if ( window )
    {
      window->setState ( state );
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the check.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::check ( bool state, FX::FXObject *object )
{
  FX::FXMenuCheck *menuCheck = SAFE_CAST_FOX ( FX::FXMenuCheck, object );
  FX::FXMenuRadio *menuRadio = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  FX::FXToggleButton *toggleButton = SAFE_CAST_FOX ( FX::FXToggleButton, object );
  FX::FXRadioButton *radioButton = SAFE_CAST_FOX ( FX::FXRadioButton, object );
  Detail::_check ( state, menuCheck );
  Detail::_check ( state, menuRadio );
  Detail::_check ( state, radioButton );
  Detail::_state ( state, toggleButton );
}
