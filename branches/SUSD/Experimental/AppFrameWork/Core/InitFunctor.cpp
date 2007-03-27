
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor called during program initialization.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/InitFunctor.h"

using namespace AFW::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InitFunctor::InitFunctor() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InitFunctor::~InitFunctor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default implementation does nothing.
//
///////////////////////////////////////////////////////////////////////////////

void InitFunctor::operator() ( Application * )
{
}
