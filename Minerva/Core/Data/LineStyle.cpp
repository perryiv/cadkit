
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/LineStyle.h"

using namespace Minerva::Core::Data;

USUL_IMPLEMENT_TYPE_ID ( LineStyle );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineStyle::LineStyle() : BaseClass(),
	_width ( 1.0f )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

LineStyle* LineStyle::create ( const Color& color, float width )
{
  LineStyle::RefPtr lineStyle ( new LineStyle );
  lineStyle->color ( color );
  lineStyle->width ( width );

  return lineStyle.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineStyle::~LineStyle()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the width.
//
///////////////////////////////////////////////////////////////////////////////

void LineStyle::width ( float w )
{
  Guard guard ( this->mutex() );
  _width = w;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width.
//
///////////////////////////////////////////////////////////////////////////////

float LineStyle::width() const
{
  Guard guard ( this->mutex() );
  return _width;
}
