
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to delete windows.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Functions/Delete.h"

#include <stdarg.h>

#include "fxver.h"
#include "fxdefs.h"
#include "FXString.h"
#include "FXStream.h"
#include "FXWindow.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all the child windows.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::deleteChildren ( FX::FXWindow *window )
{
  while ( window && window->numChildren() )
    delete window->childAtIndex ( 0 );
}
