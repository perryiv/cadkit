
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Condition that is always true.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Conditions/Always.h"

using namespace AFW::Conditions;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Always::Always() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Always::~Always()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  The condition is always met.
//
///////////////////////////////////////////////////////////////////////////////

bool Always::evaluate()
{
  return true;
}
