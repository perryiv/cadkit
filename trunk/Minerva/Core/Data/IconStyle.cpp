
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/IconStyle.h"

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

IconStyle::IconStyle() : BaseClass(),
  _scale ( 1.0 ),
  _heading ( 0.0 ),
  _href()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

IconStyle::~IconStyle()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
///////////////////////////////////////////////////////////////////////////////

void IconStyle::scale ( double scale )
{
  _scale = scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale.
//
///////////////////////////////////////////////////////////////////////////////

double IconStyle::scale() const
{
  return _scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the heading.
//
///////////////////////////////////////////////////////////////////////////////

void IconStyle::heading ( double heading )
{
  _heading = heading;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the heading.
//
///////////////////////////////////////////////////////////////////////////////

double IconStyle::heading() const
{
  return _heading;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the href.
//
///////////////////////////////////////////////////////////////////////////////

void IconStyle::href ( const std::string& href )
{
  _href = href;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the href.
//
///////////////////////////////////////////////////////////////////////////////

std::string IconStyle::href() const
{
  return _href;
}