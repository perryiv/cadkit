
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/Icon.h"

using namespace OsgTools::Legend;


Icon::Icon() : BaseClass(),
_width( 0 ),
_height ( 0 )
{
}

Icon::~Icon()
{
}


void Icon::width( unsigned int w )
{
  _width = w;
}

unsigned int Icon::width() const
{
  return _width;
}

void Icon::height( unsigned int h )
{
  _height = h;
}

unsigned int Icon::height() const
{
  return _height;
}
