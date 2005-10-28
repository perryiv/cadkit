
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all actions.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Actions/Action.h"

using namespace AFW::Actions;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Action::Action() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Action::~Action()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default action cannot be undone.
//
///////////////////////////////////////////////////////////////////////////////

bool Action::canUndo() const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default implementation of undo does nothing.
//
///////////////////////////////////////////////////////////////////////////////

void Action::undo()
{
}
