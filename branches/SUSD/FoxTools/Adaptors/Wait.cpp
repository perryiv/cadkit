
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wait cursor.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Adaptors/Wait.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Functions/App.h"

using namespace FoxTools;
using namespace FoxTools::Adaptors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Wait::Wait()
{
  FoxTools::Functions::application()->beginWaitCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Wait::~Wait()
{
  FoxTools::Functions::application()->endWaitCursor();
}