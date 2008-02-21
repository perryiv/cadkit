
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/Icon.h"

using namespace OsgTools::Legend;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Icon::Icon() : BaseClass(),
  _size ( 0, 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Icon::~Icon()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the suggested size.
//
///////////////////////////////////////////////////////////////////////////////

void Icon::sizeHint ( const Size& s )
{
  _size = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the suggested size.
//
///////////////////////////////////////////////////////////////////////////////

Icon::Size Icon::sizeHint() const
{
  return _size;
}

