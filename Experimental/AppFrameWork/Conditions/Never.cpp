
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Condition that is never true.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Conditions/Never.h"

using namespace AFW::Conditions;

USUL_IMPLEMENT_TYPE_ID ( Never );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Never::Never() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Never::~Never()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  The condition is always met.
//
///////////////////////////////////////////////////////////////////////////////

bool Never::evaluate ( AFW::Core::Object * )
{
  return true;
}
