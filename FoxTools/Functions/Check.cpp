
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

#include "Usul/Cast/Cast.h"

#include <stdarg.h>

#include "fxver.h"
#include "fxdefs.h"
#include "FXString.h"
#include "FXStream.h"
#include "FXWindow.h"
#include "FXMenuCheck.h"
#include "FXMenuRadio.h"
#include "FXToggleButton.h"


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
  Detail::_check ( state, menuCheck );
  Detail::_check ( state, menuRadio );
  Detail::_state ( state, toggleButton );
}
