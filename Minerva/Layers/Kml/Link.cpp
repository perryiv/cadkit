
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/Link.h"

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Link::Link() : BaseClass(),
  _href()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Link::~Link()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the href.
//
///////////////////////////////////////////////////////////////////////////////

void Link::href ( const std::string& href )
{
  _href = href;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the href.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Link::href() const
{
  return _href;
}
